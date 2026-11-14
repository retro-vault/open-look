#ifndef lint
static char sccsid[]="@(#)ds_xlib.c	1.8 12/14/92 Copyright 1988-1992 Sun Microsystem, Inc." ;
#endif

/*  Copyright (c) 1988-1992  Sun Microsystems, Inc.
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

#include <stdlib.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/stat.h>
#include "ds_xlib.h"
#include <X11/Xutil.h>

#define  GETHOSTNAME  (void) gethostname
#define  SPRINTF      (void) sprintf

#define  DS_MAX_STR   120          /* Maximum length for character strings. */


/*  Function:     ds_add_help()
 *
 *  Purpose:      Add a help string to an XView item.
 *
 *  Parameters:   item       Xv_opaque
 *                           An opaque handle to the XView item.
 *
 *                str        The help string to be associated with this item.
 *
 *  Returns:      XV_ERROR   if unable to assign help text.
 *                XV_OK      on successful completion.
 */

Xv_opaque
ds_add_help(item, str)
Xv_opaque item ;
char *str ;
{
  return(xv_set(item, XV_HELP_DATA, str, 0)) ;
}


/*  Function:     ds_beep()
 *
 *  Purpose:      Ring the bell (at base volume).
 *
 *  Parameters:   display    connection to the X server.
 *                           (returned from XOpenDisplay).
 *
 *  Returns:      None.
 */

void
ds_beep(display)
Display *display ;
{
  XBell(display, 0) ;
}


/*  Function:     ds_get_font()
 *
 *  Purpose:      Get an Xlib handle to a font.
 *
 *  Parameters:   display    connection to the X server.
 *                           (returned from XOpenDisplay).
 *
 *                fontname   name of font to load. If this is NULL, or the
 *                           load fails, then ds_get_font() tries to load the
 *                           lucidatypewriter font. If this fails then the
 *                           fixed font is tried.
 *
 *                size       If fontname was NULL, then this indicates the
 *                           size (scale) of the lucidatypewriter font to load.
 *
 *  Returns:      A pointer to an XFontStruct, or NULL if a font can't be
 *                loaded.
 */

XFontStruct *
ds_get_font(display, fontname, size)
Display *display ;
char *fontname ;
int size ;
{
  char fname[DS_MAX_STR] ;    /* Used to construct the required font name. */
  XFontStruct *font ;

  SPRINTF(fname, "-b&h-lucidatypewriter-medium-r-*-*-*-%1d0-*-*-*-*-iso8859-1",
          size) ;
  if (fontname == NULL || !(font = XLoadQueryFont(display, fontname)))
    if (!(font = XLoadQueryFont(display, fname)))
      font = XLoadQueryFont(display, "fixed") ;
  return(font) ;
}


/*  Function:     ds_get_frame_size()
 *
 *  Purpose:      Return the bounding box of a window.
 *
 *  Parameters:   window     XView handle to the window.
 *
 *  Returns:      Parameters:  x          left most value.
 *                             y          top most value.
 *                             width      width of bounding box.
 *                             height     height of bounding box.
 *
 *                Value:       A pointer to the Rect structure for this window
 *                             or NULL for an invalid window.
 */

Rect *
ds_get_frame_size(window, x, y, width, height)
Xv_Window window ;
int *x, *y, *width, *height ;
{
  Rect *r ;

  r = (Rect *) xv_get(window, WIN_RECT) ;
  if (r != NULL)
    {
      *x      = r->r_left ;
      *y      = r->r_top ;
      *width  = r->r_width ;
      *height = r->r_height ;
    }
  return(r) ;
}


/*  Function:     ds_get_geometry_size()
 *
 *  Purpose:      Return width and height values from an X geometry string.
 *
 *  Parameters:   str          X geometry string.
 *
 *  Returns:      Parameters:  width      width   (-1 if invalid).
 *                             height     height  (-1 if invalid).
 *
 *                Value:       None.
 */

void
ds_get_geometry_size(str, width, height)
char *str ;
int *width, *height ;
{
  int reply, x, y ;
  unsigned int w, h ;

  *width = *height = -1 ;
  reply = XParseGeometry(str, &x, &y, &w, &h) ;
  if (reply & WidthValue)  *width  = w ;
  if (reply & HeightValue) *height = h ;
}


