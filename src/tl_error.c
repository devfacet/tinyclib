// See LICENSE.txt and CONTRIBUTING.md for details.

#include "tl_error.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tl_error_set_message(TlError *error, TlErrorCode code, ...) {
    if (!error) {
        return TL_ERROR_INVALID_ARGUMENT;
    }

    // Set error code
    error->code = code;

    // Clear previous error message
    if (error->message) {
        free((void *)error->message);
        error->message      = NULL;
        error->message_size = 0;
    }

    // Set message if provided
    va_list args;
    va_start(args, code);
    const char *message = va_arg(args, const char *);
    if (message) {
        va_list format_args;
        va_copy(format_args, args);
        int size = vsnprintf(NULL, 0, message, format_args);
        va_end(format_args);

        if (size < 0) {
            va_end(args);
            return TL_ERROR_INTERNAL;
        }

        error->message = malloc(size + 1); // include null terminator by adding 1
        if (!error->message) {
            va_end(args);
            return TL_ERROR_MEMORY_ALLOCATION;
        }
        error->message_size = size + 1;

        vsnprintf((char *)error->message, error->message_size, message, args);
    }
    va_end(args);

    return TL_ERROR_NONE;
}
