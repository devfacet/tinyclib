// See LICENSE.txt and CONTRIBUTING.md for details.

#include "tl_error.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void tl_error_set(TLError *error, TLErrorCode code, const char *message, ...) {
    // Check and initialize error
    if (!error) {
        error = malloc(sizeof(TLError));
        if (!error) {
            return;
        }
        error->message = NULL;
        error->message_size = 0;
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
        va_start(args, message);
        int size = vsnprintf(NULL, 0, message, args);
        va_end(args);

        if (size < 0) {
            return; // ignore if vsnprintf fails
        }

        error->message = malloc(size + 1); // include null terminator by adding 1
        if (!error->message) {
            return; // ignore if malloc fails
        }
        error->message_size = size + 1;

        va_start(args, message);
        vsnprintf((char *)error->message, error->message_size, message, args);
        va_end(args);
    }
}
