#ifndef lint
static 	char sccsid[] = "@(#)ds_colors.c	3.2 01/21/93 Copyright 1990 Sun Microsystems, Inc";
#endif

/*
 * Copyright (c) 1990 Sun Microsystems, Inc.
 */

#include <ctype.h>
#include <values.h>      /* MAXLONG */

#include <xview/xview.h>
#include <xview/defaults.h>
#include <xview/cms.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "ds_colors.h"

#define STANDARD_PALETTE_SIZE      (8 * 9)
#define WORKSPACE_CLASS_RES        "OpenWindows.WorkspaceColor"
#define WORKSPACE_RES              "openwindows.workspacecolor"
#define WINDOW_CLASS_RES           "OpenWindows.WindowColor"
#define WINDOW_RES                 "openwindows.windowcolor"
#define BACK_CLASS_RES             "Window.Color.Background"
#define BACK_RES                   "window.color.background"
#define FORE_CLASS_RES             "Window.Color.Foreground"
#define FORE_RES                   "window.color.foreground"

static int             screen;
static Colormap        cmap;
static int             ncolors = 0;
static unsigned long  *xcolors;
static char            prop_data[4096];
static Xv_singlecolor *fg_color, *bg_color;
/*
 * Initialize array to a set of canned colors
 * in case the color property is not set in the server.
 */
static Xv_singlecolor  scolors[ MAX_DS_COLORS ] = {
    {   0,   0,   0 },
    {   0,   0,   0 },
    {  76,  76,  76 },
    { 102, 102, 102 },
    { 153, 153, 153 },
    { 178, 178, 178 },
    { 204, 204, 204 },
    { 255, 255, 255 },
    { 191, 152, 152 },
    { 191, 114, 114 },
    { 191,  76,  76 },
    { 191,  38,  38 },
    { 229, 183, 183 },
    { 229, 137, 137 },
    { 229,  91,  91 },
    { 229,  45,  45 },
    { 191, 180, 152 },
    { 191, 170, 114 },
    { 191, 158,  76 },
    { 191, 148,  38 },
    { 229, 216, 183 },
    { 229, 203, 137 },
    { 229, 190,  91 },
    { 229, 177,  45 },
    { 189, 191, 152 },
    { 187, 191, 114 },
    { 185, 191,  76 },
    { 183, 191,  38 },
    { 227, 229, 183 },
    { 224, 229, 137 },
    { 222, 229,  91 },
    { 220, 229,  45 },
    { 152, 191, 162 },
    { 114, 191, 134 },
    {  76, 191, 105 },
    { 38,  191,  76 },
    { 183, 229, 194 },
    { 137, 229, 160 },
    {  91, 229, 126 },
    {  45, 229,  91 },
    { 152, 191, 191 },
    { 114, 191, 191 },
    {  76, 191, 191 },
    {  38, 191, 191 },
    { 183, 229, 229 },
    { 137, 229, 229 },
    {  91, 229, 229 },
    {  45, 229, 229 },
    { 152, 162, 191 },
    { 114, 134, 191 },
    {  76, 105, 191 },
    {  38,  76, 191 },
    { 183, 194, 229 },
    { 137, 160, 229 },
    {  91, 126, 229 },
    {  45,  91, 229 },
    { 178, 152, 191 },
    { 165, 114, 191 },
    { 152,  76, 191 },
    { 140,  38, 191 },
    { 213, 183, 229 },
    { 198, 137, 229 },
    { 183,  91, 229 },
    { 167,  45, 229 },
    { 191, 152, 182 },
    { 191, 114, 172 },
    { 191,  76, 162 },
    { 191,  38, 152 },
    { 229, 183, 218 },
    { 229, 137, 206 },
    { 229,  91, 194 },
    { 229,  45, 183 },
};


/*
 * Read the Property Data from the server.
 * Returns 0 on failure, 1 on success.
 */
