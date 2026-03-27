/*
 * Declares public XView interfaces, types, and attributes for ttysw.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifndef xview_ttysw_DEFINED
#define xview_ttysw_DEFINED

#include <xview/xv_c_types.h>
#include <xview/tty.h>
#include <xview_private/tty_impl.h>

/*
 ***********************************************************************
 *			Definitions and Macros
 ***********************************************************************
 */

/*
 * PRIVATE #defines 
 */

/* 
 * options - controlled by ttysw_getopt(), ttysw_setopt
 * The values of the #define's are wired into aliases, shell-scripts,
 * etc. and should not be changed!
 */
#define	TTYOPT_PAGEMODE			1
#define TTYOPT_SELSVC			3
#define TTYOPT_TEXT			4	/* TERMSW */

/*
 * styles for rendering boldface characters 
 */
#define TTYSW_BOLD_NONE			0x0
#define TTYSW_BOLD_OFFSET_X		0x1
#define TTYSW_BOLD_OFFSET_Y		0x2
#define TTYSW_BOLD_OFFSET_XY		0x4
#define TTYSW_BOLD_INVERT		0x8
#define TTYSW_BOLD_MAX			0x8

/*
 * Modes for invert and underline 
 */
#define TTYSW_ENABLE			0x0
#define TTYSW_DISABLE			0x1
#define TTYSW_SAME_AS_BOLD		0x2

/*
 ***********************************************************************
 *		Typedefs, Enumerations, and Structures
 ***********************************************************************
 */

typedef caddr_t	Ttysubwindow;

/*
 ***********************************************************************
 *				Globals
 ***********************************************************************
 */

EXTERN_FUNCTION (int ttysw_input, (Tty ttysw, char *addr, int len));
EXTERN_FUNCTION (int ttysw_output, (Tty ttysw, char *addr, int len));



/*
 * C Library routines specifically related to ttysw subwindow functions.
 */

/*
 * PRIVATE functions 
 */

EXTERN_FUNCTION (void ttysw_done, (Ttysw_folio ttysw));
EXTERN_FUNCTION (void ttysw_setopt, (Ttysw_folio ttysw, int opt, int on));
EXTERN_FUNCTION (int ttysw_getopt, (Ttysw_folio ttysw, int opt));
EXTERN_FUNCTION (Pkg_private int ttysw_cooked_echo_cmd, (Ttysw_view_handle ttysw_view, char *buf, int buflen));
EXTERN_FUNCTION (Pkg_private int ttysw_copy_to_input_buffer, (Ttysw_folio ttysw, CHAR *addr, int len));

/*
 * PUBLIC functions
 * for compatibility with pre-SunView 1 code
 */
EXTERN_FUNCTION (void ttysw_becomeconsole, (Tty ttysw));

#endif /* ~xview_ttysw_DEFINED */
