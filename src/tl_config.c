// See LICENSE.txt and CONTRIBUTING.md for details.

#include "tl_config.h"
#include <stdbool.h>

/**
 * @brief debug_level holds the debug level.
 */
static int debug_level = 0;

int tl_get_debug_level() {
    return debug_level;
}

bool tl_set_debug_level(int level) {
    debug_level = level;
    return true;
}

bool tl_neon_available() {
#if TL_NEON_AVAILABLE
    return true;
#else
    return false;
#endif
}

bool tl_cmsis_dsp_available() {
#if TL_CMSIS_DSP_AVAILABLE
    return true;
#else
    return false;
#endif
}
