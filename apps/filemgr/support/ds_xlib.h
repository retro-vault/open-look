
/*  @(#)ds_xlib.h	1.3 08/31/92
 *
 *  Copyright (c) 1988-1992  Sun Microsystems, Inc.
 *  All Rights Reserved.
 *
 *  Sun considers its source code as an unpublished, proprietary
 *  trade secret, and it is available only under strict license
 *  provisions.  This copyright notice is placed here only to protect
 *  Sun in the event the source is deemed a published work.  Dissassembly,
 *  decompilation, or other means of reducing the object code to human
 *  readable form is prohibited by the license agreement under which
 *  this code is provided to the user or company in posesion of this copy.
 *
 *  RESTRICTED RIGHTS LEGEND:  Use, duplication, or disclosure by the
 *  Government is subject to restrictions as set forth in subparagraph
 *  (c)(1)(ii) of the Rights in Technical Data and Computer Software
 *  clause at DFARS 52.227-7013 and in similar clauses in the FAR and
 *  NASA FAR Supplement.
 */

#ifndef _ds_xlib_h
#define _ds_xlib_h

#include <xview/xview.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>

/* For all function declarations, if ANSI then use a prototype. */

#if  defined(__STDC__)
#define P(args)  args
#else  /* ! __STDC__ */
#define P(args)  ()
#endif /* STDC */

/* Function declarations. */

XFontStruct *ds_get_font          P((Display *, char *, int)) ;
XrmDatabase ds_load_deskset_defs  P(()) ;
XrmDatabase ds_load_resources     P((Display *)) ;
Xv_opaque ds_add_help             P((Xv_opaque, char *)) ;
Xv_opaque ds_save_cmdline         P((Frame, int, char *[])) ;
Xv_opaque ds_save_resources       P((XrmDatabase)) ;
Xv_opaque ds_set_frame_size       P((Xv_Window, int, int, int, int)) ;

Rect *ds_get_frame_size           P((Xv_Window, int *, int *, int *, int *)) ;

char *ds_get_resource             P((XrmDatabase, char *, char *)) ;

int ds_get_strwidth               P((XFontStruct *, char *)) ;

void ds_beep                      P((Display *)) ;
void ds_get_geometry_size         P((char *, int *, int *)) ;
void ds_put_resource              P((XrmDatabase *, char *, char *, char *)) ;

#endif /*_ds_xlib_h*/
