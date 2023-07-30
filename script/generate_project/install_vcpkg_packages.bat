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

vcpkg install fmt:x64-windows                       --recurse
vcpkg install benchmark:x64-windows                 --recurse
vcpkg install tracy:x64-windows                     --recurse
vcpkg install eastl:x64-windows                     --recurse
vcpkg install concurrentqueue:x64-windows           --recurse 
vcpkg install spdlog:x64-windows                    --recurse
vcpkg install glm:x64-windows                       --recurse
vcpkg install libpng:x64-windows 			        --recurse
vcpkg install stb:x64-windows 				        --recurse
vcpkg install nlohmann-json:x64-windows 	        --recurse
vcpkg install vulkan-memory-allocator:x64-windows 	--recurse
vcpkg install spirv-cross:x64-windows 	            --recurse

POPD

@pause