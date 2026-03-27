/*
 * Declares public XView interfaces, types, and attributes for win env.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(WIN_ENV__H)
#define WIN_ENV__H

#include <xview/pkg.h>
#include <xview/rect.h>

void we_setparentwindow(char *windevname);
int we_getparentwindow(char *windevname);
void we_setgfxwindow(char *windevname);
int we_getgfxwindow(char *windevname);
void we_setinitdata(struct rect *initialrect, struct rect *initialsavedrect, int iconic); 
int  we_getinitdata(struct rect *initialrect, struct rect *initialsavedrect, int *iconic); 
int _we_setstrfromenvironment(char *tag, char *target);

#endif
