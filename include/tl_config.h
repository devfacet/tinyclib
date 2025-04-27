// See LICENSE.txt and CONTRIBUTING.md for details.

#ifndef TL_CONFIG_H
#define TL_CONFIG_H

#include "tl_error.h"
#include <stdbool.h>

/**
 * @brief Defines whether ARM NEON is available.
 *
 * @note Available for certain ARM architectures.
 */
#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#define TL_NEON_AVAILABLE 1
#else
#define TL_NEON_AVAILABLE 0
#endif

/**
 * @brief Defines whether ARM CMSIS-DSP is available.
 *
 * @note Available for all ARM architectures.
 */
#if defined(__ARM_ARCH)
#define TL_CMSIS_DSP_AVAILABLE 1
#else
#define TL_CMSIS_DSP_AVAILABLE 0
#endif

/**
 * @brief Returns the debug level.
 *
 * @return The debug level.
 */
int tl_get_debug_level();

/**
 * @brief Sets the debug level.
 *
 * @param level The debug level.
 *
 * @return True or false.
 */
bool tl_set_debug_level(int level);

/**
 * @brief Returns whether ARM NEON is available.
 *
 * @return True or false.
 */
bool tl_neon_available();

/**
 * @brief Returns whether ARM CMSIS-DSP is available.
 *
 * @return True or false.
 */
bool tl_cmsis_dsp_available();

#endif // TL_CONFIG_H
