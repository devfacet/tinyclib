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
#define TL_HAS_NEON 1
#else
#define TL_HAS_NEON 0
#endif

/**
 * @brief Defines whether ARM CMSIS-DSP is available.
 *
 * @note Available for all ARM architectures.
 */
#ifdef __ARM_ARCH
#define TL_HAS_CMSIS_DSP 1
#else
#define TL_HAS_CMSIS_DSP 0
#endif

/**
 * @brief Returns the debug level.
 *
 * @return The debug level.
 */
int tl_config_get_debug_level();

/**
 * @brief Sets the debug level.
 *
 * @param level The debug level.
 *
 * @return void
 */
void tl_config_set_debug_level(int level);

/**
 * @brief Returns whether ARM NEON is available.
 *
 * @return True or false.
 */
bool tl_config_has_neon();

/**
 * @brief Returns whether ARM CMSIS-DSP is available.
 *
 * @return True or false.
 */
bool tl_config_has_cmsis_dsp();

#endif // TL_CONFIG_H
