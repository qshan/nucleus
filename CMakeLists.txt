cmake_minimum_required(VERSION 3.3)
project(nucleus)

set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif (NOT CMAKE_BUILD_TYPE)

if(MSVC)
    add_definitions("/W3" "/D _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS" "/D _CRT_SECURE_NO_WARNINGS" "/D _WIN32_WINNT=0x0A00")
    add_compile_options( "$<$<CONFIG:RELEASE>:/O2>")
else()
    add_definitions("-Wall" ) # "-Wextra" "-Werror"
    add_compile_options("$<$<CONFIG:RELEASE>:-O3>")
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options("-stdlib=libc++")
    else()
        # nothing yet
    endif()
endif()

include(FindThreads)

find_package(PkgConfig QUIET)

find_path(PMDK_INCLUDE_DIRS libpmemobj.h)
find_library(PMDK_LIBPMEMOBJ_LIBRARY NAMES libpmemobj libpmemobj.so HINTS /usr/local/lib64/)

message (STATUS "Found PMDK include headers in ${PMDK_INCLUDE_DIRS}" )
message (STATUS "Found PMDK LIBPMEMOBJ library in ${PMDK_LIBPMEMOBJ_LIBRARY}" )

if(NOT PMDK_INCLUDE_DIRS OR NOT PMDK_LIBPMEMOBJ_LIBRARY)
    message(FATAL_ERROR "Unable to find PMDK LIBMEMOBJ - check messages above.")
endif()

# include_directories(D:/app/vcpkg/installed/x64-windows/include )
# link_directories(D:/app/vcpkg/packages/pmdk_x64-windows/lib)

file(GLOB APP_CORE ./app/*.cpp)
file(GLOB SOURCES_CORE ./src/*.cpp)
file(GLOB SOURCES_LIB ./src/lib/*.cpp)
file(GLOB SOURCES_SPDLOG ./src/lib/spdlog/*.cpp)
file(GLOB SOURCES_HTTP_PARSER ./src/lib/http_parser/http_parser.c)

add_executable(nucleus ${APP_CORE} ${SOURCES_CORE} ${SOURCES_LIB} ${SOURCES_SPDLOG} ${SOURCES_HTTP_PARSER})

target_include_directories(nucleus PUBLIC ${PMDK_INCLUDE_DIRS} ./app ./src ./src/lib ./src/lib/http_parser)
target_link_libraries(nucleus ${PMDK_LIBPMEMOBJ_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} )

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
