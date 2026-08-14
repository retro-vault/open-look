/*
 * Declares the olwmslave interfaces used by the olwmslave application.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

extern	Display		*display;	/* X handle on display server */
extern  Xv_Server	dpyServer;	/* XView handle on display server */

typedef struct _ScreenRoot {
	Bool		used;		/* whether we're using this screen */
	Xv_Screen	screen;		/* XView handle on screen */
	Xv_Window	root;		/* XView handle on root window */
	Window		rootwin;	/* X handle on root window */
	char		*dpyEnvStr;	/* DISPLAY env string for this screen */
} ScreenRoot;

typedef struct _ScreenInfo {
	int		numScreens;	/* Number of screens for a display */
	ScreenRoot	*screenList;	/* List of ScreenRoot's - one for each 
					 * screen that the display handles */
} ScreenInfo;

extern	ScreenInfo	screenInfo;

/*----------------------------------------------------------------------
 *	ScreenInfo convenience macros
 *----------------------------------------------------------------------*/
#define NumScreens()		(screenInfo.numScreens)
#define ScreenList(nscr)	(&(screenInfo.screenList[(nscr)]))
#define ScreenUsed(nscr)	(screenInfo.screenList[(nscr)].used)
#define XVScreen(nscr)		(screenInfo.screenList[(nscr)].screen)
#define XVRoot(nscr)		(screenInfo.screenList[(nscr)].root)
#define RootWin(nscr)		(screenInfo.screenList[(nscr)].rootwin)
#define DpyEnvString(nscr)	(screenInfo.screenList[(nscr)].dpyEnvStr)

#ifdef OW_I18N_L3
#include <locale.h>
#include <libintl.h>
#define LOCALIZE(msg)		gettext(msg)
#else
#define LOCALIZE(msg)		(msg)
#endif
