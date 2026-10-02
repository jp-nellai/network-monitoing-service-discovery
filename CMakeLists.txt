cmake_minimum_required(VERSION 3.16)
project(nms_discovery VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)
find_path(NETSNMP_INCLUDE_DIR NAMES net-snmp/net-snmp-config.h)
find_library(NETSNMP_LIBRARY NAMES netsnmp)

if(NOT NETSNMP_INCLUDE_DIR OR NOT NETSNMP_LIBRARY)
    message(FATAL_ERROR "Net-SNMP development files were not found. Install libsnmp-dev/net-snmp-devel.")
endif()

add_executable(nms-discovery
    src/main.cpp
    src/device.cpp
    src/discovery.cpp
    src/network_scanner.cpp
    src/snmp_client.cpp
)

target_include_directories(nms-discovery PRIVATE ${PROJECT_SOURCE_DIR}/include ${NETSNMP_INCLUDE_DIR})
target_link_libraries(nms-discovery PRIVATE ${NETSNMP_LIBRARY} Threads::Threads)

if(MSVC)
    target_compile_options(nms-discovery PRIVATE /W4)
else()
    target_compile_options(nms-discovery PRIVATE -Wall -Wextra -Wpedantic)
endif()
