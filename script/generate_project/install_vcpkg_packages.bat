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

call bootstrap-vcpkg.bat

vcpkg install fmt:x64-windows                       	--recurse
vcpkg install benchmark:x64-windows                 	--recurse
vcpkg install tracy:x64-windows                     	--recurse
vcpkg install eastl:x64-windows                     	--recurse
vcpkg install concurrentqueue:x64-windows           	--recurse 
vcpkg install spdlog:x64-windows                    	--recurse
vcpkg install glm:x64-windows                       	--recurse
vcpkg install libpng:x64-windows 			        	--recurse
vcpkg install stb:x64-windows 				        	--recurse
vcpkg install nlohmann-json:x64-windows 	        	--recurse
vcpkg install vulkan-memory-allocator:x64-windows 		--recurse
vcpkg install spirv-cross:x64-windows 	            	--recurse
vcpkg install shaderc:x64-windows 	                	--recurse
vcpkg install directx-dxc:x64-windows 	            	--recurse
vcpkg install directx-headers:x64-windows 	        	--recurse
vcpkg install imgui[docking-experimental]:x64-windows 	--recurse
vcpkg install assimp:x64-windows 						--recurse

@rem reference: "https://learn.microsoft.com/en-us/vcpkg/consume/lock-package-versions?tabs=inspect-powershell"
@rem vcpkg --feature-flags=manifests --x-manifest-root="%manifest_dir%" --x-install-root=installed install

@rem vcpkg install vulkan-headers:x64-windows -v 1.3.268.0	    --recurse 
@rem vcpkg install vulkan-loader:x64-windows 				--recurse 
@rem vcpkg install vulkan-sdk-components:x64-windows 		--recurse 
@rem vcpkg install vulkan-validationlayers:x64-windows 		--recurse 
@rem vcpkg install vulkan-tools:x64-windows 			--recurse 
@rem vcpkg install vulkan-utility-libraries:x64-windows --recurse 

POPD

@pause