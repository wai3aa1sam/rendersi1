set(sub_project_name ${project_namespace}_render_api_layer
)
message("==== ${sub_project_name} ====")
project(${sub_project_name})

include(${project_root}/cmake_utility.cmake)

my_add_library(${sub_project_name} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${sub_project_name} PROPERTIES FOLDER ${project_namespace}/render)
target_link_libraries(${sub_project_name} 
    PUBLIC  ${project_namespace}_core
    )

target_compile_definitions(${sub_project_name}  
    PUBLIC  SPIRV_CROSS_STATIC=1
    )

target_include_directories  (${sub_project_name} PUBLIC ${Vulkan_includeDir})
target_link_libraries       (${sub_project_name} PUBLIC ${Vulkan_libraryDir})
target_link_libraries       (${sub_project_name} PUBLIC Vulkan::Headers GPUOpen::VulkanMemoryAllocator
                                                            Microsoft::DirectX-Guids Microsoft::DirectX-Headers)

#function(my_link_sub_module target_name sub_module_project_name sub_module_namespace sub_module_section proj_root)
#my_link_sub_module(${sub_project_name} testlib testlib core ${project_root})