static int
read_prop_data( dpy )
    Display  *dpy;
{
    Atom           atom;
    Atom           actual_atom;
    int            status;
    int            format;
    unsigned long  bytes_read = 0;
    unsigned long  bytes_left;
    unsigned char *ptr;

    atom = XInternAtom( dpy, COLOR_PROP_NAME, True );
    if ( atom == None ) {
      /* give user warning if deskset color property atom not found */
#ifdef DEBUG
      fprintf(stderr, "read_prop_data: %s server property not set\n",
              COLOR_PROP_NAME );
#endif /* DEBUG */
      return( 0 );
    }

    prop_data[0] = 0;

    status = XGetWindowProperty( dpy, RootWindow( dpy, screen ), atom, 0, 4096 / 4 + 1,
				 False, AnyPropertyType, &actual_atom, &format,
                                 &bytes_read, &bytes_left, &ptr );
    if ( ( status != Success ) || ( bytes_read == 0 ) ) {
      fprintf( stderr, "ds_colors.read_prop_data:  Error reading property data.\n" );
      return( 0 );
    }

    strncat( prop_data, ( char * )ptr, bytes_read );
    XFree( ( caddr_t )ptr  );
    return( 1 );

}

/*
 * The Property Data in the server should be in a string format
 * "CMAP_VALUE HEX_COLOR HEX_COLOR HEX_COLOR ..."
 * where CMAP_VALUE is the Colormap and
 *       HEX_COLOR  is in the hex format #RGB, #RRGGBB, #RRRGGGBBB, or #RRRRGGGGBBBB.
 */
static int
parse_prop_data( dpy )
    Display *dpy;
{
    char   *start_ptr,
           *end_ptr;
    int    red, green, blue;
    XColor xcolor;

    start_ptr = prop_data;
    end_ptr   = start_ptr;

    /*
     * Parse the Colormap value.
     */
    while ( isdigit ( *end_ptr ) )
      end_ptr++;
    if ( *end_ptr == ' ' )
      *end_ptr = 0;
    else
      return( 0 );
    ( void )sscanf( start_ptr, "%lu", &cmap );

    /*
     * Read colors and convert the hex values into RGB colors.
     * Hex format can be in any of the following formats:
     * 1.  #RGB          (4  bits each of red, green, and blue)
     * 2.  #RRGGBB       (8  bits each of red, green, and blue)
     * 3.  #RRRGGGBBB    (12 bits each of red, green, and blue )
     * 4.  #RRRRGGGGBBBB (16 bits each of red, green, and blue )
     */
    start_ptr = ++end_ptr;
    while ( *start_ptr == '#' ) {

      while ( *end_ptr && *end_ptr != ' ' )
        end_ptr++;
      *end_ptr = 0;

      if ( !XParseColor( dpy, cmap, start_ptr, &xcolor ) )
        fprintf( stderr, "Could not look up color %s\n", start_ptr );

      scolors[ncolors].red   = xcolor.red >> 8;
      scolors[ncolors].green = xcolor.green >> 8;
      scolors[ncolors].blue  = xcolor.blue >> 8;

      ncolors++;

      start_ptr = ++end_ptr;

    }

    return( 1 );
}


/*
 * Since the property was not set, recover by trying to
 * allocate the canned colors.
 */
static int
parse_canned_colors( dpy )
    Display *dpy;
{
  XColor          color_cell;
  unsigned long   pixel;
  char            hex_color[24];

  for ( pixel = 0; pixel < STANDARD_PALETTE_SIZE; pixel++ ) {

    sprintf( hex_color, "#%02x%02x%02x", scolors[pixel].red, scolors[pixel].green, scolors[pixel].blue );
    if ( !XParseColor( dpy, cmap, hex_color, &color_cell ) )
       break;
    scolors[pixel].red   = color_cell.red >> 8;
    scolors[pixel].green = color_cell.green >> 8;
    scolors[pixel].blue  = color_cell.blue >> 8;
    ncolors++;
  }
  return( 1 );

}

static void
init_xrdb()
{
  char user_defaults[ 256 ];

  sprintf( user_defaults, "%s/.Xdefaults", getenv( "HOME" ) );

  /*
   * Initialize the resource manager.
   * Load the user's own resource database.
   */
  defaults_load_db( user_defaults );

}

/*
 * string_to_xcolor()
 *   string may be:
 *     color name:   "Blue", "Pink", etc.
 *     hex color:   #a0bf1e
 *     sunview decimal color:   100 220 255
 *
 * if it is not one of these formats (or if the name cannot be found),
 * then string_to_xcolor() returns 1, otherwise it returns 0.
 */
