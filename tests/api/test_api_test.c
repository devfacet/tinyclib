#include "tl_test.h"
#include "unity.h"
#include <time.h>

void setUp(void) {
    // Setup code if needed
}

void tearDown(void) {
    // Teardown code if needed
}

static void test_tl_test_get_timespec_diff_ns_returns_nanoseconds(void) {
    struct timespec start = {.tv_sec = 1, .tv_nsec = 500000000};
    struct timespec end   = {.tv_sec = 2, .tv_nsec = 200000000};

    long long diff_ns = tl_test_get_timespec_diff_ns(&start, &end);

    TEST_ASSERT_EQUAL(700000000, diff_ns);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_tl_test_get_timespec_diff_ns_returns_nanoseconds);

    return UNITY_END();
}
