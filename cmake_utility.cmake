
# references
# https://github.com/SimpleTalkCpp/SimpleGameEngine
# https://www.scivision.dev/cmake-git-submodule/

# my_print_var(variable_name)
function(my_print_var var)
  message("${var} = ${${var}}")
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
    target_compile_options(${target_name} PRIVATE /wd5072) #warning C5072: ASAN enabled without debug information emission.
  else()
    target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic -Werror -Wconversion -Wshadow)
  endif()
endfunction()

function(my_set_target_unity_build_mode target_name)
  set_target_properties(${target_name} PROPERTIES
                          UNITY_BUILD ON
                          UNITY_BUILD_MODE BATCH
                          UNITY_BUILD_BATCH_SIZE 16
                          )
endfunction()

function(my_set_multi_core_compile target_name)
  if(MSVC)
    target_compile_options(${target_name} PRIVATE /MP)     
  else()
  endif()
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
  my_set_multi_core_compile(${target_name})
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
  my_set_multi_core_compile(${target_name})

  if(${${project_namespace_marco}_ENABLE_SANITIZER})
    my_enable_sanitizer(${target_name})
  endif()
  
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
  target_link_directories(${target_name}    PUBLIC ${sub_module_section_path}/src)
  target_link_libraries(${target_name}      PUBLIC ${sub_module_namespace}_${sub_module_section})

endfunction()

function(my_enable_sanitizer target_name)

  if(MSVC)
  message("--- ${project_namespace_marco}_ENABLE_SANITIZER for MSVC")
    #set_property(TARGET ${target_name}     PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    target_compile_options(${target_name}  
      PRIVATE 
              "$<$<CONFIG:DEBUG>:/fsanitize=fuzzer>"
              "$<$<CONFIG:DEBUG>:/Zi>"
    )

              
    target_link_options(${target_name}     
      PRIVATE 
              "$<$<CONFIG:DEBUG>:/fsanitize=fuzzer>"
              "$<$<CONFIG:DEBUG>:/Zi>"
    )

    set(asan_dst_path     "$<TARGET_FILE:${target_name}>/../")
    set(asan_asan_dbg_dll "$(MsvcAnalysisToolsPath)/clang_rt.asan_dbg_dynamic-x86_64.dll")
    set(asan_asan_dll     "$(MsvcAnalysisToolsPath)/clang_rt.asan_dynamic-x86_64.dll")

    # don;t add VERBATIM, it will make \" to \" msvc instead of just "
    add_custom_command(
      TARGET ${target_name} 
      POST_BUILD
      COMMAND "$<$<CONFIG:Debug>:${CMAKE_COMMAND}>" -E copy \"${asan_asan_dbg_dll}\"  \"${asan_dst_path}\"
      COMMAND "$<$<CONFIG:Debug>:${CMAKE_COMMAND}>" -E copy \"${asan_asan_dll}\"      \"${asan_dst_path}\"
    )

    # "$<$<CONFIG:DEBUG>:/MTd>"
    #target_compile_options(${target_name}  PRIVATE "$<$<CONFIG:RELEASE>:/MD>")
  endif()
endfunction()

function(my_set_opt opt val)
  unset (${opt} CACHE)
  option(${opt} "" ${val}) # ${ARGV2}
endfunction()

function(my_only_one_opt_could_on_3 opt1 opt2 opt3)
  if( (${opt1} AND (${opt2} OR ${opt3}) )
      OR (${opt2} AND ${opt3})
    )
      message( FATAL_ERROR "only either one could be enable, 
              ${opt1}
              or ${opt2}
              or ${opt3}
              "
              )
  endif()
endfunction()
