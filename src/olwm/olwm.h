/*
 * olwm.h: declarations and shared types for the olwm module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef _OLWM_OLWM_H
#define _OLWM_OLWM_H

#ifndef ABS
#define ABS(a)		(((a) < 0) ? -(a) : (a))
#endif

#ifndef MAX
#define	MAX(a,b)	(((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)        ((a) < (b) ? (a) : (b))
#endif

/*
 * path and file name lengths -- if not defined already
 */
#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif
#ifndef MAXNAMELEN
#define MAXNAMELEN 256
#endif

/* Determine the size of an object type in 32bit multiples.
 * Rounds up to make sure the result is large enough to hold the object. */
#define LONG_LENGTH(a)	((long)(( sizeof(a) + 3 ) / 4))

#define	FOOTLEN	50L

/* protocols bits */
#define		TAKE_FOCUS		(1<<0)
#define		SAVE_YOURSELF		(1<<1)
#define		DELETE_WINDOW		(1<<2)

/* Workspace Background Styles */
typedef enum { WkspDefault, WkspColor, WkspPixmap } WorkspaceStyle;

/* Icon positioning modes */
typedef enum { AlongTop, AlongBottom, AlongRight, AlongLeft,
	       AlongTopRL, AlongBottomRL, AlongRightBT, AlongLeftBT
	     } IconPreference;

/* size of icon window */
#define ICON_WIN_WIDTH 60
#define ICON_WIN_HEIGHT 60
#define ICON_GRID_WIDTH 13
#define ICON_GRID_HEIGHT 13

/* min/max/inc icon sizes */
#define ICON_MIN_WIDTH 		1
#define ICON_MIN_HEIGHT 	1
#define ICON_MAX_WIDTH		160
#define ICON_MAX_HEIGHT		160
#define ICON_WIDTH_INC		1
#define ICON_HEIGHT_INC		1

/* minimum window size */
#define MINSIZE 5

/* Globals */
extern	char   *ProgramName;

/* adornment pixmaps */
extern	Pixmap	pixIcon;
extern	Pixmap	pixmapGray;
extern	Pixmap	pixGray;

/* miscellaneous functions */
/* ExitOLWM: Process exit olwm. */
extern int ExitOLWM();
Bool ExitRequested(void);
int ExitSignalFD(void);
void DrainExitSignal(void);
void Exit(Display *dpy);
void ReapChildren(void);
void RecursiveRefresh(Display *dpy, Window win);
void InitAtoms(Display *dpy);
void WIInit(Display *dpy);
pid_t SlaveStart(char **argv);
void SlaveStop(void);
void SlaveStopped(void);
int olwm_usleep(unsigned int usec);

#ifdef ALLPLANES
Bool XAllPlanesQueryExtension(Display *dpy, int *event_base,
                              int *error_base);
void XAllPlanesFillRectangles(Display *dpy, Drawable d,
                              XRectangle *rects, int nrects);
void XAllPlanesDrawSegments(Display *dpy, Drawable d,
                            XSegment *segments, int nsegments);
#endif
/* GetWindowProperty: Return window property. */
extern void *GetWindowProperty(Display *dpy, Window w, Atom property, long long_offset, long long_length, Atom req_type, int req_fmt, unsigned long *nitems, unsigned long *bytes_after);
#ifdef OW_I18N_L4
/* parseApplicationLocaleDefaults: Parse application locale defaults. */
extern void parseApplicationLocaleDefaults();
#endif

/* state functions */
/* StateNew: Process state new. */
extern struct _client *StateNew();
/* ReparentTree: Reparent tree. */
extern void ReparentTree();
/* StateNormIcon: Process state norm icon. */
extern void StateNormIcon();
/* StateIconNorm: Process state icon norm. */
extern void StateIconNorm();
/* StateWithdrawn: Process state withdrawn. */
extern void StateWithdrawn();

/* root window functions */
/* MakeRoot: Create root. */
extern struct _winroot *MakeRoot();

/* no-focus window information and functions */
extern Window NoFocusWin;
extern struct _wingeneric *NoFocusWinInfo;

/* MakeNoFocus: Create no focus. */
extern struct _wingeneric *MakeNoFocus();
/* NoFocusTakeFocus: Focus no take focus. */
extern void NoFocusTakeFocus();
/* NoFocusInit: Focus no init. */
extern void NoFocusInit();
/* NoFocusEventBeep: Focus no event beep. */
extern int NoFocusEventBeep();

/* client information and functions */
extern struct _List *ActiveClientList;

/* ClientCreate: Create client. */
extern struct _client *ClientCreate();
/* ClientPane: Process client pane. */
extern Window ClientPane();
typedef struct _clientinboxclose {
	Display *dpy;
	int 	screen;
	int 	(*func)();
	short 	bx, by, bw, bh;
	Time 	timestamp;
} ClientInBoxClosure;
/* ClientInBox: Process client in box. */
extern void *ClientInBox();

