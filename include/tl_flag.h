// See LICENSE.txt and CONTRIBUTING.md for details.

#ifndef TL_FLAG_H
#define TL_FLAG_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Represents a not-found result for index returning functions.
 */
#define TL_ARG_NOT_FOUND ((size_t)-1)

/**
 * @brief Represents a parsed flag.
 *
 * `name` points into argv (or the tokenizer buffer) at the first '-' of
 * the flag. `name_len` is the length up to '\0' or '='. For the '=' form
 * `name` is NOT a NUL-terminated C string at name_len: name[name_len] is
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
} TlFlag;

/**
 * @brief Represents options for argument parsing.
 */
typedef struct {
    const char *const *value_flags;
    const char *const *bool_flags;
} TlParseOptions;

/**
 * @brief Defines argument parser result codes.
 */
typedef enum {
    TL_PARSE_OK                       = 0,
    TL_PARSE_ERROR_INVALID_RANGE      = -1,
    TL_PARSE_ERROR_UNKNOWN_FLAG       = -2,
    TL_PARSE_ERROR_MISSING_VALUE      = -3,
    TL_PARSE_ERROR_CONFLICTING_FLAG   = -4,
    TL_PARSE_ERROR_MEMORY_ALLOCATION  = -5,
    TL_PARSE_ERROR_INVALID_INPUT      = -6,
    TL_PARSE_ERROR_UNTERMINATED_QUOTE = -7,
} TlParseResult;

/**
 * @brief Returns the first argv index matching the given name.
 *
 * Searches argv from index 0.
 *
 * @param argc The number of command line arguments.
 * @param argv The command line arguments.
 * @param name The exact argument name to find.
 *
 * @return The argv index, or TL_ARG_NOT_FOUND when not found.
 */
size_t tl_flag_get_arg_index(int argc, char *argv[], const char *name);

/**
 * @brief Returns the first argv index matching the given name after an index.
 *
 * Searches argv starting after index.
 *
 * @param argc The number of command line arguments.
 * @param argv The command line arguments.
 * @param name The exact argument name to find.
 * @param index The argv index to search after.
 *
 * @return The argv index, or TL_ARG_NOT_FOUND when not found.
 */
size_t tl_flag_get_arg_index_after(int argc, char *argv[], const char *name, size_t index);

/**
 * @brief Parses the given command line arguments with options.
 *
 * Default parsing is used when options is NULL or both option lists are NULL.
 * With default parsing this behaves like tl_flag_parse_args. Strict mode is enabled
 * when either option list is non-NULL.
 *
 * @param argc The number of command line arguments.
 * @param argv The command line arguments.
 * @param options The parse options, or NULL for default parsing.
 *
 * @return TL_PARSE_OK on success, or a negative parse error.
 */
TlParseResult tl_flag_parse_args_with_options(int argc, char *argv[],
                                              const TlParseOptions *options);

/**
 * @brief Parses an explicit argv range.
 *
 * Parses argv indexes [start_index, end_index). argv[0] has no special meaning
 * for this function. The parsed positional indexes are relative to the selected
 * range.
 *
 * @param argc The number of command line arguments.
 * @param argv The command line arguments.
 * @param start_index The first argv index to parse.
 * @param end_index The argv index one past the last token to parse.
 * @param options The parse options, or NULL for default parsing.
 *
 * @return TL_PARSE_OK on success, or a negative parse error.
 */
TlParseResult tl_flag_parse_args_range(int argc, char *argv[], size_t start_index, size_t end_index,
                                       const TlParseOptions *options);

/**
 * @brief Parses the given command line arguments.
 *
 * Parses argv into flags and positionals. A flag is anything starting
 * with "-" or "--" (e.g. "-h", "--help"). It can carry a value written as
 * --name=value, or as --name value in the next entry. A bare "-" is a
 * positional. A bare "--" ends flag parsing; everything after it is a
 * positional, even if it starts with dashes. Any previously parsed state
 * is thrown away first.
 *
 * @param argc The number of command line arguments.
 * @param argv The command line arguments.
 *
 * @return TL_PARSE_OK on success, or a negative parse error.
 */
TlParseResult tl_flag_parse_args(int argc, char *argv[]);

/**
 * @brief Parses a raw command line string.
 *
 * Splits the line into tokens. Double quotes group text with spaces into
 * one token, and a backslash keeps the next character as-is. The first
 * token is the program name and is skipped, like argv[0].
 *
 * @param line The command line string to parse.
 *
 * @return TL_PARSE_OK on success, or a negative parse error.
 */
TlParseResult tl_flag_parse_line(const char *line);

/**
 * @brief Releases memory held by the argument parser.
 *
 * Safe to call when nothing has been parsed. Called implicitly by
 * tl_flag_parse_args and tl_flag_parse_line.
 *
 * @return void
 */
void tl_flag_free_args(void);

/**
 * @brief Looks up a specific flag.
 *
 * @param flag The flag to look up.
 *
 * @return true if the flag is found, false otherwise.
 */
bool tl_flag_has_flag(const char *flag);

/**
 * @brief Returns the value of a specific flag.
 *
 * Returns the value of the first occurrence of flag. For repeated flags
 * use tl_flag_count_flag and tl_flag_get_value_at.
 *
 * @param flag The flag to get.
 *
 * @return The value of the flag, or NULL if not found or no value.
 */
const char *tl_flag_get_value(const char *flag);

/**
 * @brief Returns the number of times a flag was given.
 *
 * @param flag The flag to count.
 *
 * @return The occurrence count (0 if not given).
 */
size_t tl_flag_count_flag(const char *flag);

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
const char *tl_flag_get_value_at(const char *flag, size_t index);

/**
 * @brief Looks up a specific positional argument by value.
 *
 * @param value The positional value to look up.
 *
 * @return true if the positional is found, false otherwise.
 */
bool tl_flag_has_positional(const char *value);

/**
 * @brief Returns the number of positional arguments.
 *
 * Positionals are bare arguments (not starting with `-` or `--`) and
 * everything after a bare `--` terminator, in the order they appeared.
 *
 * @return The positional argument count.
 */
size_t tl_flag_count_positional(void);

/**
 * @brief Returns the positional argument at the given index.
 *
 * @param index The positional index (0-based).
 *
 * @return The positional value, or NULL if out of range.
 */
const char *tl_flag_get_positional(size_t index);

#endif // TL_FLAG_H
