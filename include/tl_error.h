// See LICENSE.txt and CONTRIBUTING.md for details.

#ifndef TL_ERROR_H
#define TL_ERROR_H

#include <stddef.h>

/**
 * @brief Represents an error code.
 */
typedef enum {
    TL_ERROR_NONE                = 0,  // no error
    TL_ERROR_INTERNAL            = 10, // internal error
    TL_ERROR_NOT_FOUND           = 11, // not found
    TL_ERROR_NOT_READY           = 12, // not ready
    TL_ERROR_NOT_IMPLEMENTED     = 13, // not implemented
    TL_ERROR_NOT_SUPPORTED       = 14, // not supported
    TL_ERROR_NOT_AVAILABLE       = 15, // not available
    TL_ERROR_TIMEOUT             = 20, // timeout error
    TL_ERROR_BUSY                = 21, // busy
    TL_ERROR_IO                  = 22, // I/O error
    TL_ERROR_OUT_OF_RANGE        = 23, // out of range error
    TL_ERROR_MEMORY_ALLOCATION   = 24, // memory allocation error
    TL_ERROR_INIT_FAILED         = 30, // initialization error
    TL_ERROR_ALREADY_INITIALIZED = 31, // already initialized
    TL_ERROR_NOT_INITIALIZED     = 32, // not initialized
    TL_ERROR_INVALID_ARGUMENT    = 33, // invalid argument
    TL_ERROR_INVALID_FUNCTION    = 34, // invalid function
    TL_ERROR_INVALID_INSTANCE    = 35, // invalid instance
    TL_ERROR_INVALID_SIZE        = 36, // invalid size
    TL_ERROR_INVALID_TYPE        = 37, // invalid type
    TL_ERROR_INVALID_VALUE       = 38, // invalid value
} TlErrorCode;

/**
 * @brief Represents an error.
 *
 * @param code The error code.
 * @param message_size The size of the error message.
 * @param message The error message.
 */
typedef struct {
    TlErrorCode code;
    size_t      message_size;
    const char *message;
} TlError;

/**
 * @brief Sets the error with the given code and message.
 *
 * @param error The error to set.
 * @param code The error code.
 * @param ... The error message followed by the formatted message arguments.
 *
 * @return TL_ERROR_NONE on success, or an error code on failure.
 */
int tl_error_set_message(TlError *error, TlErrorCode code, ...);

#endif // TL_ERROR_H
