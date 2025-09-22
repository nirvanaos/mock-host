find_package (GTest CONFIG REQUIRED)

add_executable (unittests test.cpp)
target_link_libraries (unittests PRIVATE
  mockhost
  GTest::gtest_main)

if (MSVC)
  target_compile_options (unittests PRIVATE
    "/utf-8"
    "/Zc:preprocessor"
  )
  target_compile_definitions (unittests PRIVATE
    _CRT_SECURE_NO_WARNINGS
  )
endif()

if (WIN32)
add_custom_command (TARGET unittests POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:unittests> $<TARGET_FILE_DIR:unittests>
  COMMAND_EXPAND_LISTS
)
endif()

include (GoogleTest)
gtest_discover_tests (unittests WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
