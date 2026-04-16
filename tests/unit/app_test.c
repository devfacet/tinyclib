#include "tl_app.h"
#include "tl_flag.h"
#include "unity.h"

void setUp(void) {
    // Setup code if needed
}

void tearDown(void) {
    tl_free_args();
}

static void test_tl_init_app(void) {
    char *argv[] = {"program", "--debug-level=2"};
    tl_init_app(2, argv);
    TEST_ASSERT_TRUE(tl_lookup_flag("--debug-level"));
    TEST_ASSERT_EQUAL_STRING("2", tl_get_flag("--debug-level"));
}

static void test_tl_init_app_space(void) {
    char *argv[] = {"program", "--debug-level", "3"};
    tl_init_app(3, argv);
    TEST_ASSERT_TRUE(tl_lookup_flag("--debug-level"));
    TEST_ASSERT_EQUAL_STRING("3", tl_get_flag("--debug-level"));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_tl_init_app);
    RUN_TEST(test_tl_init_app_space);

    return UNITY_END();
}
