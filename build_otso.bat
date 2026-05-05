@echo off
call "D:\Apps\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
if %errorlevel% neq 0 exit /b %errorlevel%
cmake -S . -B build-nmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --build build-nmake --target Otso
if %errorlevel% neq 0 exit /b %errorlevel%
echo BUILD_SUCCESS
