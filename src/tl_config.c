// See LICENSE.txt and CONTRIBUTING.md for details.

#include "tl_config.h"
#include <stdbool.h>

/**
 * @brief debug_level holds the debug level.
 */
static int debug_level = 0;

int tl_config_get_debug_level() {
    return debug_level;
}

void tl_config_set_debug_level(int level) {
    debug_level = level;
}

bool tl_config_has_neon() {
#if TL_HAS_NEON
    return true;
#else
    return false;
#endif
}

bool tl_config_has_cmsis_dsp() {
#if TL_HAS_CMSIS_DSP
    return true;
#else
    return false;
#endif
}
