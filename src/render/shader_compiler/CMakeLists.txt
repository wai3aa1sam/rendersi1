set(sub_project_name ${project_namespace}_shader_compiler)
message("==== ${sub_project_name} ====")
project(${sub_project_name})

include(${project_root}/cmake_utility.cmake)

my_add_executable(${sub_project_name} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${sub_project_name} PROPERTIES FOLDER ${project_namespace}/render)
target_link_libraries(${sub_project_name} 
    PUBLIC  ${project_namespace}_render
    )

target_include_directories  (${sub_project_name} PUBLIC ${Vulkan_includeDir})
target_link_libraries       (${sub_project_name} PUBLIC ${Vulkan_libraryDir})
target_link_libraries       (${sub_project_name} PRIVATE  
                                spirv-cross-core spirv-cross-hlsl spirv-cross-msl spirv-cross-glsl 
                                unofficial::shaderc::shaderc Microsoft::DirectXShaderCompiler)
                                
#function(my_link_sub_module target_name sub_module_project_name sub_module_namespace sub_module_section proj_root)
#my_link_sub_module(${sub_project_name} testlib testlib core ${project_root})