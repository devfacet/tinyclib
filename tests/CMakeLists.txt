set(TEST_BIN_DIR "${PROJECT_BINARY_DIR}/tests/bin")

set(UNIT_TEST_TARGETS
)

set(API_TEST_TARGETS
  test_api_app
  test_api_config
  test_api_debug
  test_api_error
  test_api_flag
  test_api_test
)

foreach(test_target IN LISTS UNIT_TEST_TARGETS)
  set(test_binary "${test_target}")
  set(test_sources unit/${test_target}.c)
  add_executable(${test_binary} ${test_sources})
  target_link_libraries(${test_binary} PRIVATE tinyclib unity)
  target_include_directories(${test_binary} PRIVATE ${PROJECT_SOURCE_DIR}/src)
  tl_configure_c_target(${test_binary})
  set_target_properties(${test_binary} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN_DIR})
  add_test(NAME ${test_binary} COMMAND ${test_binary})
endforeach()

foreach(test_target IN LISTS API_TEST_TARGETS)
  set(test_binary "${test_target}")
  set(test_sources api/${test_target}.c)
  add_executable(${test_binary} ${test_sources})
  target_link_libraries(${test_binary} PRIVATE tinyclib unity)
  target_include_directories(${test_binary} PRIVATE ${PROJECT_SOURCE_DIR}/include)
  tl_configure_c_target(${test_binary})
  set_target_properties(${test_binary} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN_DIR})
  add_test(NAME ${test_binary} COMMAND ${test_binary})
endforeach()
