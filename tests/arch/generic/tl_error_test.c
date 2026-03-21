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

static void test_tl_error_set_with_fixed_message(void) {
    TLError error = {0};
    tl_error_set(&error, TL_ERROR_INTERNAL, "fixed error message");

    TEST_ASSERT_EQUAL(TL_ERROR_INTERNAL, error.code);
    TEST_ASSERT_NOT_NULL(error.message);
    TEST_ASSERT_EQUAL_STRING("fixed error message", error.message);

    free((void *)error.message);
}

static void test_tl_error_set_with_formatted_message(void) {
    TLError error = {0};
    tl_error_set(&error, TL_ERROR_NOT_FOUND, "formatted error: %d", 42);

    TEST_ASSERT_EQUAL(TL_ERROR_NOT_FOUND, error.code);
    TEST_ASSERT_NOT_NULL(error.message);
    TEST_ASSERT_EQUAL_STRING("formatted error: 42", error.message);

    free((void *)error.message);
}

static void test_tl_error_set_with_null_error(void) {
    tl_error_set(NULL, TL_ERROR_NOT_READY, "this should not crash");
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_tl_error_set_with_fixed_message);
    RUN_TEST(test_tl_error_set_with_formatted_message);
    RUN_TEST(test_tl_error_set_with_null_error);

    return UNITY_END();
}
