cmake_minimum_required(VERSION 3.25)

project(tinyclib LANGUAGES C)

# Set the output directories for libraries and executables
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set the C standard to C11 and fallback to C99 if not supported
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CheckCCompilerFlag)
check_c_compiler_flag("-std=c11" COMPILER_SUPPORTS_C11)
if(NOT COMPILER_SUPPORTS_C11)
    message(WARNING "C11 is not supported by the compiler. Falling back to C99.")
    set(CMAKE_C_STANDARD 99)
endif()

# Add include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/tl_app.c
    src/tl_config.c
    src/tl_error.c
    src/tl_test.c
)

# Header files
set(HEADERS
    include/tl_app.h
    include/tl_config.h
    include/tl_constants.h
    include/tl_debug.h
    include/tl_error.h
    include/tl_test.h
)

# Create a static library
add_library(tinyclib STATIC ${SOURCES} ${HEADERS})

# Fetch dependencies
include(FetchContent)

# Fetch Unity test framework
FetchContent_Declare(
    Unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG v2.6.1
)
FetchContent_GetProperties(Unity)
if(NOT Unity_POPULATED)
    FetchContent_MakeAvailable(Unity)
    set(Unity_SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/unity-src)
    if(NOT TARGET Unity)
        add_library(Unity STATIC ${Unity_SOURCE_DIR}/src/unity.c)
        target_include_directories(Unity PUBLIC ${Unity_SOURCE_DIR}/src)
    endif()
endif()

# Build tests
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()

    include_directories(${unity_SOURCE_DIR}/src)

    # Add test executable for tl_app
    add_executable(tl_app_test tests/arch/generic/tl_app_test.c)
    target_link_libraries(tl_app_test Unity tinyclib)
    add_test(NAME tl_app_test COMMAND tl_app_test)

    # Add test executable for tl_config
    add_executable(tl_config_test tests/arch/generic/tl_config_test.c)
    target_link_libraries(tl_config_test Unity tinyclib)
    add_test(NAME tl_config_test COMMAND tl_config_test)

    # Add test executable for tl_debug
    add_executable(tl_debug_test tests/arch/generic/tl_debug_test.c)
    target_link_libraries(tl_debug_test Unity tinyclib)
    add_test(NAME tl_debug_test COMMAND tl_debug_test)

    # Add test executable for tl_error
    add_executable(tl_error_test tests/arch/generic/tl_error_test.c)
    target_link_libraries(tl_error_test Unity tinyclib)
    add_test(NAME tl_error_test COMMAND tl_error_test)

    # Add test executable for tl_test
    add_executable(tl_test_test tests/arch/generic/tl_test_test.c)
    target_link_libraries(tl_test_test Unity tinyclib)
    add_test(NAME tl_test_test COMMAND tl_test_test)
endif()
