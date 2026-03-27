/*
 * Implements the whitespace routines used by the XView string_utils
 * module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/whitespace_.h>

/*
 * these two procedures are in a separate file because they are also defined
 * in libio_stream.a, and having them be in string_utils.c tends to make the
 * loader complain about things being multiply defined
 */

enum CharClass
xv_white_space(c)
    char            c;
{
    switch (c) {
      case ' ':
      case '\n':
      case '\t':
	return (Sepr);
      default:
	return (Other);
    }
}

/* ARGSUSED */
struct CharAction
xv_everything(c)
    char            c;
{
    static struct CharAction val = {False, True};

    return (val);
}
