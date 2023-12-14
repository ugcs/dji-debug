cmake_minimum_required (VERSION 3.10)
project(camera CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)

set(COMMON_LIBS_DIR "${CMAKE_SOURCE_DIR}/../../libs/")

#Align build type parameter
if (NOT CMAKE_BUILD_TYPE)
    message(FATAL_ERROR "CMAKE_BUILD_TYPE should be specified")
endif()
string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)

message("Build ${CMAKE_BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/bin")

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB SORCES "src/*.cpp" src/hal/*.c)

if ("${CMAKE_BUILD_TYPE}" MATCHES "debug")
    add_definitions(-DDEBUG)
    set_source_files_properties(${SORCES} PROPERTIES COMPILE_FLAGS "-g")
endif()

#Define version string for application:
if (PACKAGE_VERSION)
    add_definitions(-DPACKAGE_VERSION=${PACKAGE_VERSION})
else()
    add_definitions(-DPACKAGE_VERSION="0-dev")
endif()

#Add DJI PSDK:
include_directories(${COMMON_LIBS_DIR}/dji-psdk-3.6/include)
link_directories(${COMMON_LIBS_DIR}/dji-psdk-3.6/lib/x86_64-linux-gnu-gcc)


add_executable(${CMAKE_PROJECT_NAME} ${SORCES})

target_link_libraries(${CMAKE_PROJECT_NAME} pthread payloadsdk)

