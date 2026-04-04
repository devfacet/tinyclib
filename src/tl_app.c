// See LICENSE.txt and CONTRIBUTING.md for details.

#include "tl_app.h"
#include "tl_config.h"
#include "tl_flag.h"
#include <stdlib.h>

void tl_init_app(int argc, char *argv[]) {
    tl_parse_args(argc, argv);
    if (tl_get_flag("--debug-level")) {
        tl_set_debug_level((int)strtol(tl_get_flag("--debug-level"), NULL, 10));
    }
}
