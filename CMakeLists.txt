cmake_minimum_required(VERSION 3.18)

set(CMAKE_TOOLCHAIN_FILE "./external/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(CMAKE_CXX_STANDARD 17)

include(cmake_utility.cmake)

#set(CMAKE_BUILD_TYPE Debug)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(project_name rendersi1)
set(project_namespace rds)
set(project_root ${CMAKE_CURRENT_SOURCE_DIR})

project(${project_name})

#find_package(fmt CONFIG REQUIRED)

# --- git_submodule ---
my_add_git_submodule(external/nmsplib)

# --- core ---
add_subdirectory(src/core)
add_subdirectory(src/core/test)

set_property (DIRECTORY PROPERTY VS_STARTUP_PROJECT ${project_namespace}_core_test)