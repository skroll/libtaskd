cmake_minimum_required (VERSION 2.8)
include_directories (${CMAKE_SOURCE_DIR}/src)
set (taskd_SRCS taskd.cpp)
add_library (taskd STATIC ${taskd_SRCS})
set (CMAKE_INSTALL_LIBDIR lib CACHE PATH "Output directory for libraries")
install (TARGETS taskd DESTINATION ${CMAKE_INSTALL_LIBDIR})
install (FILES taskd.h DESTINATION include)

set (CMAKE_BUILD_TYPE debug)
set (CMAKE_C_FLAGS_DEBUG "-ggdb3")
set (CMAKE_C_FLAGS_RELEASE "-O3")

