/*
 * fix for shared libraries in SunOS4.0. Code was isolated from.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview/pkg.h>
#include <xview/rect.h>
#include <xview/rectlist.h>

/*
 * rectlist constants
 */
struct rectlist rl_null = {0, 0, 0, 0, 0, 0, 0, 0};

struct rect     rect_null = {0, 0, 0, 0};
