cmake_minimum_required (VERSION 2.8)
include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src
                     ${CMAKE_SOURCE_DIR}/test
                     ${TASKD_INCLUDE_DIRS})

set (test_SRCS msg.t)

add_custom_target (test ./run_all --verbose
                        DEPENDS ${test_SRCS}
                        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test)

add_custom_target (build_tests DEPENDS ${test_SRCS}
                               WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test)

foreach (src_FILE ${test_SRCS})
  add_executable (${src_FILE} "${src_FILE}.cpp"
                              ../src/Msg.cpp
                              ../src/text.cpp
                              test.cpp)
  target_link_libraries (${src_FILE} ${TASKD_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
endforeach (src_FILE)

