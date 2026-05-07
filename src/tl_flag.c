// See LICENSE.txt and CONTRIBUTING.md for details.

#include "tl_flag.h"
#include <stdlib.h>
#include <string.h>

// Init vars
static TlFlag      *flags            = NULL;
static size_t       flag_count       = 0;
static const char **positionals      = NULL;
static size_t       positional_count = 0;
static char        *line_buf         = NULL;
static char       **line_tokens      = NULL;

/**
 * @brief Returns whether the token is a flag.
 *
 * A flag starts with "-" or "--" and is not a bare "-" or "--".
 * A bare "-" is not a flag (it's a common stdin placeholder).
 * A bare "--" is the positional terminator and is handled separately.
 */
static bool is_flag(const char *s) {
    if (s == NULL || s[0] != '-' || s[1] == '\0') {
        return false;
    }
    if (s[1] == '-' && s[2] == '\0') {
        return false;
    }
    return true;
}

/**
 * @brief Returns whether the token is the bare "--" terminator.
 */
static bool is_dash_dash(const char *s) {
    if (s == NULL) {
        return false;
    }
    if (s[0] != '-') {
        return false;
    }
    if (s[1] != '-') {
        return false;
    }
    if (s[2] != '\0') {
        return false;
    }
    return true;
}

/**
 * @brief Returns whether a parsed flag matches the given name.
 */
static bool is_flag_match(const TlFlag *f, const char *name, size_t name_len) {
    if (f->name_len != name_len) {
        return false;
    }
    return memcmp(f->name, name, name_len) == 0;
}

/**
 * @brief Returns whether the argv token matches a name.
 */
static bool has_token_match(const char *const argv[], size_t index, const char *name) {
    return argv[index] && strcmp(argv[index], name) == 0;
}

/**
 * @brief Returns whether the flag name appears in a NULL-terminated list.
 */
