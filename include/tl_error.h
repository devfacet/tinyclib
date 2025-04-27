// See LICENSE.txt and CONTRIBUTING.md for details.

#ifndef TL_ERROR_H
#define TL_ERROR_H

#include <stddef.h>

/**
 * @brief Represents an error code.
 */
typedef enum {
    TL_ERROR_NONE = 0, // no error

    TL_ERROR_INTERNAL = 10,   // internal error
    TL_ERROR_NOT_FOUND,       // not found
    TL_ERROR_NOT_READY,       // not ready
    TL_ERROR_NOT_IMPLEMENTED, // not implemented
    TL_ERROR_NOT_SUPPORTED,   // not supported
    TL_ERROR_NOT_AVAILABLE,   // not available

    TL_ERROR_TIMEOUT = 20,      // timeout error
    TL_ERROR_BUSY,              // busy
    TL_ERROR_IO,                // I/O error
    TL_ERROR_OUT_OF_RANGE,      // out of range error
    TL_ERROR_MEMORY_ALLOCATION, // memory allocation error

    TL_ERROR_INIT_FAILED = 30,    // initialization error
    TL_ERROR_ALREADY_INITIALIZED, // already initialized
    TL_ERROR_NOT_INITIALIZED,     // not initialized
    TL_ERROR_INVALID_ARGUMENT,    // invalid argument
    TL_ERROR_INVALID_FUNCTION,    // invalid function
    TL_ERROR_INVALID_INSTANCE,    // invalid instance
    TL_ERROR_INVALID_SIZE,        // invalid size
    TL_ERROR_INVALID_TYPE,        // invalid type
    TL_ERROR_INVALID_VALUE,       // invalid value
} TLErrorCode;

/**
 * @brief Represents an error.
 *
 * @param code The error code.
 * @param message_size The size of the error message.
 * @param message The error message.
 */
typedef struct {
    TLErrorCode code;
    size_t message_size;
    const char *message;
} TLError;

/**
 * @brief Sets the error with the given code and message.
 *
 * @param error The error to set.
 * @param code The error code.
 * @param message The error message.
 * @param ... The arguments for the formatted message.
 *
 * @return void
 */
void tl_error_set(TLError *error, TLErrorCode code, const char *message, ...);

#endif // TL_ERROR_H
