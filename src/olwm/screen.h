/*
 * screen.h: declarations and shared types for the screen module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef _OLWM_SCREEN_H
#define _OLWM_SCREEN_H

#include <olgx/olgx.h>
#include "olwm.h"

/*
 *	Index's into GC array of ScreenInfo struct
 */
typedef enum {	
	ROOT_GC,
	FOREGROUND_GC,
	BORDER_GC,
	WINDOW_GC,
	WORKSPACE_GC,
	BUSY_GC,
	ICON_NORMAL_GC,
	ICON_MASK_GC,
	ICON_BORDER_GC,
	NUM_GCS 
} ScreenGCIndex;

/*
 *	Index's into Graphics_info array of ScreenInfo struct
 */
typedef enum { 
	NORMAL_GINFO, 
	BUTTON_GINFO, 
	TEXT_GINFO,
	REVPIN_GINFO,
	NUM_GINFOS 
} ScreenGinfoIndex;

/*
 *	Index's into Pixmap array of ScreenInfo struct
 */
typedef enum {
	BUSY_STIPPLE,
	ICON_BITMAP,
	ICON_MASK,
	PROTO_DRAWABLE,
	GRAY50_BITMAP,
	NUM_PIXMAPS
} ScreenPixmapIndex;

/*
 * 	ColorMapFocus	- client/window which has colormap focus
 */
typedef struct _colormapfocus {
	struct _client		*client;
	struct _wingeneric	*window;
	Bool			locked;
} ColorMapFocus;

/*
 *	ColorInfo	- window/workspace/etc colors
 */
typedef struct _colorinfo {
	unsigned long		flags;
	Bool			reverseVideo;
	unsigned long		black, white;
	unsigned long		fgColor, bgColor;
	unsigned long		bg0Color,bg1Color,bg2Color,bg3Color;
	unsigned long		borderColor;
	WorkspaceStyle		workspaceStyle;
	unsigned long		workspaceColor;
	unsigned long		workspaceRootPixel;
	unsigned long		workspaceBitmapFg;
	unsigned long		workspaceBitmapBg;
} ColorInfo;

#define CIWorkspaceColorAlloced		(1L<<0)
#define CIWorkspaceBitmapColorsAlloced	(1L<<1)
#define CIWindowColorAlloced		(1L<<2)
#define CIForegroundColorAlloced	(1L<<3)
#define CIBackgroundColorAlloced	(1L<<4)
#define CIBorderColorAlloced		(1L<<5)

/*
 *	ScreenInfo	- Per screen info
 */
typedef struct _screeninfo {
	Display			*dpy;
	int			screen;
	Window			rootid;
	struct _winroot		*rootwin;
	int			depth;
	Visual			*visual;
	Colormap		colormap;
	Bool			iscolor;
	Bool			use3D;
	ColorInfo		colorInfo;
	GC			gc[NUM_GCS];
	Graphics_info		*gi[NUM_GINFOS];
	Pixmap			pixmap[NUM_PIXMAPS];
	struct _menuCache	*menuCache;
	ColorMapFocus		cmapfocus;
	int			framepos;
	struct _iconGrid	*iconGrid;
	char			**environment;
	int			instanceQ;	/* quark for this screen's
						   instance name */
	int			dfltIconWidth, dfltIconHeight;
#ifdef ALLPLANES
	Bool			useAllPlanes;
#endif
	Window			*winCache;	/* array of window IDs */
	int			winCacheSize;	/* size of the array */
	int			winCacheCount;	/* first free element */
} ScreenInfo;

/*
 *	Global functions from screen.c
 */
/* InitScreens: Initialize screens. */
extern	void		InitScreens(Display *dpy);
/* DestroyScreens: Destroy screens. */
extern	void		DestroyScreens(Display *dpy);
/* GetFirstScrInfo: Return first scr info. */
extern	ScreenInfo	*GetFirstScrInfo(void);
/* GetScrInfoOfScreen: Return scr info of screen. */
extern	ScreenInfo	*GetScrInfoOfScreen(int screen);
/* GetScrInfoOfRoot: Return scr info of root. */
extern	ScreenInfo	*GetScrInfoOfRoot(Window root);
/* SetWorkspaceBackground: Set workspace background. */
extern	void		SetWorkspaceBackground(Display *dpy);
/* SetWindowColor: Set window color. */
extern	void		SetWindowColor(Display *dpy);
/* SetForegroundColor: Set foreground color. */
extern	void		SetForegroundColor(Display *dpy);
/* SetBackgroundColor: Set background color. */
extern	void		SetBackgroundColor(Display *dpy);
/* SetBorderColor: Set border color. */
extern	void		SetBorderColor(Display *dpy);
/* SetTitleFont: Set title font. */
extern	void		SetTitleFont(Display *dpy);
/* SetTextFont: Set text font. */
extern	void		SetTextFont(Display *dpy);
/* SetButtonFont: Set button font. */
extern	void		SetButtonFont(Display *dpy);
/* SetIconFont: Set icon font. */
extern	void		SetIconFont(Display *dpy);
/* SetGlyphFont: Set glyph font. */
extern	void		SetGlyphFont(Display *dpy);
/* SetIconLocation: Set icon location. */
extern	void		SetIconLocation(Display *dpy);
/* ScreenCreateWindow: Create screen window. */
extern	Window		ScreenCreateWindow(ScreenInfo*, Window, 
					   int, int, int, int, unsigned long, 
					   XSetWindowAttributes*);
/* ScreenDestroyWindow: Destroy screen window. */
extern	void		ScreenDestroyWindow(ScreenInfo*, Window);
/* ScreenUpdateWinCacheSize: Update screen win cache size. */
extern	void		ScreenUpdateWinCacheSize(Display *dpy);
/* ReparentScreens: Reparent screens. */
extern	void		ReparentScreens(Display *dpy);

#endif	/* _OLWM_SCREEN_H */
