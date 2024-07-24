cmake_minimum_required(VERSION 3.25)

set(CMAKE_TOOLCHAIN_FILE "./external/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(CMAKE_CXX_STANDARD 17)

include(cmake_utility.cmake)

set(CMAKE_BUILD_TYPE Release)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(project_name            rendersi1)
set(project_namespace       rds)
set(project_root            ${CMAKE_CURRENT_SOURCE_DIR})
set(project_namespace_marco RDS)

project(${project_name})

set(vulkan_minVer 1.3)

set(Project_enableSanitizer 0)

set(Profiler_enableProfiler         1)
set(Profiler_enableInternalProfiler 0)

set(JobSystem_isDevelopment       1)
set(JobSystem_kMaxFrameInFlight   4)

find_package(fmt                    CONFIG REQUIRED)
find_package(benchmark              CONFIG REQUIRED)
find_package(imgui                  CONFIG REQUIRED)
find_package(assimp                 CONFIG REQUIRED)

# --- vulkan
Vulkan_findPackage(${project_namespace_marco} ${vulkan_minVer} "")  # C:/VulkanSDK/1.3.283.0
find_package(VulkanMemoryAllocator  CONFIG REQUIRED)

# --- dx12
find_package(directx-headers        CONFIG REQUIRED)

# --- shader
find_package(spirv_cross_core       CONFIG REQUIRED)
find_package(spirv_cross_glsl       CONFIG REQUIRED)
find_package(spirv_cross_hlsl       CONFIG REQUIRED)
find_package(spirv_cross_msl        CONFIG REQUIRED)
find_package(unofficial-shaderc     CONFIG REQUIRED)
find_package(directx-dxc            CONFIG REQUIRED)

# message("${Vulkan_sdkRoot}/bin: \"${Vulkan_sdkRoot}/bin\"")
# set(ENV{VK_ADD_LAYER_PATH} "${Vulkan_sdkRoot}/bin")
# message("VK_ADD_LAYER_PATH: ${VK_ADD_LAYER_PATH}, ENV{VK_ADD_LAYER_PATH}: $ENV{VK_ADD_LAYER_PATH}")

# --- git_submodule ---
my_add_git_submodule(external/nmsplib)

message("--- rendersi1 ---")

# --- core ---
add_subdirectory(src/core/base)
add_subdirectory(src/core)

#--- render ---
add_subdirectory(src/render)
add_subdirectory(src/render/api_layer)
add_subdirectory(src/render/shader_compiler)

#--- engine
add_subdirectory(src/engine)

#--- editor
add_subdirectory(src/editor)
add_subdirectory(src/editor/editor_app)

#--- demo
add_subdirectory(src/demo)
add_subdirectory(src/demo/hello_triangle)
add_subdirectory(src/demo/forward_plus)
add_subdirectory(src/demo/voxel_cone_tracing)
add_subdirectory(src/demo/pbr_ibl)

message("--- rendersi1_test ---")
#--- test
add_subdirectory(src/test/base)
add_subdirectory(src/test/core)
add_subdirectory(src/test/render)
add_subdirectory(src/test/engine)
add_subdirectory(src/test/editor)

message("--- end for add_subdirectory ---")

set_property (DIRECTORY PROPERTY VS_STARTUP_PROJECT ${project_namespace}_forward_plus)