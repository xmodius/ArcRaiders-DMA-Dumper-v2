@echo off
echo ========================================
echo Arc Raiders DMA Dumper - Auto Setup (x64)
echo ========================================
echo.

REM Force x64 environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1

REM Check if cl.exe is available
where cl.exe >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] cl.exe not found!
    echo Please install Visual Studio 2022 with C++ tools
    echo.
    pause
    exit /b 1
)

echo [1/3] Copying DMA DLLs...
echo.

set DMA_PATH=C:\Users\enfui\Desktop\lones-DMA-speed-test-main\lones-DMA-speed-test-main

if not exist "%DMA_PATH%\vmm.dll" (
    echo [ERROR] DMA DLLs not found at: %DMA_PATH%
    echo Please update the DMA_PATH variable in this script.
    echo.
    pause
    exit /b 1
)

copy "%DMA_PATH%\*.dll" . >nul 2>&1

echo [SUCCESS] DMA DLLs copied!
echo.

echo [2/3] Compiling dumper (x64)...
echo.

cl.exe /EHsc /std:c++17 /O2 /Fe:ArcRaiders_DMA_Dumper.exe main.cpp Advapi32.lib Shell32.lib 2>compile_error.txt

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Compilation failed!
    echo.
    type compile_error.txt
    echo.
    pause
    exit /b 1
)

del compile_error.txt 2>nul

echo [SUCCESS] Compilation complete!
echo.

echo [3/3] Ready to run!
echo.
echo ========================================
echo IMPORTANT:
echo 1. Launch Arc Raiders
echo 2. Get into a match (not just menu)
echo 3. Wait 15 seconds in the match
echo 4. Press any key to run the dumper
echo ========================================
echo.
pause

echo.
echo Running dumper...
echo.

ArcRaiders_DMA_Dumper.exe

echo.
pause
