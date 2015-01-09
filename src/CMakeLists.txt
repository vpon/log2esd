cmake_minimum_required (VERSION 2.8.0)

project (log2esd)

# The version number
set (LOG2ESD_VERSION_MAJOR 0)
set (LOG2ESD_VERSION_MINOR 1)

# Set compile flags
set (CMAKE_C_FLAGS "-std=gnu11 -m64 -Wall -Wextra -pedantic -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64")
set (CMAKE_C_FLAGS_DEBUG "-g")
set (CMAKE_C_FLAGS_RELEASE "-O2")
set (CMAKE_CXX_FLAGS "-std=c++11 -m64 -Wall -Wno-long-long -Wextra -pedantic -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64")
set (CMAKE_CXX_FLAGS_DEBUG "-g")
set (CMAKE_CXX_FLAGS_RELEASE "-O2")

# Set external dependencies
set (PROJECT_INCLUDE_DIRS ./)
set (CURL_INCLUDE_DIRS ./external/curl-7.39.0/include)

set (CURL_LIBRARY_DIRS ${CMAKE_SOURCE_DIR}/ext_lib)

# configure a header file to pass some of CMake settings
# to the source code

# configure_file (
#  "${PROJECT_SOURCE_DIR}/src/config.h.in"
#  "${PROJECT_SOURCE_DIR}/config.h"
# )

include_directories(${PROJECT_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIRS})

add_subdirectory(log)
add_subdirectory(public)
add_subdirectory(producer)
add_subdirectory(agent)

link_directories(${CURL_LIBRARY_DIRS})
add_executable(log2esd log2esd.cc)
target_link_libraries (log2esd producer agent log pub curl)
