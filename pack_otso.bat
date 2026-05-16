@echo off
setlocal
echo Initializing Visual Studio Environment...
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
if %errorlevel% neq 0 (
    echo Error: Could not find Visual Studio Build Tools.
    exit /b %errorlevel%
)

pushd "%~dp0build-nmake"
echo Running CPack to generate NSIS Installer...
cpack -G NSIS
if %errorlevel% neq 0 (
    echo CPack failed. Check the logs.
    popd
    exit /b %errorlevel%
)
popd
echo.
echo ========================================================
echo INSTALLER GENERATED SUCCESSFULLY
echo Location: build-nmake\Otso-2.2.1-win64.exe (atau serupa)
echo ========================================================
pause
