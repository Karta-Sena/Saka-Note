@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
if %errorlevel% neq 0 exit /b %errorlevel%
pushd "%~dp0"
cmake -S . -B build-nmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 (popd & exit /b %errorlevel%)
cmake --build build-nmake --target Otso
if %errorlevel% neq 0 (popd & exit /b %errorlevel%)
popd
echo BUILD_SUCCESS
