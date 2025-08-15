cmake_minimum_required (VERSION 3.25)
set (CMAKE_CXX_SCAN_FOR_MODULES 0)
set (CMAKE_CONFIGURATION_TYPES Debug Release)

project (mock-host CXX)

add_library (mockhost SHARED
	src/constants.cpp
	src/HostAPI.cpp
	src/Mutex.cpp
)

target_compile_definitions (mockhost PRIVATE MOCKHOST_EXPORTS)
