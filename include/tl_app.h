// See LICENSE.txt and CONTRIBUTING.md for details.

#ifndef TL_APP_H
#define TL_APP_H

#include <stdbool.h>

/**
 * @brief Initializes the running app with the given command line arguments.
 *
 * @param argc The number of command line arguments.
 * @param argv The command line arguments.
 *
 * @return void
 */
void tl_init_app(int argc, char *argv[]);

/**
 * @brief Parses the given command line arguments.
 *
 * @param argc The number of command line arguments.
 * @param argv The command line arguments.
 *
 * @return void
 */
void tl_parse_args(int argc, char *argv[]);

/**
 * @brief Looks up a specific flag.
 *
 * @param flag The flag to look up.
 *
 * @return true if the flag is found, false otherwise.
 */
bool tl_lookup_flag(const char *flag);

/**
 * @brief Returns the value of a specific flag.
 *
 * @param flag The flag to get.
 *
 * @return The value of the flag, or NULL if not found.
 */
const char *tl_get_flag(const char *flag);

#endif // TL_APP_H
