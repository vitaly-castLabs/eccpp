cmake_minimum_required(VERSION 3.14)
project(eccpp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)

add_library(eccpp INTERFACE)
target_include_directories(eccpp INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

if(BUILD_TESTS)
    include(CTest)

    enable_testing()
    add_subdirectory(tests)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

install(FILES
    gn.h
    kron.h
    mdarray.h
    minstar.h
    DESTINATION include/eccpp
)

install(TARGETS eccpp
    EXPORT eccpp-targets
    INCLUDES DESTINATION include
)

install(EXPORT eccpp-targets
    FILE eccpp-targets.cmake
    NAMESPACE eccpp::
    DESTINATION lib/cmake/eccpp
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/eccpp-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_file(cmake/eccpp-config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/eccpp-config.cmake"
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/eccpp-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/eccpp-config-version.cmake"
    DESTINATION lib/cmake/eccpp
)
