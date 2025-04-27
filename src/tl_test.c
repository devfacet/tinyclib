// See LICENSE.txt and CONTRIBUTING.md for details.

#define _POSIX_C_SOURCE 199309L

#include "tl_test.h"
#include <time.h>

// TODO: Add tests

long long tl_timespec_diff_ns(struct timespec *start, struct timespec *end) {
    return (end->tv_sec - start->tv_sec) * 1000000000LL + (end->tv_nsec - start->tv_nsec);
}
