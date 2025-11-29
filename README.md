# Build Instructions

## Prerequisites
1. Install [git](https://git-scm.com) and make sure Git from the command line is enabled.

2. Clone this repo with submodules:
   ```bash
   git clone --recurse-submodules -j8 https://github.com/mankool0/GTFOHax.git
   ```

   If you have errors due to repo size, clone with depth 1:
   ```bash
   git clone --depth=1 --recurse-submodules -j8 https://github.com/mankool0/GTFOHax.git
   ```

## Build With CMake (Linux/MinGW)

### Prerequisites
- CMake 3.31+
- MinGW-w64 cross-compiler
- MinGW-w64 FreeType library
- pkg-config

**Fedora/RHEL:**
```bash
sudo dnf install cmake mingw64-gcc-c++ mingw64-freetype-static mingw64-libpng-static mingw64-zlib-static mingw64-bzip2-static
```

**Debian/Ubuntu:**
> **Note:** Debian/Ubuntu do not provide pre-built MinGW-w64 libraries for FreeType, libpng, or bzip2. You will need to either disable FreeType support or build the libraries manually from source.

```bash
# Install base toolchain (libraries need additional setup - see note above)
sudo apt install cmake g++-mingw-w64-x86-64 libz-mingw-w64-dev pkg-config
```

**Arch Linux:**
> **Note:** MinGW-w64 library packages are in the AUR (Arch User Repository), not official repos. You'll need an AUR helper like `yay` or `paru`.

```bash
# Install from official repos
sudo pacman -S cmake mingw-w64-gcc

# Install from AUR (requires AUR helper like yay)
yay -S mingw-w64-freetype2 mingw-w64-libpng mingw-w64-bzip2 mingw-w64-zlib
```

### Build
```bash
# Configure (Debug)
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Configure (Release)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build -j$(nproc)
```

Output locations:
- **Debug**: `build/GTFOHax.dll`
- **Release**: `build/GTFOHax-v{version}.dll`

## Build With Visual Studio (Windows)

### Prerequisites
1. [Visual Studio 2026](https://visualstudio.microsoft.com/) with "Desktop development with C++" workload
2. [vcpkg](https://vcpkg.io/en/getting-started.html) - Follow steps 1-2 to clone and bootstrap, then run: `vcpkg integrate install`

### Build With MSBuild CLI

1. Open Visual Studio Developer Command Prompt:
   ```cmd
   %comspec% /k "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat"
   ```

2. Build:
   ```cmd
   cd GTFOHax
   msbuild GTFOHax.sln /P:Configuration=Release
   ```

3. Output: `x64\Release\GTFOHax.dll`

### Build With Visual Studio GUI

1. Open `GTFOHax.sln`
2. Set configuration to Release
3. Build -> Build Solution
4. Output: `x64\Release\GTFOHax.dll`

**Note:** If this is your first time building and you see "Error applying a patch" in PreBuildEvent or build output, try to manually apply the failed git patch. If it's already been applied this error can be ignored.

# Updating

1. Build Il2CppInspector from [mankool0/Il2CppInspector](https://github.com/mankool0/Il2CppInspector) (includes necessary patches) or use the [upstream version](https://github.com/djkaty/Il2CppInspector) with [these changes](https://github.com/djkaty/Il2CppInspector/issues/193).

2. Generate C++ scaffolding projects for both compilers:
   - Create a project targeting **GCC** and place files in `GTFOHax/appdata/gcc/`
   - Create a project targeting **MSVC** and place files in `GTFOHax/appdata/msvc/`

   The build system automatically selects the correct files based on the compiler being used.

3. Fix any errors that might show up if game functions changed.

# Contributing
Make a PR