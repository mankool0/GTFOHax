@echo off
echo ========================================
echo   Building GTFOHax Complete Package
echo ========================================
echo.

REM Step 1: Build Release DLL
echo [STEP 1/4] Building GTFOHax.dll (Release)...
msbuild GTFOHax.sln /p:Configuration=Release /p:Platform=x64 /v:minimal /nologo
if errorlevel 1 (
    echo [ERROR] Failed to build DLL
    pause
    exit /b 1
)
echo.

REM Step 2: Extract version from PropertySheet.props
echo [STEP 2/4] Extracting version number...
set VERSION=
for /f "tokens=3 delims=<>" %%a in ('findstr /r "<LibraryVersion>[0-9]" GTFOHax\PropertySheet.props') do set VERSION=%%a
if "%VERSION%"=="" (
    echo [ERROR] Failed to extract version number
    pause
    exit /b 1
)
echo [INFO] Version: %VERSION%
echo.

REM Step 3: Find the DLL file
set DLL_FILE=
if exist "x64\Release\GTFOHax-v%VERSION%.dll" (
    set DLL_FILE=x64\Release\GTFOHax-v%VERSION%.dll
) else if exist "x64\Release\GTFOHax.dll" (
    set DLL_FILE=x64\Release\GTFOHax.dll
) else (
    echo [ERROR] DLL not found in x64\Release\
    pause
    exit /b 1
)
echo [INFO] Found DLL: %DLL_FILE%
echo.

REM Step 4: Copy DLL to a fixed name for resource embedding
echo [STEP 3/4] Preparing DLL for embedding...
copy /Y "%DLL_FILE%" "GTFOHax.dll" >nul
if errorlevel 1 (
    echo [ERROR] Failed to copy DLL
    pause
    exit /b 1
)

REM Compile resource file
rc.exe injector.rc >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Failed to compile resource file
    pause
    exit /b 1
)

REM Step 5: Compile injector with version in filename
echo [STEP 4/4] Compiling GTFOHax-Injector-v%VERSION%.exe...
cl.exe /nologo /std:c++20 /EHsc /O2 /MD /Fe:GTFOHax-Injector-v%VERSION%.exe injector.cpp injector.res /link user32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64
if errorlevel 1 (
    echo [ERROR] Compilation failed
    pause
    exit /b 1
)

REM Cleanup temporary files
del /Q injector.obj injector.res GTFOHax.dll 2>nul
echo.

REM Step 6: Compress with UPX
echo [STEP 5/5] Compressing with UPX...
where upx >nul 2>&1
if errorlevel 1 (
    echo [WARNING] UPX not found in PATH, skipping compression
    echo [INFO] Download UPX from: https://upx.github.io/
) else (
    upx --best --lzma GTFOHax-Injector-v%VERSION%.exe
    if errorlevel 1 (
        echo [WARNING] UPX compression failed, keeping uncompressed version
    ) else (
        echo [OK] Compression successful
    )
)

echo.
echo ========================================
echo   Build Complete!
echo ========================================
echo.
echo Output files:
echo   - x64\Release\GTFOHax-v%VERSION%.dll
echo   - GTFOHax-Injector-v%VERSION%.exe (DLL embedded, UPX compressed)
echo.
pause

