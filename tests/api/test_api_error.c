#include "tl_error.h"
#include "unity.h"
#include <stdlib.h>
#include <string.h>

void setUp(void) {
    // Setup code if needed
}

void tearDown(void) {
    // Teardown code if needed
}

static void test_tl_error_set_message_fixed_message_sets_error(void) {
    TlError error  = {0};
    int     result = tl_error_set_message(&error, TL_ERROR_INTERNAL, "fixed error message");

    TEST_ASSERT_EQUAL_INT(TL_ERROR_NONE, result);
    TEST_ASSERT_EQUAL(TL_ERROR_INTERNAL, error.code);
    TEST_ASSERT_NOT_NULL(error.message);
    TEST_ASSERT_EQUAL_STRING("fixed error message", error.message);

    free((void *)error.message);
}

static void test_tl_error_set_message_formatted_message_sets_error(void) {
    TlError error  = {0};
    int     result = tl_error_set_message(&error, TL_ERROR_NOT_FOUND, "formatted error: %d", 42);

    TEST_ASSERT_EQUAL_INT(TL_ERROR_NONE, result);
    TEST_ASSERT_EQUAL(TL_ERROR_NOT_FOUND, error.code);
    TEST_ASSERT_NOT_NULL(error.message);
    TEST_ASSERT_EQUAL_STRING("formatted error: 42", error.message);

    free((void *)error.message);
}

static void test_tl_error_set_message_null_error_returns_invalid_argument(void) {
    TEST_ASSERT_EQUAL_INT(TL_ERROR_INVALID_ARGUMENT,
                          tl_error_set_message(NULL, TL_ERROR_NOT_READY, "this should not crash"));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_tl_error_set_message_fixed_message_sets_error);
    RUN_TEST(test_tl_error_set_message_formatted_message_sets_error);
    RUN_TEST(test_tl_error_set_message_null_error_returns_invalid_argument);

    return UNITY_END();
}
