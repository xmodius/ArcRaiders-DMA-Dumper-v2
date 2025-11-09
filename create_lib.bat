@echo off
REM Create vmm.lib from vmm.dll for linking

echo Creating vmm.lib import library...

REM Check if dumpbin exists
where dumpbin >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] dumpbin not found! Run from Developer Command Prompt.
    exit /b 1
)

REM Check if vmm.dll exists
if not exist "vmm.dll" (
    echo [ERROR] vmm.dll not found!
    exit /b 1
)

REM Export functions from DLL
dumpbin /EXPORTS vmm.dll > vmm_exports.txt

REM Create DEF file
echo LIBRARY vmm > vmm.def
echo EXPORTS >> vmm.def
findstr /R /C:"VMMDLL_" vmm_exports.txt | findstr /V /C:"Summary" >> vmm.def

REM Create LIB file
lib /DEF:vmm.def /OUT:vmm.lib /MACHINE:X86

REM Cleanup
del vmm_exports.txt vmm.def vmm.exp

echo [SUCCESS] vmm.lib created!
