// See LICENSE.txt and CONTRIBUTING.md for details.

#include "tl_app.h"
#include "tl_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Init vars
static int    arg_count = 0;
static char **args      = NULL;

void tl_init_app(int argc, char *argv[]) {
    tl_parse_args(argc, argv);
    if (tl_get_flag("--debug-level")) {
        tl_set_debug_level((int)strtol(tl_get_flag("--debug-level"), NULL, 10));
    }
}

void tl_parse_args(int argc, char *argv[]) {
    arg_count = argc;
    args      = argv;
}

bool tl_lookup_flag(const char *flag) {
    for (int i = 1; i < arg_count; i++) {
        // If the argument starts with the flag and is followed by either '\0' or '=' then
        if (strncmp(args[i], flag, strlen(flag)) == 0 &&
            (args[i][strlen(flag)] == '\0' || args[i][strlen(flag)] == '=')) {
            return true;
        }
    }
    return false;
}

const char *tl_get_flag(const char *flag) {
    size_t flag_len = strlen(flag);
    for (int i = 1; i < arg_count; i++) {
        if (strncmp(args[i], flag, flag_len) != 0) {
            continue;
        }
        // If the argument is followed by '=' then return the value after '='
        if (args[i][flag_len] == '=') {
            return args[i] + flag_len + 1;
        }
        // If the argument is an exact match and the next argument exists then return it
        if (args[i][flag_len] == '\0' && i + 1 < arg_count) {
            return args[i + 1];
        }
    }
    return NULL;
}
