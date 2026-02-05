@echo off
setlocal

:: -------------------------------
:: CONFIGURATION
:: -------------------------------
set "SRC_PATH=src"
set "BUILD_PATH=build"
set "LIB_PATH=lib"
set "INCLUDE_PATH=include"
set "EXE_NAME=main.exe"

:: Create build folder if it doesn't exist
if not exist "%BUILD_PATH%" mkdir "%BUILD_PATH%"

:: -------------------------------
:: SETUP VISUAL STUDIO ENVIRONMENT
:: -------------------------------
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Failed to initialize Visual Studio environment.
    pause
    exit /b 1
)

:: -------------------------------
:: COMPILE
:: -------------------------------
echo Compiling...

:: Put .obj files into build folder with /Fo
cl /EHsc /MD /std:c++17 /I"%INCLUDE_PATH%" /Fo"%BUILD_PATH%\\" /Fe"%BUILD_PATH%\%EXE_NAME%" "%SRC_PATH%\*.cpp" /link /LIBPATH:"%LIB_PATH%" ^
    raylib.lib ^
    opengl32.lib gdi32.lib user32.lib kernel32.lib winmm.lib shell32.lib advapi32.lib


:: -------------------------------
:: CHECK BUILD SUCCESS
:: -------------------------------
if errorlevel 1 (
    echo.
    echo [BUILD FAILED] Fix errors above.
    pause
    exit /b 1
)

echo.
echo Build successful: %BUILD_PATH%\%EXE_NAME%
echo Running...
"%BUILD_PATH%\%EXE_NAME%"

pause
