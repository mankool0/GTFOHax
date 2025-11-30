#!/usr/bin/env python3
"""Symbol loader for debugging Windows PE DLLs on Linux via GDB.

Handles runtime symbol relocation for injected DLLs running under Wine/Proton.
"""

import subprocess
import os
import gdb


class DllSymbolLoader(gdb.Command):
    """Loads debug symbols from PE DLL files with runtime address mapping."""

    def __init__(self):
        super().__init__("load-symbols", gdb.COMMAND_FILES)
        workspace = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        self.default_dll = os.path.join(workspace, "build", "GTFOHax.dll")

    def invoke(self, arguments, from_tty):
        dll = gdb.string_to_argv(arguments)[0] if arguments else self.default_dll

        if not os.path.exists(dll):
            print(f"ERROR: Cannot locate DLL at {dll}")
            return

        try:
            self._process_dll(dll)
        except Exception as err:
            print(f"Symbol loading failed: {err}")
            import traceback
            traceback.print_exc()

    def _process_dll(self, dll_file):
        """Main symbol loading routine."""
        print(f"Processing: {dll_file}")

        # Extract section metadata
        section_data = self._extract_sections(dll_file)
        if '.text' not in section_data:
            raise RuntimeError("Missing .text section")

        base_vma = section_data['.text']['vma']

        # Locate DLL in process memory
        mem_regions = self._scan_process_memory(dll_file)
        if not mem_regions:
            raise RuntimeError(f"{os.path.basename(dll_file)} not loaded in process")

        # Find executable region
        exec_addr = self._locate_code_section(mem_regions)
        if not exec_addr:
            raise RuntimeError("Cannot determine code section address")

        offset = exec_addr - base_vma

        # Construct GDB command
        gdb_cmd = [f"add-symbol-file {dll_file} {hex(exec_addr)}"]

        for name, info in section_data.items():
            if name == '.text':
                continue
            relocated = info['vma'] + offset
            gdb_cmd.append(f"-s {name} {hex(relocated)}")

        final_cmd = " ".join(gdb_cmd)
        print(f"Executing: {final_cmd}")
        gdb.execute(final_cmd)
        print("Symbol loading complete")

    def _extract_sections(self, binary):
        """Use objdump to extract PE section information."""
        proc = subprocess.run(
            ['objdump', '-h', binary],
            capture_output=True,
            text=True,
            check=True
        )

        sections = {}
        parsing = False

        for line in proc.stdout.splitlines():
            if "Sections:" in line:
                parsing = True
                continue

            if not parsing or "Idx Name" in line:
                continue

            tokens = line.strip().split()
            if len(tokens) >= 6 and tokens[0].isdigit() and tokens[1].startswith('.'):
                try:
                    sections[tokens[1]] = {
                        'size': int(tokens[2], 16),
                        'vma': int(tokens[3], 16)
                    }
                except (ValueError, IndexError):
                    pass

        return sections

    def _scan_process_memory(self, target_dll):
        """Query process mappings to find DLL regions."""
        output = gdb.execute("info proc mappings", to_string=True)
        basename = os.path.basename(target_dll)

        regions = []
        found_dll = False

        for line in output.splitlines():
            if "Start Addr" in line or not line.strip():
                continue

            parts = line.strip().split()
            if len(parts) < 5:
                continue

            try:
                region = {
                    'start': int(parts[0], 16),
                    'end': int(parts[1], 16),
                    'size': int(parts[2], 16),
                    'offset': int(parts[3], 16),
                    'perms': parts[4],
                    'path': ' '.join(parts[5:]) if len(parts) > 5 else ''
                }

                if basename.lower() in region['path'].lower():
                    regions.append(region)
                    found_dll = True
                elif found_dll and not region['path']:
                    # Anonymous regions adjacent to DLL
                    regions.append(region)
            except (ValueError, IndexError):
                continue

        return regions

    def _locate_code_section(self, regions):
        """Find the executable memory region."""
        for r in regions:
            if 'x' in r['perms']:
                return r['start']
        return regions[0]['start'] if regions else None


# Initialize command
symbol_loader = DllSymbolLoader()

# Auto-load state
_loaded = False


def _on_debugger_stop(evt):
    """Auto-load symbols when debugger stops."""
    global _loaded
    if _loaded:
        return

    try:
        proc_info = gdb.execute("info proc", to_string=True)
        if "process" in proc_info.lower():
            _loaded = True
            print("Auto-loading symbols...")
            symbol_loader.invoke("", False)
    except:
        pass


# Register auto-loader
try:
    gdb.events.stop.connect(_on_debugger_stop)
except:
    print("Warning: Auto-load registration failed")
