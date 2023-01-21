@REM #change current directory to this file

@rem change driver letter
@%~d0

@rem change current directory
@cd %~dp0

set dir=..\..\external\vcpkg

mkdir %dir%

git clone --recurse https://github.com/microsoft/vcpkg.git %dir%

PUSHD %dir%

git pull

call bootstrap-vcpkg.bat

vcpkg install fmt:x64-windows --recurse

POPD

@pause