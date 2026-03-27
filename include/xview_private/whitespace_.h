/*
 * Declares private XView interfaces, types, and macros for whitespace.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(WHITESPACE__H)
#define WHITESPACE__H

#include <xview/pkg.h>
#include <xview/str_utils.h>

enum CharClass xv_white_space(char c);
struct CharAction xv_everything(char c);

#endif
