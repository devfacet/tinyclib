set(TEST_BIN_DIR "${PROJECT_BINARY_DIR}/tests/bin")

set(UNIT_TEST_TARGETS
  app_test
  config_test
  debug_test
  error_test
  flag_test
  test_test
)

foreach(test_target IN LISTS UNIT_TEST_TARGETS)
  string(REGEX REPLACE "_test$" "" test_name "${test_target}")
  set(test_binary "tl_${test_name}")
  add_executable(${test_binary} unit/${test_target}.c)
  target_link_libraries(${test_binary} PRIVATE tinyclib unity)
  target_include_directories(${test_binary} PRIVATE ${PROJECT_SOURCE_DIR}/src)
  set_target_properties(${test_binary} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN_DIR})
  add_test(NAME ${test_binary} COMMAND ${test_binary})
endforeach()