static int
string_to_xcolor(dpy, s, xcolor)
    Display    *dpy;
    char       *s;
    XColor     *xcolor;
{
    if (!XParseColor(dpy, cmap, s, xcolor)) {
	int         red,
	            green,
	            blue;
	if (sscanf(s, "%d %d %d", &red, &green, &blue) == 3) {
	    xcolor->red = red << 8;
	    xcolor->green = green << 8;
	    xcolor->blue = blue << 8;
	    xcolor->flags = DoRed | DoGreen | DoBlue;
            return ( 1 );
	}
        else
          return( 0 );
    }
    else
      return( 1 );
}

static int
process_resource_color( dpy, color )
     Display  *dpy;
     char     *color;
{
  int     status;
  char    hex_color[ 24 ];
  XColor  xcolor;

  /*
   * Convert the string into an XColor
   * Convert to an Xv_singlecolor to store in array.
   * Add to scolors array.
   */

  status = string_to_xcolor( dpy, color, &xcolor );
  if ( status ) {
    scolors[ ncolors ].red   = xcolor.red >> 8;
    scolors[ ncolors ].green = xcolor.green >> 8;
    scolors[ ncolors ].blue  = xcolor.blue >> 8;

#ifdef DEBUG
    fprintf(stderr,"process_resource_color: coverting color[%d] \"%s\" to %d %d %d\n",
      ncolors, color,
      scolors[ ncolors ].red,
      scolors[ ncolors ].green,
      scolors[ ncolors ].blue) ;
#endif /* DEBUG */

    ncolors++;
  }

}


/*
 *
 */
static void
read_resources( dpy, window )
     Display  *dpy;
     Xv_window window;
{
  char  *s, status;

  s = defaults_get_string( WORKSPACE_RES, WORKSPACE_CLASS_RES, NULL );
  if ( s )
    status = process_resource_color ( dpy, s );
  s = defaults_get_string( WINDOW_RES, WINDOW_CLASS_RES, NULL );
  if ( s )
    status = process_resource_color( dpy, s );

  s = defaults_get_string( BACK_RES, BACK_CLASS_RES, "White" );
  if ( s )
    status = process_resource_color( dpy, s );
  s = defaults_get_string( FORE_RES, FORE_CLASS_RES, "Black" );
  if ( s )
    status = process_resource_color( dpy, s );

}


/****************************************************************************
 *
 *  Function:        ds_colors_init()
 *
 *  Purpose:         Initializes the colors of the DeskSet Color Property
 *                   to be read into memory to do a color match on.
 *
 *  Parameters:      dpy         *Display
 *
 *  Returns:         1           Success
 *		     0           Failure
 *
 ****************************************************************************/

int
ds_colors_init( dpy, window )
    Display   *dpy;
    Xv_window  window;
{
    int  status;

    cmap = xv_get( xv_get( window, WIN_CMS ), CMS_CMAP_ID );

    /*
     * Read the color property data from the server.
     * If the data is there, parse the colors in the colormap.
     * else just used the canned set of colors
     * preset in the scolors array plus any X Resource colors
     * since these are already allocated.
     */
    bg_color = ( Xv_singlecolor * )xv_get( window, FRAME_BACKGROUND_COLOR, NULL );
    fg_color = ( Xv_singlecolor * )xv_get( window, FRAME_FOREGROUND_COLOR, NULL );
    status = read_prop_data( dpy );
    if ( status ) {
      status = parse_prop_data( dpy );
      return( status );
    }
    else {
#ifdef DEBUG
      fprintf( stderr, "ds_colors_init: using canned colors\n") ;
#endif /* DEBUG */

      ncolors = STANDARD_PALETTE_SIZE;
      init_xrdb();
      read_resources( dpy, window );
      return ( 1 );
    }



}


/****************************************************************************
 *
 *  Function:        ds_cms_create()
 *
 *  Purpose:         Creates an XView Colormap segment to be used in an
 *                   application where the DeskSet colors can be shared.
 *
 *  Parameters:      window      Xv_window
 *			         The foreground and background color should
 *			         be set.
 *
 *  Returns:         Cms         XV_NULL if unsuccessful
 *                               Valid Cms if successful
 *
 ****************************************************************************/

