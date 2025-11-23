@echo off

set VSCMD_SKIP_SENDTELEMETRY=1

if not "%VCINSTALLDIR%"=="" exit /b 0

call vcvarsall.bat x64 >nul
