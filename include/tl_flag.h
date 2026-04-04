// See LICENSE.txt and CONTRIBUTING.md for details.

#ifndef TL_FLAG_H
#define TL_FLAG_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Represents a parsed flag.
 *
 * `name` points into argv (or the tokenizer buffer) at the first '-' of
 * the flag. `name_len` is the length up to '\0' or '='. For the '=' form
 * `name` is NOT a NUL-terminated C string at name_len — name[name_len] is
 * '=', so comparisons must use memcmp with name_len, never strcmp.
 *
 *   argv entry: "--foo=bar"
 *                -  -  f  o  o  =  b  a  r  \0
 *                ^              ^
 *                name           value
 *                name_len = 5
 *                value    = "bar"
 *
 *   argv entry: "--foo" "bar"
 *                -  -  f  o  o  \0    b  a  r  \0
 *                ^                    ^
 *                name                 value
 *                name_len = 5
 *                value    = "bar"
 *
 *   argv entry: "--foo" (boolean, no value)
 *                -  -  f  o  o  \0
 *                ^
 *                name
 *                name_len = 5
 *                value    = NULL
 */
typedef struct {
    const char *name;     // points at the first '-' of the flag in argv
    size_t      name_len; // length of the flag name up to '\0' or '='
    const char *value;    // value after first '=', or NULL if none
} tl_flag_t;

/**
 * @brief Parses the given command line arguments.
 *
 * Parses argv into flags. A flag is anything starting with "--". It can carry
 * a value written as --name=value, or as --name value in the next entry.
 * A bare "--" ends flag parsing; everything after it is a positional, even if
 * it starts with dashes. Any previously parsed state is thrown away first.
 *
 * @param argc The number of command line arguments.
 * @param argv The command line arguments.
 *
 * @return void
 */
void tl_parse_args(int argc, char *argv[]);

/**
 * @brief Parses a raw command line string.
 *
 * Splits the line into tokens. Double quotes group text with spaces into
 * one token, and a backslash keeps the next character as-is. The first
 * token is the program name and is skipped, like argv[0].
 *
 * @param line The command line string to parse.
 *
 * @return true on success, false otherwise.
 */
bool tl_parse_line(const char *line);

/**
 * @brief Releases memory held by the argument parser.
 *
 * Safe to call when nothing has been parsed. Called implicitly by
 * tl_parse_args and tl_parse_line.
 *
 * @return void
 */
void tl_free_args(void);

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
 * Returns the value of the first occurrence of flag. For repeated flags
 * use tl_count_flag and tl_get_flag_at.
 *
 * @param flag The flag to get.
 *
 * @return The value of the flag, or NULL if not found or no value.
 */
const char *tl_get_flag(const char *flag);

/**
 * @brief Returns the number of times a flag was given.
 *
 * @param flag The flag to count.
 *
 * @return The occurrence count (0 if not given).
 */
size_t tl_count_flag(const char *flag);

/**
 * @brief Returns the value of a repeated flag at a given index.
 *
 * Occurrences are indexed in the order they appeared on the command line.
 *
 * @param flag The flag to get.
 * @param index The occurrence index (0-based).
 *
 * @return The value, or NULL if out of range or no value at that index.
 */
const char *tl_get_flag_at(const char *flag, size_t index);

/**
 * @brief Returns the number of positional arguments.
 *
 * Positionals are bare arguments (not starting with `--`) and everything
 * after a bare `--` terminator, in the order they appeared.
 *
 * @return The positional argument count.
 */
size_t tl_count_positional(void);

/**
 * @brief Returns the positional argument at the given index.
 *
 * @param index The positional index (0-based).
 *
 * @return The positional value, or NULL if out of range.
 */
const char *tl_get_positional(size_t index);

#endif // TL_FLAG_H
