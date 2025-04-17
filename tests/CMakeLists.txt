find_package(GTest QUIET)
if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.16.0
    )
    FetchContent_MakeAvailable(googletest)
endif()

file(GLOB TEST_SOURCES "*.cpp")

add_executable(eccpp_tests ${TEST_SOURCES})
target_link_libraries(eccpp_tests PRIVATE eccpp gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(eccpp_tests)

add_custom_target(run_tests ALL
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS eccpp_tests
)
