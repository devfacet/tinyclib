// See LICENSE.txt and CONTRIBUTING.md for details.

#include "tl_error.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void tl_error_set(TLError *error, TLErrorCode code, const char *message, ...) {
    // Ignore if error is NULL
    if (!error) {
        return;
    }

    // Set error code and message
    error->code = code;
    // Clear previous error message
    if (error->message) {
        free((void *)error->message);
        error->message = NULL;
        error->message_size = 0;
    }

    // Set message if provided
    if (message) {
        va_list args;
        va_list args_copy;

        va_start(args, message);
        va_copy(args_copy, args);

        int size = vsnprintf(NULL, 0, message, args_copy);
        va_end(args_copy);

        if (size < 0) {
            va_end(args);
            return; // ignore if vsnprintf fails
        }

        error->message_size = (size_t)size + 1;
        error->message = malloc(error->message_size); // include null terminator by adding 1
        if (!error->message) {
            va_end(args);
            return; // ignore if malloc fails
        }

        vsnprintf((char *)error->message, error->message_size, message, args);
        va_end(args);
    }
}
