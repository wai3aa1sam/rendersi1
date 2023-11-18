cmake_minimum_required(VERSION 3.18)

set(CMAKE_TOOLCHAIN_FILE "./external/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(CMAKE_CXX_STANDARD 17)

include(cmake_utility.cmake)

#set(CMAKE_BUILD_TYPE Debug)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(project_name rendersi1)
set(project_namespace rds)
set(project_root ${CMAKE_CURRENT_SOURCE_DIR})
set(project_namespace_marco RDS)

project(${project_name})
my_set_opt(${project_namespace_marco}_ENABLE_PROFILER           1)
my_set_opt(${project_namespace_marco}_ENABLE_SANITIZER          0)

find_package(fmt                    CONFIG REQUIRED)
find_package(benchmark              CONFIG REQUIRED)

# --- vulkan
find_package(Vulkan)
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

# --- git_submodule ---
my_add_git_submodule(external/nmsplib)

message("--- rendersi1 ---")
# --- core ---
add_subdirectory(src/core/base)
add_subdirectory(src/core)

#-- render --
add_subdirectory(src/render)
add_subdirectory(src/render/api_layer)
add_subdirectory(src/render/shader_compiler)

#-- engine
add_subdirectory(src/engine)

#-- editor
add_subdirectory(src/editor)
add_subdirectory(src/editor/app)

message("--- rendersi1_test ---")
#test
add_subdirectory(src/test/base)
add_subdirectory(src/test/core)
add_subdirectory(src/test/render)
add_subdirectory(src/test/engine)
add_subdirectory(src/test/editor)


message("--- end for add_subdirectory ---")

set_property (DIRECTORY PROPERTY VS_STARTUP_PROJECT ${project_namespace}_core_test)