Cms
ds_cms_create( window )
    Xv_window  window;
{
  Cms       cms = XV_NULL;
  int       status;
  Display  *dpy;

  if ( !window )
    return ( XV_NULL );

  if ( ( int )xv_get( window, WIN_DEPTH ) <= 1 )
    return( XV_NULL );

  /*
   * Read the colors into the array of scolors
   * to create the xview colormap segment.
   */
  dpy    = ( Display * )xv_get( window, XV_DISPLAY );
  status = ds_colors_init( dpy, window );
  if ( !status )
    return( XV_NULL );

  /*
   * Create the Read-Only, Shareable Colormap Segment.
   */
  cms = xv_create( NULL, CMS,
                   CMS_TYPE, XV_STATIC_CMS,
                   CMS_CONTROL_CMS, TRUE,
                   CMS_SIZE, CMS_CONTROL_COLORS + ncolors + 2,
                   NULL );

  if ( cms ) {
    xv_set( window, WIN_CMS, cms, XV_NULL );
    xcolors = ( unsigned long * )xv_get( cms, CMS_INDEX_TABLE );
  }

  return ( cms );

}


/****************************************************************************
 *
 *  Function:        ds_alloc_colors()
 *
 *  Purpose:         Forces all colors set in the property to be allocated.
 *
 *  Parameters:      window   Xv_window
 *
 *  Returns:
 *
 ****************************************************************************/

int
ds_alloc_colors( window )
    Xv_window  window;
{
    Display  *dpy;
    Cms       cms;
    int       i, index, maxcolors;
    int       status;

    cms = ( Cms )xv_get( window, WIN_CMS );
    maxcolors = ncolors;

    /*
     * Try to allocate all the colors first.
     */
    status = xv_set( cms,
		    CMS_COLOR_COUNT, maxcolors,
		    CMS_INDEX, CMS_CONTROL_COLORS,
		    CMS_COLORS, scolors,
		    NULL );
    /*
     * If unable to allocate all the colors, then allocate as many as you
     * can one at a time and add from the list then add the
     * defaults resources  and allocate them.
     */
    if ( status == XV_ERROR ) {
      ncolors = 0;
      for ( i = 0; i < maxcolors; i++ ) {
        status = xv_set( cms,
		         CMS_COLOR_COUNT, 1,
		         CMS_INDEX, CMS_CONTROL_COLORS + ncolors,
		         CMS_COLORS, &scolors[i],
		         NULL );
        if ( status != XV_ERROR )
          ncolors++;
        else
	  break;
      }

      index = ncolors;
      dpy = ( Display * )xv_get( window, XV_DISPLAY );
      read_resources( dpy, window );
      for ( i = index; i < ncolors; i++ ) {
        status = xv_set( cms,
		         CMS_COLOR_COUNT, 1,
		         CMS_INDEX, CMS_CONTROL_COLORS + i,
		         CMS_COLORS, &scolors[i],
		         NULL );
        if ( status == XV_ERROR )
          fprintf( stderr, "ds_alloc_colors: Couldn't allocate a resource color!\n" );
      }

    }

    xcolors = ( unsigned long * )xv_get( cms, CMS_INDEX_TABLE );
    return( ncolors  );

}

/*
 * Finds the closest match into the list of DeskSet colors.
 * Based on MITR4 X11 best fit matching algorithm.
 */
/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

static unsigned long
best_fit_index( color )
  Xv_singlecolor *color;
{
    unsigned long dr, dg, db;
    unsigned long final, minval;
    unsigned long sum, diff;
    int pixel;

    final  = 0;
    minval = ~0;

    for ( pixel = 0; pixel < ncolors; pixel++ ) {
      dr = dg = db = 0;
      dr = ( int )( scolors[pixel].red - color->red );
      dg = ( int )( scolors[pixel].green - color->green );
      db = ( int )( scolors[pixel].blue - color->blue );
      diff = dr * dr;
      sum = diff + dg * dg;
      if ( sum < diff )
        continue;
      diff = sum + db * db;
      if ( ( diff >= sum ) && ( diff < minval ) ) {
        final = pixel;
        minval = diff;
      }
    }

    return ( final );

}