/* frame functions */
/* MakeFrame: Create frame. */
extern struct _winpaneframe *MakeFrame();
/* FrameSetPosFromPane: Set frame pos from pane. */
extern void FrameSetPosFromPane();
/* FrameFullSize: Full frame size. */
extern void FrameFullSize();
/* FrameNormSize: Process frame norm size. */
extern void FrameNormSize();
/* FrameNewFooter: Process frame new footer. */
extern void FrameNewFooter();
/* FrameNewHeader: Process frame new header. */
extern void FrameNewHeader();
/* FrameSetBusy: Set frame busy. */
extern void FrameSetBusy();
/* FrameWarpPointer: Warp frame pointer. */
extern void FrameWarpPointer();
/* FrameUnwarpPointer: Unwarp frame pointer. */
extern void FrameUnwarpPointer();

/* generic frame functions */
/* GFrameFocus: Focus g frame. */
extern int GFrameFocus();
/* GFrameSelect: Select g frame. */
extern int GFrameSelect();
/* GFrameSetConfigFunc: Set g frame config func. */
extern int GFrameSetConfigFunc();
/* GFrameSetStack: Set g frame stack. */
extern void GFrameSetStack();
/* GFrameSetConfig: Set g frame config. */
extern void GFrameSetConfig();
/* GFrameEventButtonPress: Process g frame event button press. */
extern int GFrameEventButtonPress();
/* GFrameEventMotionNotify: Process g frame event motion notify. */
extern int GFrameEventMotionNotify();
/* GFrameEventButtonRelease: Process g frame event button release. */
extern int GFrameEventButtonRelease();
/* GFrameEventFocus: Focus g frame event. */
extern int GFrameEventFocus();
/* GFrameEventEnterNotify: Process g frame event enter notify. */
extern int GFrameEventEnterNotify();

/* icon functions */
/* IconInit: Initialize icon. */
extern void IconInit();
/* MakeIcon: Create icon. */
extern struct _winiconframe *MakeIcon();
/* IconChangeName: Change icon name. */
extern void IconChangeName();
/* DrawIconToWindowLines: Draw icon to window lines. */
extern void DrawIconToWindowLines();
/* IconShow: Show icon. */
extern void IconShow();
/* IconHide: Hide icon. */
extern void IconHide();
/* IconSetPos: Set icon pos. */
extern void IconSetPos();

/* icon pane functions */
/* MakeIconPane: Create icon pane. */
extern struct _winiconpane *MakeIconPane();

/* pane functions */
/* MakePane: Create pane. */
extern struct _winpane *MakePane();

/* pinned menu functions */
/* MakeMenu: Create menu. */
extern struct _winmenu *MakeMenu();

/* colormap functions */
/* MakeColormap: Create colormap. */
extern struct _wingeneric *MakeColormap();
/* TrackSubwindows: Track subwindows. */
extern void TrackSubwindows();
/* UnTrackSubwindows: Track un subwindows. */
extern void UnTrackSubwindows();
/* ColormapInhibit: Inhibit colormap. */
extern void ColormapInhibit();
/* InstallColormap: Install colormap. */
extern void InstallColormap();
/* InstallPointerColormap: Install pointer colormap. */
extern void InstallPointerColormap();
/* UnlockColormap: Unlock colormap. */
extern void UnlockColormap();
/* ColorWindowCrossing: Process color window crossing. */
extern void ColorWindowCrossing();
/* ColormapUnhook: Process colormap unhook. */
extern struct _wingeneric *ColormapUnhook();
/* ColormapTransmogrify: Process colormap transmogrify. */
extern void ColormapTransmogrify();

/* selection functions */
/* IsSelected: Check whether selected. */
extern Bool IsSelected();
/* EnumSelections: Process enum selections. */
extern struct _client *EnumSelections();
/* TimeFresh: Process time fresh. */
extern Time TimeFresh();
/* AddSelection: Add selection. */
extern int AddSelection();
/* RemoveSelection: Remove selection. */
extern Bool RemoveSelection();
/* ToggleSelection: Toggle selection. */
extern Bool ToggleSelection();
/* ClearSelections: Clear selections. */
extern void ClearSelections();
/* SelectionResponse: Response selection. */
extern void SelectionResponse();

/* decoration window functions */
/* MakePushPin: Create push pin. */
extern struct _winpushpin *MakePushPin();
/* MakeButton: Create button. */
extern struct _winbutton *MakeButton();
/* MakeResize: Create resize. */
extern struct _winresize *MakeResize();

/* general window functions */
/* WinCallFocus: Focus win call. */
extern void WinCallFocus();
/* WinRedrawAllWindows: Process win redraw all windows. */
extern void WinRedrawAllWindows();
/* WinShowHelp: Show win help. */
extern Bool WinShowHelp();

/* general window event functions */
/* WinEventExpose: Process win event expose. */
extern int WinEventExpose();
/* WinNewPosFunc: Process win new pos func. */
extern int WinNewPosFunc();
/* WinNewConfigFunc: Process win new config func. */
extern int WinNewConfigFunc();
/* WinSetConfigFunc: Set win config func. */
extern int WinSetConfigFunc();

/* rubber-banding functions */
/* UserMoveWindows: Move user windows. */
extern void UserMoveWindows();
/* UserResizeWin: Resize user win. */
extern void UserResizeWin();
/* TraceRootBox: Process trace root box. */
extern void TraceRootBox();

/* busy windows */
/* MakeBusy: Create busy. */
extern struct _winbusy *MakeBusy();

#endif /* _OLWM_OLWM_H */