static bool has_flag_name(const char *const *names, const char *name, size_t name_len) {
    if (!names || !name) {
        return false;
    }
    for (size_t i = 0; names[i]; i++) {
        size_t current_len = strlen(names[i]);
        if (current_len == name_len && memcmp(names[i], name, name_len) == 0) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Returns whether parser options require strict flag matching.
 */
static bool has_strict_flag_lists(const TlParseOptions *options) {
    if (!options) {
        return false;
    }
    return options->value_flags || options->bool_flags;
}

/**
 * @brief Returns whether any flag appears in both strict flag lists.
 */
static bool has_conflicting_flags(const TlParseOptions *options) {
    if (!options || !options->value_flags || !options->bool_flags) {
        return false;
    }
    for (size_t i = 0; options->value_flags[i]; i++) {
        size_t value_len = strlen(options->value_flags[i]);
        if (has_flag_name(options->bool_flags, options->value_flags[i], value_len)) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Adds a parsed flag entry.
 */
static void add_flag(const char *name, size_t name_len, const char *value) {
    flags[flag_count].name     = name;
    flags[flag_count].name_len = name_len;
    flags[flag_count].value    = value;
    flag_count++;
}

/**
 * @brief Returns the default parsing value for a space-form flag.
 */
static const char *take_default_flag_value(char **tokens, size_t *index, size_t end_index) {
    size_t next_index = *index + 1;
    if (next_index >= end_index) {
        return NULL;
    }
    if (is_flag(tokens[next_index]) || is_dash_dash(tokens[next_index])) {
        return NULL;
    }
    *index = next_index;
    return tokens[next_index];
}

/**
 * @brief Takes the required strict-mode value for a value flag.
 */
static TlParseResult take_strict_flag_value(char **tokens, size_t *index, size_t end_index,
                                            const char **value) {
    size_t next_index = *index + 1;
    if (next_index >= end_index) {
        return TL_PARSE_ERROR_MISSING_VALUE;
    }
    if (is_flag(tokens[next_index]) || is_dash_dash(tokens[next_index])) {
        return TL_PARSE_ERROR_MISSING_VALUE;
    }
    *index = next_index;
    *value = tokens[next_index];
    return TL_PARSE_OK;
}

/**
 * @brief Parses one flag token into the flag table.
 */
static TlParseResult parse_flag_token(char **tokens, size_t *index, size_t end_index,
                                      const TlParseOptions *options, bool strict) {
    const char *tok        = tokens[*index];
    const char *eq         = strchr(tok, '=');
    size_t      name_len   = eq ? (size_t)(eq - tok) : strlen(tok);
    bool        value_flag = false;

    if (strict) {
        value_flag = has_flag_name(options->value_flags, tok, name_len);
        if (!value_flag && !has_flag_name(options->bool_flags, tok, name_len)) {
            return TL_PARSE_ERROR_UNKNOWN_FLAG;
        }
    }

    if (eq) {
        add_flag(tok, name_len, eq + 1);
        return TL_PARSE_OK;
    }

    const char *value = NULL;
    if (strict && value_flag) {
        TlParseResult result = take_strict_flag_value(tokens, index, end_index, &value);
        if (result != TL_PARSE_OK) {
            return result;
        }
    }
    if (!strict) {
        value = take_default_flag_value(tokens, index, end_index);
    }

    add_flag(tok, name_len, value);
    return TL_PARSE_OK;
}

/**
 * @brief Fills the flag and positional tables from a token list.
 *
 * Tokens in [start_index, end_index) are sorted into flags (anything starting
 * with "-" or "--") and positionals (everything else, plus anything after a
 * bare "--").
 */
static TlParseResult parse_token_range(char **tokens, size_t start_index, size_t end_index,
                                       const TlParseOptions *options) {
    if (start_index == end_index) {
        return TL_PARSE_OK;
    }
    if (!tokens) {
        return TL_PARSE_ERROR_INVALID_RANGE;
    }
    if (has_conflicting_flags(options)) {
        return TL_PARSE_ERROR_CONFLICTING_FLAG;
    }

    size_t count = end_index - start_index;
    flags        = (TlFlag *)calloc(count, sizeof(*flags));
    positionals  = (const char **)calloc(count, sizeof(*positionals));
    if (!flags || !positionals) {
        return TL_PARSE_ERROR_MEMORY_ALLOCATION;
    }

    bool after_dd = false;
    bool strict   = has_strict_flag_lists(options);
    for (size_t i = start_index; i < end_index; i++) {
        char *tok = tokens[i];
        if (!tok) {
            continue;
        }
        // After "--" everything is positional
        if (after_dd) {
            positionals[positional_count++] = tok;
            continue;
        }
        // Bare "--" terminator
        if (is_dash_dash(tok)) {
            after_dd = true;
            continue;
        }
        // Flag
        if (is_flag(tok)) {
            TlParseResult result = parse_flag_token(tokens, &i, end_index, options, strict);
            if (result != TL_PARSE_OK) {
                return result;
            }
            continue;
        }
        // Positional
        positionals[positional_count++] = tok;
    }
    return TL_PARSE_OK;
}

/**
 * @brief Reads one token from `line` starting at `*i` into `line_buf` at `*bi`.
 *
 * Stops at unquoted whitespace or end of line. Writes the NUL terminator.
 */
static TlParseResult read_one_token(const char *line, size_t len, size_t *i, size_t *bi) {
    bool in_quote = false;
    while (*i < len) {
        char c = line[*i];
        if (!in_quote && (c == ' ' || c == '\t')) {
            break;
        }
        if (c == '"') {
            if (in_quote) {
                in_quote = false;
            } else {
                in_quote = true;
            }
            (*i)++;
            continue;
        }
        if (c == '\\' && *i + 1 < len) {
            line_buf[(*bi)++] = line[*i + 1];
            *i += 2;
            continue;
        }
        line_buf[(*bi)++] = c;
        (*i)++;
    }
    if (in_quote) {
        return TL_PARSE_ERROR_UNTERMINATED_QUOTE;
    }
    line_buf[(*bi)++] = '\0';
    return TL_PARSE_OK;
}

/**
 * @brief Splits a command line string into tokens stored in line_tokens.
 *
 * Text inside double quotes is kept as one token, spaces and all. The
 * quote characters themselves are dropped. A backslash keeps the next
 * character as-is (e.g. \" or \\).
 */
static TlParseResult tokenize_line(const char *line, size_t *token_count) {
    if (!line || !token_count) {
        return TL_PARSE_ERROR_INVALID_INPUT;
    }

    size_t len = strlen(line);
    line_buf   = malloc(len + 1);
    if (!line_buf) {
        return TL_PARSE_ERROR_MEMORY_ALLOCATION;
    }

    // Upper bound: one token per two bytes, plus a slot for the "argv[0]" entry
    size_t tok_cap = (len / 2) + 2;
    line_tokens    = (char **)malloc(tok_cap * sizeof(*line_tokens));
    if (!line_tokens) {
        return TL_PARSE_ERROR_MEMORY_ALLOCATION;
    }

    size_t n  = 0;
    size_t bi = 0;
    size_t i  = 0;
    while (i < len) {
        // Skip leading whitespace
        while (i < len && (line[i] == ' ' || line[i] == '\t')) {
            i++;
        }
        if (i >= len) {
            break;
        }
        line_tokens[n++]     = &line_buf[bi];
        TlParseResult result = read_one_token(line, len, &i, &bi);
        if (result != TL_PARSE_OK) {
            return result;
        }
    }
    *token_count = n;
    return TL_PARSE_OK;
}

size_t tl_arg_index(int argc, char *argv[], const char *name) {
    if (argc <= 0 || !argv || !name) {
        return TL_ARG_NOT_FOUND;
    }
    for (int i = 0; i < argc; i++) {
        if (has_token_match((const char *const *)argv, (size_t)i, name)) {
            return (size_t)i;
        }
    }
    return TL_ARG_NOT_FOUND;
}

size_t tl_arg_index_after(int argc, char *argv[], const char *name, size_t index) {
    if (index == TL_ARG_NOT_FOUND || argc <= 0 || !argv || !name) {
        return TL_ARG_NOT_FOUND;
    }
    size_t start = index + 1;
    if (start >= (size_t)argc) {
        return TL_ARG_NOT_FOUND;
    }
    for (size_t i = start; i < (size_t)argc; i++) {
        if (has_token_match((const char *const *)argv, i, name)) {
            return i;
        }
    }
    return TL_ARG_NOT_FOUND;
}

TlParseResult tl_parse_args_ex(int argc, char *argv[], const TlParseOptions *options) {
    tl_free_args();
    if (argc <= 1 || !argv) {
        return TL_PARSE_OK;
    }
    TlParseResult result = parse_token_range(argv, 1, (size_t)argc, options);
    if (result != TL_PARSE_OK) {
        tl_free_args();
    }
    return result;
}

TlParseResult tl_parse_args_range(int argc, char *argv[], size_t start_index, size_t end_index,
                                  const TlParseOptions *options) {
    tl_free_args();
    if (argc < 0 || start_index == TL_ARG_NOT_FOUND || end_index == TL_ARG_NOT_FOUND ||
        start_index > end_index || start_index > (size_t)argc || end_index > (size_t)argc) {
        return TL_PARSE_ERROR_INVALID_RANGE;
    }
    if (start_index == end_index) {
        return TL_PARSE_OK;
    }
    TlParseResult result = parse_token_range(argv, start_index, end_index, options);
    if (result != TL_PARSE_OK) {
        tl_free_args();
    }
    return result;
}

TlParseResult tl_parse_args(int argc, char *argv[]) {
    return tl_parse_args_ex(argc, argv, NULL);
}

TlParseResult tl_parse_line(const char *line) {
    tl_free_args();
    if (!line) {
        return TL_PARSE_ERROR_INVALID_INPUT;
    }
    size_t        n      = 0;
    TlParseResult result = tokenize_line(line, &n);
    if (result != TL_PARSE_OK) {
        tl_free_args();
        return result;
    }
    if (n <= 1) {
        return TL_PARSE_OK;
    }
    result = parse_token_range(line_tokens, 1, n, NULL);
    if (result != TL_PARSE_OK) {
        tl_free_args();
    }
    return result;
}

void tl_free_args(void) {
    if (flags) {
        free(flags);
        flags = NULL;
    }
    flag_count = 0;
    if (positionals) {
        free((void *)positionals);
        positionals = NULL;
    }
    positional_count = 0;
    if (line_tokens) {
        free((void *)line_tokens);
        line_tokens = NULL;
    }
    if (line_buf) {
        free(line_buf);
        line_buf = NULL;
    }
}

bool tl_lookup_flag(const char *flag) {
    if (!flag) {
        return false;
    }
    size_t flen = strlen(flag);
    for (size_t i = 0; i < flag_count; i++) {
        if (is_flag_match(&flags[i], flag, flen)) {
            return true;
        }
    }
    return false;
}

const char *tl_get_flag(const char *flag) {
    return tl_get_flag_at(flag, 0);
}

size_t tl_count_flag(const char *flag) {
    if (!flag) {
        return 0;
    }
    size_t flen = strlen(flag);
    size_t n    = 0;
    for (size_t i = 0; i < flag_count; i++) {
        if (is_flag_match(&flags[i], flag, flen)) {
            n++;
        }
    }
    return n;
}

const char *tl_get_flag_at(const char *flag, size_t index) {
    if (!flag) {
        return NULL;
    }
    size_t flen = strlen(flag);
    size_t k    = 0;
    for (size_t i = 0; i < flag_count; i++) {
        if (is_flag_match(&flags[i], flag, flen)) {
            if (k == index) {
                return flags[i].value;
            }
            k++;
        }
    }
    return NULL;
}

bool tl_lookup_positional(const char *value) {
    if (!value) {
        return false;
    }
    for (size_t i = 0; i < positional_count; i++) {
        if (strcmp(positionals[i], value) == 0) {
            return true;
        }
    }
    return false;
}

size_t tl_count_positional(void) {
    return positional_count;
}

const char *tl_get_positional(size_t index) {
    if (index >= positional_count) {
        return NULL;
    }
    return positionals[index];
}
