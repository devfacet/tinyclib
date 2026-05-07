#include "tl_config.h"
#include "unity.h"

void setUp(void) {
    // Setup code if needed
}

void tearDown(void) {
    // Teardown code if needed
}

static void test_tl_config_get_debug_level_returns_level(void) {
    tl_config_set_debug_level(3);
    TEST_ASSERT_EQUAL(3, tl_config_get_debug_level());
}

static void test_tl_config_set_debug_level_sets_level(void) {
    tl_config_set_debug_level(5);
    TEST_ASSERT_EQUAL(5, tl_config_get_debug_level());
}

static void test_tl_config_has_neon_matches_macro(void) {
#if TL_HAS_NEON
    TEST_ASSERT_TRUE(tl_config_has_neon());
#else
    TEST_ASSERT_FALSE(tl_config_has_neon());
#endif
}

static void test_tl_config_has_cmsis_dsp_matches_macro(void) {
#if TL_HAS_CMSIS_DSP
    TEST_ASSERT_TRUE(tl_config_has_cmsis_dsp());
#else
    TEST_ASSERT_FALSE(tl_config_has_cmsis_dsp());
#endif
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_tl_config_get_debug_level_returns_level);
    RUN_TEST(test_tl_config_set_debug_level_sets_level);
    RUN_TEST(test_tl_config_has_neon_matches_macro);
    RUN_TEST(test_tl_config_has_cmsis_dsp_matches_macro);

    return UNITY_END();
}
