find_package(GTest)
if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.16.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    # (https://github.com/google/googletest/blob/main/googletest/README.md)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

file(GLOB TEST_SOURCES "*.cpp")

add_executable(eccpp_tests ${TEST_SOURCES})
target_link_libraries(eccpp_tests PRIVATE eccpp gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(eccpp_tests)

# something weird is happening on Windows: it tries to run eccpp_tests_NOT_BUILT.exe
if(NOT WIN32)
    add_custom_command(TARGET eccpp_tests
                       POST_BUILD
                       COMMAND ctest -C $<CONFIGURATION> --output-on-failure)
endif()
