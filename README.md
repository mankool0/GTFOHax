# Note: This was tested on a fresh Win 10 x64 & Win 11 x64 install using Visual Studio 2022 Community.

# Build Instructions

## Prerequisites 
1. Install [git](https://git-scm.com) and make sure Git from the command line is enabled. I installed with default options and it's fine.

2. Install [Visual Studio](https://visualstudio.microsoft.com/) selecting "Desktop development with C++" in "Workloads" during the install

3. Install [vcpkg](https://vcpkg.io/en/getting-started.html)
    
    Follow steps 1-2 to clone the repo and run the bootstrap script. Then run the command to use vcpkg with MSBuild: `vcpkg integrate install`

4. Clone this repo with the submodules: `git clone --recurse-submodules -j8 https://github.com/mankool0/GTFOHax.git`

    If you have errors cloning the repo due to the size of it try cloning with a depth of 1: `git clone --depth=1 --recurse-submodules -j8 https://github.com/mankool0/GTFOHax.git`

## Build With CLI

1. Open Visual Studio Command Prompt
    
    Example command using Visual Studio 2022 Community:
    `%comspec% /k "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"`

2. cd into GTFOHax root directory

3. Execute: `MSBUILD GTFOHax.sln /P:Configuration=Release`
    
    If this is your first time building and in PreBuildEvent you see "Error applying a patch" try to manually apply the failed git patch. If it's already been applied this error can be ignored.

4. Grab the .dll in `x64\Release\`

## Build With Visual Studio 

1. Open GTFOHax.sln in GTFOHax root directory with Visual Studio.

2. Change the build from Debug to Release

3. Press Build -> Build Solution and wait for the build to complete.
    
    If this is your first time building and in the build output you see "Error applying a patch" try to manually apply the failed git patch. If it's already been applied this error can be ignored.

4. Grab the .dll in `x64\Release\`

# Updating

1. Build Il2CppInspector by following steps at [Il2CppInspector](https://github.com/djkaty/Il2CppInspector?tab=readme-ov-file#build-instructions) with these changes: https://github.com/djkaty/Il2CppInspector/issues/193

2. Create a C++ scaffolding project and replace all files in appdata directory with newly generated ones.

3. Fix any errors that might show up if game functions changed.

# Contributing
Make a PR