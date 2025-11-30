#!/usr/bin/env python3
"""
Crash log analyzer for GTFOHax
Maps crash addresses to source code locations with context
"""

import argparse
import subprocess
import re
import os
from pathlib import Path
from typing import Optional, List, Dict, Tuple


class CrashAnalyzer:
    def __init__(self, dll_binary: str):
        self.dll_path = Path(dll_binary)
        self.image_base = None
        self.workspace = Path(__file__).parent.parent

    def extract_image_base(self) -> Optional[int]:
        """Extract PE ImageBase using objdump."""
        try:
            result = subprocess.run(
                ['objdump', '-p', str(self.dll_path)],
                capture_output=True,
                text=True,
                check=True
            )

            for line in result.stdout.splitlines():
                if 'ImageBase' in line:
                    tokens = line.split()
                    if len(tokens) >= 2:
                        return int(tokens[-1], 16)

            print("[WARNING] ImageBase not found in PE header")
            return None

        except subprocess.CalledProcessError as e:
            print(f"[ERROR] objdump failed: {e}")
            return None

    def locate_crash_log(self) -> Optional[Path]:
        """Find most recent GTFO crash log."""
        steam_paths = [
            Path.home() / ".steam/debian-installation/steamapps/compatdata/493520/pfx/drive_c/users/steamuser/AppData/Local/Temp",
            Path.home() / ".local/share/Steam/steamapps/compatdata/493520/pfx/drive_c/users/steamuser/AppData/Local/Temp",
        ]

        for base_path in steam_paths:
            if not base_path.exists():
                continue

            # Look for Player.log in crash directories
            crash_dirs = list(base_path.glob("**/Crashes/Crash_*"))

            player_logs = []
            for crash_dir in crash_dirs:
                player_log = crash_dir / "Player.log"
                if player_log.exists():
                    player_logs.append(player_log)

            if player_logs:
                latest = max(player_logs, key=lambda p: p.stat().st_mtime)
                print(f"[INFO] Found crash log: {latest}")
                return latest

        print("[ERROR] No crash logs found")
        return None

    def resolve_address_to_source(self, file_offset: int) -> Dict[str, str]:
        """Use addr2line to map address to source location."""
        result = subprocess.run(
            ['addr2line', '-e', str(self.dll_path), '-f', '-C', '-p', f'0x{file_offset:x}'],
            capture_output=True,
            text=True
        )

        info = result.stdout.strip()
        location_match = re.search(r'at (.+):(\d+)', info)

        if location_match:
            return {
                'file': location_match.group(1),
                'line': int(location_match.group(2)),
                'raw': info
            }

        return {'raw': info}

    def extract_source_snippet(self, filepath: str, target_line: int, radius: int = 3) -> str:
        """Extract source code around crash location."""
        source_path = Path(filepath)

        if not source_path.exists():
            return f"[Source file not accessible: {filepath}]"

        try:
            with open(source_path, 'r') as f:
                lines = f.readlines()

            start = max(0, target_line - radius - 1)
            end = min(len(lines), target_line + radius)

            snippet = []
            for idx in range(start, end):
                marker = ">>>" if idx == target_line - 1 else "   "
                snippet.append(f"{marker} {idx + 1:4d} | {lines[idx].rstrip()}")

            return '\n'.join(snippet)

        except IOError as e:
            return f"[Failed to read source: {e}]"

    def disassemble_at_offset(self, offset: int, context: int = 8) -> str:
        """Disassemble code around crash offset."""
        result = subprocess.run(
            ['objdump', '-d', '--start-address', f'{offset - context * 4}',
             '--stop-address', f'{offset + context * 4}', str(self.dll_path)],
            capture_output=True,
            text=True
        )

        if result.returncode != 0:
            return "[Disassembly unavailable]"

        lines = result.stdout.splitlines()
        highlighted = []
        target_hex = f"{offset:x}"

        for line in lines:
            if target_hex in line.split(':')[0] if ':' in line else '':
                highlighted.append(f">>> {line}")
            else:
                highlighted.append(f"    {line}")

        return '\n'.join(highlighted)

    def find_nearest_symbol(self, file_offset: int) -> Optional[Tuple[str, int]]:
        """Find nearest function symbol below the crash address."""
        result = subprocess.run(
            ['nm', '--demangle', '--numeric-sort', str(self.dll_path)],
            capture_output=True,
            text=True
        )

        if result.returncode != 0:
            return None

        closest_symbol = None
        closest_addr = 0

        for line in result.stdout.splitlines():
            parts = line.split(maxsplit=2)
            if len(parts) < 3 or parts[1].upper() not in ['T', 'W']:
                continue

            try:
                sym_addr = int(parts[0], 16)
                if sym_addr <= file_offset and sym_addr > closest_addr:
                    closest_addr = sym_addr
                    closest_symbol = parts[2]
            except ValueError:
                continue

        if closest_symbol:
            return (closest_symbol, file_offset - closest_addr)

        return None

    def analyze_log(self, log_path: Path):
        """Main analysis routine."""
        print(f"[INFO] Analyzing: {log_path}")

        self.image_base = self.extract_image_base()
        if not self.image_base:
            print("[ERROR] Cannot proceed without ImageBase")
            return

        print(f"[INFO] ImageBase: 0x{self.image_base:x}")

        with open(log_path, 'r', errors='ignore') as f:
            log_content = f.read()

        dll_name = self.dll_path.stem

        # Find the module base address from the log
        # Format: GTFOHax.dll:GTFOHax.dll (00006FFFE3860000), size: 97964032
        module_base = None
        module_pattern = re.compile(
            rf'{dll_name}\.dll.*?\(([0-9A-Fa-f]+)\).*?size:',
            re.IGNORECASE
        )

        module_match = module_pattern.search(log_content)
        if module_match:
            module_base = int(module_match.group(1), 16)
            print(f"[INFO] Module base address: 0x{module_base:x}")
        else:
            print("[WARNING] Could not find module base address in log")
            print("[INFO] Will attempt to process addresses anyway")

        # Unity stack trace format in Player.log:
        # 0x00006FFFE3C1C728 (gtfohax) (function-name not available)
        # Match lines with our DLL name in parentheses
        stack_pattern = re.compile(
            rf'0x([0-9A-Fa-f]+)\s+\({dll_name}\)',
            re.IGNORECASE
        )

        matches = stack_pattern.findall(log_content)
        addresses = list(matches)

        if not addresses:
            print(f"[WARNING] No stack trace found for {dll_name}")
            print("[INFO] Trying alternative pattern...")
            # Try pattern without parentheses
            alt_pattern = re.compile(
                rf'{dll_name}[^\n]*?0x([0-9A-Fa-f]+)',
                re.IGNORECASE
            )
            addresses = alt_pattern.findall(log_content)

        if not addresses:
            print("[ERROR] No crash addresses found in log")
            return

        print(f"[INFO] Found {len(addresses)} stack frame(s) for {dll_name}\n")

        for idx, addr_str in enumerate(addresses[:10]):  # Limit to first 10
            addr = int(addr_str, 16)

            # Calculate file offset: runtime_addr - module_base + image_base
            if module_base:
                file_offset = addr - module_base + self.image_base
            else:
                # Fallback: try using address as-is
                file_offset = addr

            print(f"\n{'=' * 70}")
            print(f"Frame #{idx}: Runtime address: 0x{addr:x}")
            if module_base:
                print(f"  File offset: 0x{file_offset:x}")
            print(f"{'=' * 70}")

            # Map to source
            source_info = self.resolve_address_to_source(file_offset)
            if 'file' in source_info:
                print(f"\nSource Location:")
                print(f"  {source_info['file']}:{source_info['line']}")
                print(f"\nCode Context:")
                print(self.extract_source_snippet(source_info['file'], source_info['line']))
            else:
                print(f"\nSource: {source_info.get('raw', 'Unknown')}")

            # Find symbol
            symbol_info = self.find_nearest_symbol(file_offset)
            if symbol_info:
                print(f"\nNearest Symbol:")
                print(f"  {symbol_info[0]} + 0x{symbol_info[1]:x}")

            # Disassembly
            print(f"\nDisassembly:")
            print(self.disassemble_at_offset(file_offset))


def main():
    parser = argparse.ArgumentParser(
        description="Analyze GTFO crash logs and map to GTFOHax source code"
    )
    parser.add_argument(
        'log_file',
        nargs='?',
        help='Path to crash log (auto-detect if omitted)'
    )
    parser.add_argument(
        '--dll',
        default='build/GTFOHax.dll',
        help='Path to GTFOHax DLL (default: build/GTFOHax.dll)'
    )

    args = parser.parse_args()

    analyzer = CrashAnalyzer(args.dll)

    if args.log_file:
        log_path = Path(args.log_file)
    else:
        log_path = analyzer.locate_crash_log()

    if not log_path or not log_path.exists():
        print("[ERROR] Crash log not found")
        return 1

    analyzer.analyze_log(log_path)
    return 0


if __name__ == '__main__':
    exit(main())
