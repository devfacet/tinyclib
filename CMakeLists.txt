cmake_minimum_required(VERSION 3.25)

project(tinyclib VERSION 0.4.0 LANGUAGES C)

# Settings and options
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
option(TL_BUILD_TESTS "Build tinyclib tests" ${PROJECT_IS_TOP_LEVEL})

function(tl_configure_c_target target)
  set_property(TARGET ${target} PROPERTY C_STANDARD 11)
  set_property(TARGET ${target} PROPERTY C_STANDARD_REQUIRED ON)
  set_property(TARGET ${target} PROPERTY C_EXTENSIONS OFF)
endfunction()

# Dependencies
include(CTest)
include(FetchContent)

# Sources
file(GLOB SOURCES CONFIGURE_DEPENDS src/*.c)
# Add the library (BUILD_SHARED_LIBS is handled by CMake)
add_library(tinyclib ${SOURCES})
tl_configure_c_target(tinyclib)

# Set include directories for build and install
target_include_directories(
  tinyclib
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
set_target_properties(tinyclib PROPERTIES POSITION_INDEPENDENT_CODE ON) # support shared libraries
add_library(tinyclib::tinyclib ALIAS tinyclib) # add a namespace alias for modern linking

# Tests
if(TL_BUILD_TESTS)
  add_subdirectory(third_party/unity)
  add_subdirectory(tests)
endif()

# Install targets and headers
include(GNUInstallDirs)

install(
  TARGETS tinyclib
  EXPORT tinyclibTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Package configuration
include(CMakePackageConfigHelpers)

configure_package_config_file(
  cmake/tinyclibConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/tinyclibConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tinyclib
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/tinyclibConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/tinyclibConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/tinyclibConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tinyclib
)

install(
  EXPORT tinyclibTargets
  FILE tinyclibTargets.cmake
  NAMESPACE tinyclib::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tinyclib
)
