set BUILD_DIR="build_vs2017_uwp_x64"

@echo off
FOR /F "TOKENS=1,2,*" %%A IN ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\VisualStudio\SxS\VS7" /v "15.0"') DO IF "%%A"=="15.0" SET VSPATH=%%C
@echo on
set CMAKE="%VSPATH%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"

@echo off
FOR /F "TOKENS=1,2,*" %%A IN ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\VisualStudio\SxS\VS7" /v "15.0"') DO IF "%%A"=="15.0" SET VSPATH=%%C
@echo on
set MSBUILD="%VSPATH%\MSBuild\15.0\Bin\msbuild.exe"

if not exist %BUILD_DIR% mkdir %BUILD_DIR%
pushd %BUILD_DIR%
%CMAKE% -DCMAKE_SYSTEM_NAME=WindowsStore -DCMAKE_SYSTEM_VERSION=10.0 -DCMAKE_C_FLAGS=/ZW /EHsc /DWIN32=1 -DCMAKE_CXX_FLAGS=/ZW /EHsc /DWIN32=1 -G "Visual Studio 15 2017 Win64" ..
@rem %MSBUILD% INSTALL.vcxproj /p:Configuration=Release
popd

