# main
add_executable(hello hello_main.cc hello.cc)
target_link_libraries(hello absl::strings)

# googletest
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
add_executable(Google_Tests_run hello_test.cc hello.cc)
target_link_libraries(Google_Tests_run gtest gtest_main absl::strings)