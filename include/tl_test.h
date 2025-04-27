// See LICENSE.txt and CONTRIBUTING.md for details.

#ifndef TL_TEST_H
#define TL_TEST_H

#include <time.h>

/**
 * @brief Returns the difference between two timespec in nanoseconds.
 *
 * @param start The start timespec.
 * @param end The end timespec.
 *
 * @return The difference between the two timespec structures in nanoseconds.
 */
long long tl_timespec_diff_ns(struct timespec *start, struct timespec *end);

#endif // TL_TEST_H
