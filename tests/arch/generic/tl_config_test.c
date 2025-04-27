#include "tl_config.h"
#include "unity.h"

void setUp(void) {
    // Setup code if needed
}

void tearDown(void) {
    // Teardown code if needed
}

void test_tl_get_debug_level(void) {
    tl_set_debug_level(3);
    TEST_ASSERT_EQUAL(3, tl_get_debug_level());
}

void test_tl_set_debug_level(void) {
    TEST_ASSERT_TRUE(tl_set_debug_level(5));
    TEST_ASSERT_EQUAL(5, tl_get_debug_level());
}

void test_tl_neon_available(void) {
#if TL_NEON_AVAILABLE
    TEST_ASSERT_TRUE(tl_neon_available());
#else
    TEST_ASSERT_FALSE(tl_neon_available());
#endif
}

void test_tl_cmsis_dsp_available(void) {
#if TL_CMSIS_DSP_AVAILABLE
    TEST_ASSERT_TRUE(tl_cmsis_dsp_available());
#else
    TEST_ASSERT_FALSE(tl_cmsis_dsp_available());
#endif
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_tl_get_debug_level);
    RUN_TEST(test_tl_set_debug_level);
    RUN_TEST(test_tl_neon_available);
    RUN_TEST(test_tl_cmsis_dsp_available);

    return UNITY_END();
}
