@REM #change current directory to this file

@rem change driver letter
@%~d0

@rem change current directory
@cd %~dp0

set dir=..\..\external\vcpkg
set manifest_dir=..\..\script\generate_project

mkdir %dir%

git clone --recurse https://github.com/microsoft/vcpkg.git %dir%

PUSHD %dir%

git pull

@rem since --x-manifest-root, --x-install-root=installed, ... are experimental, and we are using them
@rem this SHA should sync %manifest_dir%/vcpkg.json
git checkout 772f784ba8d1add92420bd6b81d4cd6a91fb36ac

call bootstrap-vcpkg.bat

@rem old method, but this do not support install specific version
@REM vcpkg install fmt:x64-windows                       	--recurse
@REM vcpkg install benchmark:x64-windows                 	--recurse
@REM vcpkg install tracy:x64-windows                     	--recurse
@REM vcpkg install eastl:x64-windows                     	--recurse
@REM vcpkg install concurrentqueue:x64-windows           	--recurse 
@REM vcpkg install spdlog:x64-windows                    	--recurse
@REM vcpkg install glm:x64-windows                       	--recurse
@REM vcpkg install libpng:x64-windows 			        	--recurse
@REM vcpkg install stb:x64-windows 				        	--recurse
@REM vcpkg install nlohmann-json:x64-windows 	        	--recurse
@REM vcpkg install vulkan-memory-allocator:x64-windows 		--recurse
@REM vcpkg install spirv-cross:x64-windows 	            	--recurse
@REM vcpkg install shaderc:x64-windows 	                	--recurse
@REM vcpkg install directx-dxc:x64-windows 	            	--recurse
@REM vcpkg install directx-headers:x64-windows 	        	--recurse
@REM vcpkg install imgui[docking-experimental]:x64-windows 	--recurse
@REM vcpkg install assimp:x64-windows 						--recurse

@rem vulkan sdk
@rem vcpkg install vulkan-headers:x64-windows -v 1.3.268.0	--recurse 
@rem vcpkg install vulkan-loader:x64-windows 				--recurse 
@rem vcpkg install vulkan-sdk-components:x64-windows 		--recurse 
@rem vcpkg install vulkan-validationlayers:x64-windows 		--recurse 
@rem vcpkg install vulkan-tools:x64-windows 			    --recurse 
@rem vcpkg install vulkan-utility-libraries:x64-windows     --recurse 

@rem reference: "https://learn.microsoft.com/en-us/vcpkg/consume/lock-package-versions?tabs=inspect-powershell"
vcpkg install --feature-flags=manifests --x-manifest-root="%manifest_dir%" --x-install-root=installed 

POPD

@pause