static unsigned long
recover_index( dpy, xcolor )
  Display  *dpy;
  XColor   *xcolor;
{
    unsigned long dr, dg, db;
    unsigned long final, minval;
    unsigned long sum, diff;
    int pixel;
    unsigned long ncells;
    XColor   qcolor;

    final  = 0;
    minval = ~0;

    ncells = DisplayCells( dpy, DefaultScreen( dpy ) );

    for ( pixel = 0; pixel < ncells; pixel++ ) {
      dr = dg = db = 0;
      qcolor.pixel = pixel;
      XQueryColor( dpy, cmap, &qcolor );
      dr = ( int )( qcolor.red   - xcolor->red );
      dg = ( int )( qcolor.green - xcolor->green );
      db = ( int )( qcolor.blue  - xcolor->blue );
      diff = dr * dr;
      sum = diff + dg * dg;
      if ( sum < diff )
        continue;
      diff = sum + db * db;
      if ( ( diff >= sum ) && ( diff < minval ) ) {
        final = pixel;
        minval = diff;
      }
    }

    return ( final );

}

/****************************************************************************
 *
 *  Function:        ds_cms_index()
 *
 *  Purpose:         Return the CMS_INDEX for the cms created with ds_cms_create
 *                   for the given color.  The index will be the "best fit" into
 *                   the DeskSet colors from cms.
 *
 *  Parameters:      cms            Cms                cms created with ds_cms_create()
 *                   color          Xv_singlecolor     ( R, G, and B values )
 *
 *  Returns:         CMS_INDEX which is the index into the Colormap segment.
 *
 ****************************************************************************/

unsigned long
ds_cms_index( cms, color )
    Cms             cms;
    Xv_singlecolor *color;
{
    unsigned long  closest;
    int            status;

    /*
     * Return the best fit RGB values that will
     * actually be allocated.
     */
    closest = best_fit_index( color );

    /*
     * Set this color in the colormap segment.
     * I think this allocates only if needed.
     */

    status = xv_set( cms, CMS_COLOR_COUNT, 1,
                 CMS_INDEX, CMS_CONTROL_COLORS + closest,
	         CMS_COLORS, &scolors[closest],
                 NULL );
    if ( status == XV_ERROR ) {
#ifdef DEBUG
      fprintf( stderr, "ds_cms_index: Could not allocate new color.\n" );
#endif
return( 0 );
    }
    /*
     * Update the x color indicies for the new color in the Cms.
     */
    xcolors = ( unsigned long * )xv_get( cms, CMS_INDEX_TABLE );

    return( CMS_CONTROL_COLORS + closest );
}


/****************************************************************************
 *
 *  Function:        ds_x_index()
 *
 *  Purpose:         Returns the index into the hardware colormap used for
 *                   setting the foreground and background colors in GC's
 *                   used in Xlib calls.
 *
 *  Parameters:      cms_index      logical index into cms - value returned from ds_cms_index
 *
 *  Returns:         Index into colormap segment.
 *
 ****************************************************************************/

unsigned long
ds_x_index( cms_index )
    unsigned long   cms_index;
{
    if ( cms_index < ( CMS_CONTROL_COLORS + ncolors  + 2 ) )
      return( xcolors[ cms_index ] );
    else
      return( 0 );
}


/****************************************************************************
 *
 *  Function:        ds_x_pixel()
 *
 *  Purpose:         Returns the pixel value into the hardware colormap used for
 *                   setting the foreground and background colors in GC's
 *                   used in Xlib calls. This call does not require a cms from
 *                   XView.
 *
 *  Parameters:      Display     *dpy
 *                   Colormap    cmap
 *                   XColor      xcolor
 *
 *  Returns:         pixel value into DefaultColormap
 *
 ****************************************************************************/