/*  Function:     ds_get_resource()
 *
 *  Purpose:      Get an X resource from the server.
 *
 *  Parameters:   rDB          X resources database.
 *
 *                appname      application name.
 *
 *                resource     X resource string to search for.
 *
 *  Returns:      resource string, or NULL if not found.
 *
 *  Note:         The first character of the appname and resource strings may
 *                be modified.
 */

char *
ds_get_resource(rDB, appname, resource)
XrmDatabase rDB ;                         /* Resources database. */
char *appname ;                           /* Application name. */
char *resource ;                          /* X resource to search for. */
{
  char cstr[DS_MAX_STR], nstr[DS_MAX_STR], str[DS_MAX_STR] ;
  char *str_type[20] ;
  XrmValue value ;

  if (isupper(appname[0])) appname[0] = tolower(appname[0]) ;
  SPRINTF(nstr, "deskset.%s.%s", appname, resource) ;

  if (islower(resource[0])) resource[0] = toupper(resource[0]) ;
  if (islower(appname[0])) appname[0] = toupper(appname[0]) ;
  SPRINTF(cstr, "Deskset.%s.%s", appname, resource) ;

  if (XrmGetResource(rDB, nstr, cstr, str_type, &value) == 0)
    return((char *) NULL) ;
  else return(value.addr) ;
}


/*  Function:     ds_get_strwidth()
 *
 *  Purpose:      Get the length in pixels of a text string, given the font
 *                it would be written in.
 *
 *  Parameters:   font         font XFontStruct pointer.
 *
 *                str          string
 *
 *  Returns:      length in pixels of the text string.
 */

int
ds_get_strwidth(font, str)
XFontStruct *font ;
char *str ;
{
  return(XTextWidth(font, str, strlen(str))) ;
}


/*  Function:     ds_load_deskset_defs()
 *
 *  Purpose:      Load the Deskset resources database from the
 *                DESKSETDEFAULTS environment variable (if set), or
 *                from $HOME/.desksetdefaults.
 *
 *  Parameters:   None.
 *
 *  Returns:      X resources database.
 */

XrmDatabase
ds_load_deskset_defs()
{
  XrmDatabase rDB ;
  char name[MAXPATHLEN], *ptr ;

  if ((ptr = getenv("DESKSETDEFAULTS")) == NULL)
    {
      SPRINTF(name, "%s/.desksetdefaults", getenv("HOME")) ;
      rDB = XrmGetFileDatabase(name) ;
    }
  else rDB = XrmGetFileDatabase(ptr) ;
  return(rDB) ;
}


/*  Function:     ds_load_resources()
 *
 *  Purpose:      Get the resource databases. These are looked for in the
 *                following ways:
 *
 *                Classname file in the app-defaults directory. In this case,
 *                Classname is Deskset.
 *
 *                Classname file in the directory specified by the
 *                XUSERFILESEARCHPATH or XAPPLRESDIR environment variable.
 *
 *                Property set using xrdb, accessible through the
 *                XResourceManagerString macro or, if that is empty, the
 *                ~/.Xdefaults file.
 *
 *                XENVIRONMENT environment variable or, if not set,
 *                .Xdefaults-hostname file.
 *
 *                DESKSETDEFAULTS environment variable or, if not set, the
 *                ~/.desksetdefaults file
 *
 *  Parameters:   display    connection to the X server.
 *                           (returned from XOpenDisplay).
 *
 *  Returns:      X combined resources database.
 */

XrmDatabase
ds_load_resources(display)
Display *display ;
{
  XrmDatabase db, rDB ;
  char *home, name[MAXPATHLEN], *owhome, *ptr ;
  int len ;

  rDB  = NULL ;
  home = getenv("HOME") ;
  XrmInitialize() ;
  if ((owhome = getenv("OPENWINHOME")) != NULL)
    {
      SPRINTF(name, "%s/lib/app-defaults/Deskset", getenv("OPENWINHOME")) ;

/* Get applications defaults file, if any. */

      db = XrmGetFileDatabase(name) ;
      XrmMergeDatabases(db, &rDB) ;
    }

/* Merge server defaults, created by xrdb. If nor defined, use ~/.Xdefaults. */

  if (XResourceManagerString(display) != NULL)
    db = XrmGetStringDatabase(XResourceManagerString(display)) ;
  else
    {
      SPRINTF(name, "%s/.Xdefaults", home) ;
      db = XrmGetFileDatabase(name) ;
    }
  XrmMergeDatabases(db, &rDB) ;

/*  Open XENVIRONMENT file or, if not defined, the .Xdefaults, and merge
 *  into existing database.
 */

  if ((ptr = getenv("XENVIRONMENT")) == NULL)
    {
      SPRINTF(name, "%s/.Xdefaults-", home) ;
      len = strlen(name) ;
      GETHOSTNAME(name+len, 1024-len) ;
      db = XrmGetFileDatabase(name) ;
    }
  else db = XrmGetFileDatabase(ptr) ;
  XrmMergeDatabases(db, &rDB) ;

/*  Finally merge in Deskset defaults via DESKSETDEFAULTS or, if not
 *  defined, the ~/.desksetdefaults file.
 */

  if ((ptr = getenv("DESKSETDEFAULTS")) == NULL)
    {
      SPRINTF(name, "%s/.desksetdefaults", home) ;
      db = XrmGetFileDatabase(name) ;
    }
  else db = XrmGetFileDatabase(ptr) ;
  XrmMergeDatabases(db, &rDB) ;
  return(rDB) ;
}


