#include "tl_app.h"
#include "unity.h"

void setUp(void) {
    // Setup code if needed
}

void tearDown(void) {
    // Teardown code if needed
}

void test_tl_init_app(void) {
    char *argv[] = {"program", "--debug-level=2"};
    tl_init_app(2, argv);
    TEST_ASSERT_TRUE(tl_lookup_flag("--debug-level"));
    TEST_ASSERT_EQUAL_STRING("2", tl_get_flag("--debug-level"));
}

void test_tl_parse_args(void) {
    char *argv[] = {"program", "--test-flag"};
    tl_parse_args(2, argv);
    TEST_ASSERT_TRUE(tl_lookup_flag("--test-flag"));
}

void test_tl_lookup_flag(void) {
    char *argv[] = {"program", "--test-flag"};
    tl_init_app(2, argv);
    TEST_ASSERT_TRUE(tl_lookup_flag("--test-flag"));
    TEST_ASSERT_FALSE(tl_lookup_flag("--nonexistent-flag"));
}

void test_tl_get_flag(void) {
    char *argv[] = {"program", "--key=value"};
    tl_init_app(2, argv);
    TEST_ASSERT_EQUAL_STRING("value", tl_get_flag("--key"));
    TEST_ASSERT_NULL(tl_get_flag("--nonexistent-key"));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_tl_init_app);
    RUN_TEST(test_tl_parse_args);
    RUN_TEST(test_tl_lookup_flag);
    RUN_TEST(test_tl_get_flag);

    return UNITY_END();
}
