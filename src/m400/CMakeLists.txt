cmake_minimum_required (VERSION 3.10)
project(m400 CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)

#find_package(PkgConfig REQUIRED)
#pkg_check_modules(LIBUSB REQUIRED libusb-1.0)


set(COMMON_LIBS_DIR "${CMAKE_SOURCE_DIR}/../../libs/")

#Align build type parameter
if (NOT CMAKE_BUILD_TYPE)
    message(FATAL_ERROR "CMAKE_BUILD_TYPE should be specified")
endif()
string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)

message("Build ${CMAKE_BUILD_TYPE} $ENV{DESKTOP_ARCH}")

if ("$ENV{DESKTOP_ARCH}" MATCHES "arm64")
  link_directories(${COMMON_LIBS_DIR}/dji-psdk-3.13.1/lib/aarch64-linux-gnu-gcc)
else()

  link_directories(${COMMON_LIBS_DIR}/dji-psdk-3.13.1/lib/x86_64-linux-gnu-gcc)
endif()

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
include_directories(${COMMON_LIBS_DIR}/dji-psdk-3.13.1/include)
#link_directories(${COMMON_LIBS_DIR}/dji-psdk-3.13.1/lib/x86_64-linux-gnu-gcc)

add_executable(${CMAKE_PROJECT_NAME} ${SORCES})

target_link_libraries(${CMAKE_PROJECT_NAME} pthread payloadsdk)


if(LIBUSB_FOUND)
	message("Build with libusb support")
    add_definitions(-DLIBUSB_INSTALLED)
    target_link_libraries(${PROJECT_NAME} usb-1.0)
else()
	message("Build w/o libusb support")
endif()

