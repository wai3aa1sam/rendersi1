@REM #change current directory to this file

@rem change driver letter
@%~d0

@rem change current directory
@cd %~dp0

@cd ../../

set nmsplib_dir=external\nmsplib
mkdir %nmsplib_dir%
git clone --recurse https://github.com/wai3aa1sam/nmsplib.git %nmsplib_dir%
PUSHD %nmsplib_dir%
git checkout main
git pull
POPD

cmake -G "Visual Studio 17 2022" -B ./build/rendersi1-x64-windows .

@pause