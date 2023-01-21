
# references
# https://github.com/SimpleTalkCpp/SimpleGameEngine
# https://www.scivision.dev/cmake-git-submodule/

function(my_print_var variable)
message(STATUS "${variable}")
endfunction()

function(my_source_group src_path src_files)
  foreach(FILE ${src_files}) 
      get_filename_component(PARENT_DIR "${FILE}" PATH)
      file(RELATIVE_PATH PARENT_DIR ${src_path} ${PARENT_DIR})
      string(REPLACE "/" "\\" GROUP "${PARENT_DIR}")
      set(GROUP "${GROUP}")
      source_group("${GROUP}" FILES "${FILE}")
  endforeach()
  
endfunction()

function(my_set_target_warning_level target_name)
  if(MSVC)
    target_compile_options(${target_name} PRIVATE /WX)     #warning treated as error
    target_compile_options(${target_name} PRIVATE /W4)     #warning level 4
    target_compile_options(${target_name} PRIVATE /wd4100) #warning C4100: unreferenced formal parameter in function
    target_compile_options(${target_name} PRIVATE /wd4201) #warning C4201: nonstandard extension used: nameless struct/union
    target_compile_options(${target_name} PRIVATE /wd4127) #warning C4127: conditional expression is constant
  else()
    target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic -Werror)
  endif()
endfunction()

function(my_set_target_unity_build_mode target_name)
  set_target_properties(${target_name} PROPERTIES
                          UNITY_BUILD ON
                          UNITY_BUILD_MODE BATCH
                          UNITY_BUILD_BATCH_SIZE 16
                          )	
endfunction()

function(my_add_library target_name src_path)
  file(GLOB_RECURSE src_files
      "${src_path}/src/*.*"
  )
  my_source_group(${src_path} "${src_files}")
  
  add_library(${target_name} ${src_files})	
  target_precompile_headers(${target_name} PUBLIC src/${target_name}-pch.h)
  target_include_directories(${target_name} PUBLIC src)
  my_set_target_warning_level(${target_name})
  my_set_target_unity_build_mode(${target_name})
endfunction()

function(my_add_executable target_name src_path)
  file(GLOB_RECURSE src_files
      "${src_path}/src/*.*"
  )	
  my_source_group(${src_path} "${src_files}")
  
  add_executable(${target_name} ${src_files})	
  target_precompile_headers(${target_name} PUBLIC src/${target_name}-pch.h)
  target_include_directories(${target_name} PUBLIC src)
  my_set_target_warning_level(${target_name})	
  my_set_target_unity_build_mode(${target_name})
endfunction()

function(my_add_git_submodule dir)
# add a Git submodule directory to CMake, assuming the
# Git submodule directory is a CMake project.
#
# Usage: in CMakeLists.txt
# 
# include(AddGitSubmodule.cmake)
# add_git_submodule(mysubmod_dir)

find_package(Git REQUIRED)

if(NOT EXISTS ${dir}/CMakeLists.txt)
  execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive -- ${dir}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND_ERROR_IS_FATAL ANY)
  
endif()

add_subdirectory(${dir})

endfunction(my_add_git_submodule)

function(my_link_sub_module target_name sub_module_project_name sub_module_namespace sub_module_section proj_root)

set(sub_module_section_dir external/${sub_module_project_name}/src/${sub_module_section})
set(sub_module_section_path ${proj_root}/${sub_module_section_dir})

target_include_directories(${target_name} PUBLIC ${sub_module_section_path}/src)
target_link_directories(${target_name} PUBLIC ${sub_module_section_path}/src)
target_link_libraries(${target_name} PUBLIC ${sub_module_namespace}_${sub_module_section})

endfunction()
