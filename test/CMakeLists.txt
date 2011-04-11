include_directories (${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/test)
add_custom_target (test ./run_all DEPENDS #thing.t
                                  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test)
#add_executable (thing.t thing.t.cpp test.cpp)
#target_link_libraries (thing.t libtaskd)
