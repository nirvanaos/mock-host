cmake_minimum_required (VERSION 3.25)
set (CMAKE_CXX_SCAN_FOR_MODULES 0)
set (CMAKE_CONFIGURATION_TYPES Debug Release)

project (mockhost LANGUAGES CXX VERSION 3.0.1)
include (CTest)

add_library (mockhost SHARED
	src/constants.cpp
	src/HostAPI.cpp
	src/Mutex.cpp
	src/Thread.cpp
)

target_compile_definitions (mockhost PRIVATE MOCKHOST_EXPORTS)

target_include_directories (mockhost PRIVATE
	${CMAKE_SOURCE_DIR}/library/Include/Nirvana
)

if (WIN32)
target_link_libraries (mockhost PRIVATE mincore)
endif ()

if (BUILD_TESTING)
add_subdirectory (unittests)
endif ()

install(TARGETS mockhost EXPORT ${PROJECT_NAME}_targets)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION include/${PROJECT_NAME}
    FILES_MATCHING PATTERN "*.h"
)

target_include_directories (${PROJECT_NAME} PUBLIC
	$<INSTALL_INTERFACE:include>
)

# Generate and install *-targets.cmake 
install(EXPORT ${PROJECT_NAME}_targets
    FILE ${PROJECT_NAME}-targets.cmake
    DESTINATION share/${PROJECT_NAME}
)

# Generate the config file in the current binary dir (this ensures it's not placed directly in source)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    "include(\"\${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}-targets.cmake\")\n"
)

# Install the generated config file
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    DESTINATION share/${PROJECT_NAME}
)
