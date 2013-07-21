cmake_minimum_required (VERSION 2.8)
include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src
                     ${TASKD_INCLUDE_DIRS})
set (taskd_SRCS api.cpp       taskd.h
                Msg.cpp       Msg.h
                TLSClient.cpp TLSClient.h)

add_library (taskd STATIC ${taskd_SRCS})
set (CMAKE_INSTALL_LIBDIR lib CACHE PATH "Output directory for libraries")
install (TARGETS taskd DESTINATION ${CMAKE_INSTALL_LIBDIR})
install (FILES taskd.h DESTINATION include)

set (CMAKE_BUILD_TYPE debug)
set (CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -g -Wall")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wall")

