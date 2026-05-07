#include "tl_app.h"
#include "tl_flag.h"
#include "unity.h"

void setUp(void) {
    // Setup code if needed
}

void tearDown(void) {
    tl_flag_free_args();
}

static void test_tl_app_init_equals_debug_level(void) {
    char *argv[] = {"program", "--debug-level=2"};
    tl_app_init(2, argv);
    TEST_ASSERT_TRUE(tl_flag_has_flag("--debug-level"));
    TEST_ASSERT_EQUAL_STRING("2", tl_flag_get_value("--debug-level"));
}

static void test_tl_app_init_space_debug_level(void) {
    char *argv[] = {"program", "--debug-level", "3"};
    tl_app_init(3, argv);
    TEST_ASSERT_TRUE(tl_flag_has_flag("--debug-level"));
    TEST_ASSERT_EQUAL_STRING("3", tl_flag_get_value("--debug-level"));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_tl_app_init_equals_debug_level);
    RUN_TEST(test_tl_app_init_space_debug_level);

    return UNITY_END();
}
