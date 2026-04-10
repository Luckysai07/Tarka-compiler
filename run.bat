@echo off
echo Setting up compiler path...
set PATH=C:\msys64\ucrt64\bin;%PATH%

echo.
echo === COMPILING PROJECT ===
mingw32-make
if %errorlevel% neq 0 (
    echo Compilation Failed!
    pause
    exit /b %errorlevel%
)

echo.
echo === RUNNING COMPILER ON test1.pas ===
compiler.exe test1.pas

echo.
pause
