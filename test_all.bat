@echo off
set PATH=C:\msys64\ucrt64\bin;%PATH%

echo === BUILDING COMPILER ===
mingw32-make clean
mingw32-make
if %errorlevel% neq 0 (
    echo Build failed!
    exit /b %errorlevel%
)

echo.
echo === RUNNING ALL TESTS ===
for %%f in (*.pas) do (
    echo.
    echo ========================================
    echo TESTING: %%f
    echo ========================================
    compiler.exe "%%f"
)

echo.
echo === ALL TESTS COMPLETED ===
