cmake_minimum_required(VERSION 3.25)

project(tinyclib VERSION 0.1.0 LANGUAGES C)

# Set the output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Sources
file(GLOB SOURCES CONFIGURE_DEPENDS src/*.c)

# Add the library (BUILD_SHARED_LIBS is handled by CMake)
add_library(tinyclib ${SOURCES})

# Set the C standard to C11 for now
set_property(TARGET tinyclib PROPERTY C_STANDARD 11)
set_property(TARGET tinyclib PROPERTY C_STANDARD_REQUIRED ON)
set_property(TARGET tinyclib PROPERTY C_EXTENSIONS OFF)

# Set include directories for build and install
target_include_directories(
  tinyclib
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

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

# Add a namespace alias for modern linking
add_library(tinyclib::tinyclib ALIAS tinyclib)

# Support shared libraries
set_target_properties(tinyclib PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Build tests
if(BUILD_TESTS)
  include(CTest)
  include(FetchContent)

  # FetchContent for Unity testing framework
  FetchContent_Declare(
      Unity
      GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
      GIT_TAG v2.6.1
  )
  FetchContent_MakeAvailable(Unity)
  FetchContent_GetProperties(Unity)

  if(NOT TARGET Unity)
      add_library(Unity STATIC ${unity_SOURCE_DIR}/src/unity.c)
  endif()
  target_include_directories(Unity PUBLIC ${unity_SOURCE_DIR}/src)

  enable_testing()
  foreach(t app config debug error test)
    add_executable(tl_${t}_test tests/arch/generic/tl_${t}_test.c)
    target_link_libraries(tl_${t}_test Unity tinyclib)
    add_test(NAME tl_${t}_test COMMAND tl_${t}_test)
  endforeach()
endif()
