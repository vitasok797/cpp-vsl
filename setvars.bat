@echo off

call vcvarsall.bat x64 >nul
set VSCMD_SKIP_SENDTELEMETRY=1
