// See LICENSE.txt and CONTRIBUTING.md for details.

#include "tl_flag.h"
#include <stdlib.h>
#include <string.h>

// Init vars
static tl_flag_t   *flags            = NULL;
static size_t       flag_count       = 0;
static const char **positionals      = NULL;
static size_t       positional_count = 0;
static char        *line_buf         = NULL;
static char       **line_tokens      = NULL;

/**
 * @brief Returns whether the token is a long flag (starts with "--" and has content).
 */
static bool is_long_flag(const char *s) {
    return s != NULL && s[0] == '-' && s[1] == '-' && s[2] != '\0';
}

/**
 * @brief Returns whether the token is the bare "--" terminator.
 */
static bool is_dash_dash(const char *s) {
    return s != NULL && s[0] == '-' && s[1] == '-' && s[2] == '\0';
}

/**
 * @brief Returns whether a parsed flag matches the given name.
 */
static bool flag_matches(const tl_flag_t *f, const char *name, size_t name_len) {
    if (f->name_len != name_len) {
        return false;
    }
    return memcmp(f->name, name, name_len) == 0;
}

/**
 * @brief Fills the flag and positional tables from a token list.
 *
 * The first token is the program name and is skipped. The rest are
 * sorted into flags (anything starting with "--") and positionals
 * (everything else, plus anything after a bare "--").
 */
static bool parse_tokens(char **tokens, int count) {
    if (count <= 1 || !tokens) {
        return true;
    }

    flags       = (tl_flag_t *)calloc((size_t)count, sizeof(*flags));
    positionals = (const char **)calloc((size_t)count, sizeof(*positionals));
    if (!flags || !positionals) {
        return false;
    }

    bool after_dd = false;
    for (int i = 1; i < count; i++) {
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
        // Long flag
        if (is_long_flag(tok)) {
            char *eq = strchr(tok, '=');
            if (eq) {
                flags[flag_count].name     = tok;
                flags[flag_count].name_len = (size_t)(eq - tok);
                flags[flag_count].value    = eq + 1;
            } else {
                const char *value = NULL;
                // Consume the next token as the value if it is not another flag
                // and not the "--" terminator
                if (i + 1 < count && !is_long_flag(tokens[i + 1]) && !is_dash_dash(tokens[i + 1])) {
                    value = tokens[i + 1];
                    i++;
                }
                flags[flag_count].name     = tok;
                flags[flag_count].name_len = strlen(tok);
                flags[flag_count].value    = value;
            }
            flag_count++;
            continue;
        }
        // Positional
        positionals[positional_count++] = tok;
    }
    return true;
}

/**
 * @brief Splits a command line string into tokens stored in line_tokens.
 *
 * Text inside double quotes is kept as one token, spaces and all. The
 * quote characters themselves are dropped. A backslash keeps the next
 * character as-is (e.g. \" or \\). Returns the token count on success,
 * or -1 if malloc fails or a quote is never closed.
 */
static int tokenize_line(const char *line) {
    size_t len = strlen(line);
    line_buf   = malloc(len + 1);
    if (!line_buf) {
        return -1;
    }

    // Upper bound: one token per two bytes, plus a slot for the "argv[0]" entry
    size_t tok_cap = (len / 2) + 2;
    line_tokens    = (char **)malloc(tok_cap * sizeof(*line_tokens));
    if (!line_tokens) {
        return -1;
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
        // Start a new token at the current buffer position
        line_tokens[n++] = &line_buf[bi];
        bool in_quote    = false;
        while (i < len) {
            char c = line[i];
            if (!in_quote && (c == ' ' || c == '\t')) {
                break;
            }
            if (c == '"') {
                in_quote = in_quote ? false : true;
                i++;
                continue;
            }
            if (c == '\\' && i + 1 < len) {
                line_buf[bi++] = line[i + 1];
                i += 2;
                continue;
            }
            line_buf[bi++] = c;
            i++;
        }
        if (in_quote) {
            return -1; // unterminated quoted string
        }
        line_buf[bi++] = '\0';
    }
    return (int)n;
}

void tl_parse_args(int argc, char *argv[]) {
    tl_free_args();
    if (!parse_tokens(argv, argc)) {
        tl_free_args();
    }
}

bool tl_parse_line(const char *line) {
    tl_free_args();
    if (!line) {
        return false;
    }
    int n = tokenize_line(line);
    if (n < 0) {
        tl_free_args();
        return false;
    }
    if (!parse_tokens(line_tokens, n)) {
        tl_free_args();
        return false;
    }
    return true;
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
        if (flag_matches(&flags[i], flag, flen)) {
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
        if (flag_matches(&flags[i], flag, flen)) {
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
        if (flag_matches(&flags[i], flag, flen)) {
            if (k == index) {
                return flags[i].value;
            }
            k++;
        }
    }
    return NULL;
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