extern unsigned long
ds_x_pixel( dpy, cmap, xcolor )
    Display   *dpy;
    Colormap  cmap;
    XColor    *xcolor;
{
    Xv_singlecolor   scolor;
    unsigned long    closest;
    XColor           actual_color;

    /*
     * Return the best fit RGB values that will
     * actually be allocated.
     */
    scolor.red   = xcolor->red >> 8;
    scolor.green = xcolor->green >> 8;
    scolor.blue  = xcolor->blue >> 8;
    closest = best_fit_index( &scolor );

    actual_color.red   = ( unsigned long )scolors[closest].red << 8;
    actual_color.green = ( unsigned long )scolors[closest].green << 8;
    actual_color.blue  = ( unsigned long )scolors[closest].blue << 8;
    actual_color.flags = DoRed | DoGreen | DoBlue;

    if ( !XAllocColor( dpy, cmap, &actual_color ) ) {
#ifdef DEBUG
      fprintf( stderr, "ds_x_pixel: Cannot allocated color." );
#endif
return( recover_index( dpy, &actual_color ) );
    }

    return( actual_color.pixel );


}

/****************************************************************************
 *
 *  Function:        ds_set_colormap()
 *
 *  Purpose:         Sets a cms on a windo with foreground and background values.
 *
 *  Parameters:      win         window (panel, canvas, ttysw, etc.) to set the
 *                               cms on
 *                   cms         Cms created from ds_cms_create()
 *                   fg          foreground index
 *                   bg          background index
 *
 *  Notes:           If fg, bg are DS_NULL_CMS_INDEX then use the default
 *                   fg and bg color from the resource database.
 *
 *  Returns:         1           Success
 *		     0           Failure
 *
 ****************************************************************************/

int
ds_set_colormap( win, cms, fg, bg )
    Window         win;
    Cms            cms;
    unsigned long  fg;
    unsigned long  bg;
{
    int  status;

    /*
     * Check if we should use the default foreground/background.
     */
    if ( fg == DS_NULL_CMS_INDEX )
      fg = ncolors + 1 ;

    if ( bg == DS_NULL_CMS_INDEX )
      bg = ncolors ;


    /*
     * Ensure the foreground and background colors are
     * allocated before setting them.
     */
    scolors[fg].red = fg_color->red;
    scolors[fg].green = fg_color->green;
    scolors[fg].blue = fg_color->blue;
    status = xv_set( cms,
	             CMS_COLOR_COUNT, 1,
	             CMS_INDEX, CMS_CONTROL_COLORS + fg,
	             CMS_COLORS, &scolors[fg],
	             NULL );
    if ( status == XV_ERROR ) {
      fprintf( stderr,
               "ds_set_colormap: Couldn't allocate foreground resource color!\n" );
      return( 0 );
    }

    scolors[bg].red = bg_color->red;
    scolors[bg].green = bg_color->green;
    scolors[bg].blue = bg_color->blue;
    status = xv_set( cms,
	             CMS_COLOR_COUNT, 1,
	             CMS_INDEX, CMS_CONTROL_COLORS + bg,
	             CMS_COLORS, &scolors[bg],
	             NULL );
    if ( status == XV_ERROR ) {
      fprintf( stderr,
               "ds_set_colormap: Couldn't allocate background resource color!\n" );
      return( 0 );
    }

    /*
     * Update the x color indicies for the new color in the Cms.
     */
    xcolors = ( unsigned long * )xv_get( cms, CMS_INDEX_TABLE );

    /*
     * Set the colors onto the window.
     */

#ifdef DEBUG
    fprintf(stderr, "ds_set_colormap: fg=%3d,%3d,%3d  cms_i=%d  xpixel_i=%d\n",
      scolors[fg].red, scolors[fg].green, scolors[fg].blue,
      fg, xcolors[CMS_CONTROL_COLORS+fg]) ;
    fprintf(stderr, "                 bg=%3d,%3d,%3d  cms_i=%d  xpixel_i=%d\n",
      scolors[bg].red, scolors[bg].green, scolors[bg].blue,
      bg, xcolors[CMS_CONTROL_COLORS+bg]) ;
#endif /* DEBUG */

    xv_set( win,
	    WIN_CMS, cms,
	    WIN_FOREGROUND_COLOR, CMS_CONTROL_COLORS + fg,
	    WIN_BACKGROUND_COLOR, CMS_CONTROL_COLORS + bg,
	    NULL );

    return( 1 );
}





