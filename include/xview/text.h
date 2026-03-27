/*
 * Declares public XView interfaces, types, and attributes for text.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifndef xview_text_DEFINED
#define xview_text_DEFINED

/*
 ***********************************************************************
 *			Include Files
 ***********************************************************************
 */

#include <xview/textsw.h>

/*
 ***********************************************************************
 *			Definitions and Macros
 ***********************************************************************
 */

#define TEXT_TYPE 	ATTR_PKG_TEXTSW
#define TEXT 		textsw_window_object, WIN_COMPATIBILITY

/*
 ***********************************************************************
 *			Typedefs
 ***********************************************************************
 */

typedef Textsw Text;

#endif /* ~xview_text_DEFINED  */
