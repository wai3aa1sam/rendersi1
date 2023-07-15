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

# init vulkan
if(NOT DEFINED ENV{VK_SDK_PATH})
    message(FATAL_ERROR "VK_SDK_PATH (env var) is not defined! Try to install VulkanSDK in online")
endif()
set(VK_SDK_PATH 			$ENV{VK_SDK_PATH})
set(Vulkan_INCLUDE_DIRS 	${VK_SDK_PATH}/Include)
set(Vulkan_LIBRARY 			${VK_SDK_PATH}/Lib/vulkan-1.lib)

target_include_directories  (${sub_project_name} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries       (${sub_project_name} PUBLIC ${Vulkan_LIBRARY}
                                                        unofficial::vulkan-memory-allocator::vulkan-memory-allocator)

#function(my_link_sub_module target_name sub_module_project_name sub_module_namespace sub_module_section proj_root)
#my_link_sub_module(${sub_project_name} testlib testlib core ${project_root})