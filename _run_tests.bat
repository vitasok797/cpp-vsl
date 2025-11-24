@echo off

set PRESET=win-x64-debug

call msvc_setvars.bat
if errorlevel 1 goto :error

if EXIST "out\build" (
	rmdir /s /q "out\build"
	if errorlevel 1 goto :error
)

cmake --preset=%PRESET%
if %errorlevel% NEQ 0 goto :error

cmake --build --preset=%PRESET%
if %errorlevel% NEQ 0 goto :error

echo.
echo -----------------------------------------------------
echo.

out\build\%PRESET%\run_tests.exe

goto:eof

:error
echo ERROR
pause
