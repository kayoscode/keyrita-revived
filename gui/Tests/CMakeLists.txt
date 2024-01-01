include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(ctrl_tree_tests ControlTreeTests.cpp ${HEADER_FILES})
target_link_libraries(ctrl_tree_tests gtest_main wgui)
add_test(ctrl_tree_gtests ctrl_tree_tests test_trees ctrl_tree_tests)
