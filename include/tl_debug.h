// See LICENSE.txt and CONTRIBUTING.md for details.

#ifndef TL_DEBUG_H
#define TL_DEBUG_H

#include "tl_config.h"
#include <stdio.h>

/**
 * @brief Prints the given message to stderr if the given debug level is greater or equal to the current debug level.
 *
 * @param level The debug level.
 * @param fmt The format string.
 * @param ... The arguments.
 * 
 * @return void
 */
#define TL_DEBUG_PRINT(level, fmt, ...)          \
    do {                                         \
        if (tl_get_debug_level() >= level)       \
            fprintf(stderr, fmt, ##__VA_ARGS__); \
    } while (0)

#endif // TL_DEBUG_H
