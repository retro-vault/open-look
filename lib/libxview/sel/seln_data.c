/*
 * Implements the seln data routines used by the XView sel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/seln_impl.h>


Seln_function_buffer seln_null_function = {
    SELN_FN_ERROR, SELN_UNKNOWN,
    SELN_NULL_HOLDER, SELN_NULL_HOLDER,
    SELN_NULL_HOLDER, SELN_NULL_HOLDER
};
Seln_holder     seln_null_holder = SELN_NULL_HOLDER;
Seln_request    seln_null_request = {
    0, {0, 0}, 0, SELN_UNKNOWN, SELN_FAILED, 0, 0
};
