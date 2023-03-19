set(sub_project_name ${project_namespace}_core_test)
message("==== ${sub_project_name} ====")
project(${sub_project_name})

include(${project_root}/cmake_utility.cmake)

my_add_executable(${sub_project_name} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${sub_project_name} PROPERTIES FOLDER ${project_namespace}/test)
target_link_libraries(${sub_project_name} 
    PUBLIC  ${project_namespace}_test_base
    )


target_compile_definitions(${sub_project_name} 
    PRIVATE RDS_TEST_ALL_MODULE=0
    )
