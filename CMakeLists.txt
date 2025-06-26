cmake_minimum_required(VERSION 3.14)

# on Ubuntu 24.04 clang++ (18.1.3) produces much faster executables than g++ (13.3.0)
set(CMAKE_CXX_COMPILER_NAMES clang++ g++ icpc c++ cxx)
project(eccpp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Enable LTO via CMake when supported
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

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
    hamdist.h
    kron.h
    mdarray.h
    minstar.h
    phi.h
    polar-enc.h
    repeat-enc.h
    sign.h
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