/*  Function:     ds_put_resource()
 *
 *  Purpose:      Adds an X resource string (name and value) to a resources
 *                database.
 *
 *  Parameters:   rDB          X resources database.
 *
 *                appname      application name.
 *
 *                rstr         X resource string name.
 *
 *                rval         X resource string value.
 *
 *  Returns:      None.
 *
 *  Note:         The first character of the appname and resource strings may
 *                be modified.
 */

void
ds_put_resource(rDB, appname, rstr, rval)
XrmDatabase *rDB ;
char *appname, *rstr, *rval ;
{
  char resource[DS_MAX_STR] ;

  if (isupper(appname[0])) appname[0] = tolower(appname[0]) ;
  SPRINTF(resource, "deskset.%s.%s", appname, rstr) ;
  XrmPutStringResource(rDB, resource, rval) ;
}


/*  Function:     ds_save_cmdline()
 *
 *  Purpose:      Save away the application command line options.
 *
 *  Parameters:   frame        Applications main XView base frame.
 *
 *                argc         Number of command line options.
 *
 *                argv         An array of command line options.
 *
 *  Returns:      XV_ERROR   if unable to save command line.
 *                XV_OK      on successful completion.
 */

Xv_opaque
ds_save_cmdline(frame, argc, argv)
Frame frame ;
int argc ;
char *argv[] ;
{
  return(xv_set(frame, FRAME_WM_COMMAND_ARGC_ARGV, argc, argv, 0)) ;
}


/*  Function:     ds_save_resources()
 *
 *  Purpose:      Save away the resources database to the file given by the
 *                DESKSETDEFAULTS environment variable (if set), or
 *                to $HOME/.desksetdefaults.
 *
 *  Parameters:   rDB        X resources database to save.
 *
 *  Returns:      XV_ERROR   if cannot access resource database to write.
 *                XV_OK      on successful completion.
 */


Xv_opaque
ds_save_resources(rDB)
XrmDatabase rDB ;
{
	char *home, *filename;
	struct  stat    statbuf;

	if ((filename = getenv("DESKSETDEFAULTS")) == NULL) {
		home = getenv("HOME");
		filename = (char*)calloc(1, strlen(home)+18);
      		sprintf(filename, "%s/.desksetdefaults", home);
	}
	/* if file exists but user does not have access*/
	if (stat(filename, &statbuf) != -1 && access(filename, W_OK) != 0) {
		free(filename);
		return(XV_ERROR);
	}
	/* if file does not exist this call will create it*/
	XrmPutFileDatabase(rDB, filename);
	free(filename);
  	return(XV_OK);
}


/*  Function:     ds_set_frame_size()
 *
 *  Purpose:      Sets the bounding box of a window.
 *
 *  Parameters:   window     XView handle to the window.
 *
 *                x          left most value.
 *                y          top most value.
 *                width      width of bounding box.
 *                height     height of bounding box.
 *
 *  Returns:      XV_ERROR   if the operation was successful.
 *                XV_OK      on successful completion.
 */

Xv_opaque
ds_set_frame_size(window, x, y, width, height)
Xv_Window window ;
int x, y, width, height ;
{
  Rect r ;

  r.r_left   = x ;
  r.r_top    = y ;
  r.r_width  = width ;
  r.r_height = height ;
  return(xv_set(window, WIN_RECT, &r, 0)) ;
}
