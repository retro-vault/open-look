/*
 * events.h: declarations and shared types for the events module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef _OLWM_EVENTS_H
#define _OLWM_EVENTS_H

/*
 * Define InterposerFunc to be a pointer to an interposer function.
 */
typedef int (*InterposerFunc)();

enum {
    DISPOSE_DISPATCH,
    DISPOSE_USED,
    DISPOSE_DEFER
};

/* keyboard mapping */
extern KeySym *KbdMap;
extern int MinKeyCode;
extern int MaxKeyCode;
extern int KeySymsPerKeyCode;

/* modifiers and modifier masks */

enum {
    MOD_CONSTRAIN,
    MOD_WMGRAB,
    MOD_REDUCE,
    MOD_INVERT,
    MOD_SETDEFAULT,
    MOD_IGNORE,
    MOD_MASK_COUNT		/* must be last */
};

extern unsigned int ModMaskMap[MOD_MASK_COUNT];

/* mouse binding match states */
typedef enum {
    MATCH_NONE,		/* no binding matches at all */
    MATCH_INCOMPLETE,	/* partial match */
    MATCH_AMBIG,	/* more than one exact match */
    MATCH_PREFIX,	/* exact match, but also a prefix for another */
    MATCH_EXACT		/* exact match, not a prefix */
} MouseMatchState;

/* semantic actions */
typedef enum {
    ACTION_NONE,
    ACTION_SELECT,
    ACTION_ADJUST,
    ACTION_MENU,
    ACTION_HELP,
    ACTION_STOP,
    ACTION_FRONT,
    ACTION_OPEN,
    ACTION_EXEC_DEFAULT,
    ACTION_FOCUS_HELP,
    ACTION_SET_DEFAULT,
    ACTION_UP,
    ACTION_DOWN,
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_JUMP_UP,
    ACTION_JUMP_DOWN,
    ACTION_JUMP_LEFT,
    ACTION_JUMP_RIGHT,
    ACTION_ROW_START,
    ACTION_ROW_END,
    ACTION_DATA_START,
    ACTION_DATA_END,
    ACTION_FIRST_CONTROL,
    ACTION_LAST_CONTROL,
    ACTION_TOGGLE_PIN,
    ACTION_CANCEL,		/* REMIND does this differ from STOP? */
    ACTION_NEXT_ELEMENT,
    ACTION_PREVIOUS_ELEMENT,

    /* actions for menu accelerators */

    ACTION_REFRESH,
    ACTION_BACK,
    ACTION_OPEN_CLOSE,
    ACTION_FULL_RESTORE,
    ACTION_QUIT,
    ACTION_OWNER,
    ACTION_MOVE,
    ACTION_RESIZE,
    ACTION_PROPS
} SemanticAction;

/* key bindings */

typedef struct {
    char		*rsrc_name;
    char		*dflt_binding;
    void		(*function)();
    SemanticAction	action;
    unsigned long	flags;
} KeyDescriptor;

/* values for KeyDescriptor flags */
#define KD_IMMUNE	(1<<0)		/* immune to suspension */
#define KD_SUNVIEW	(1<<1)		/* active if mouseless == SunView */
#define KD_BASIC	(1<<2)		/* active if mouseless == basic */
#define KD_FULL		(1<<3)		/* active if mouseless == full */
#define KD_MENU_ACCEL	(1<<4)		/* is a menu accelerator */

#define KD_BASIC_FULL	(KD_BASIC | KD_FULL)
#define KD_ALWAYS	(KD_SUNVIEW | KD_BASIC_FULL)

typedef struct _keyBinding {
    unsigned int	modstate;
    KeyCode		keycode;
    KeyDescriptor	*desc;
} KeyBinding;

/* LookupKeyBinding: Return key binding. */
extern KeyBinding *LookupKeyBinding(/* SemanticAction */);
/* ModifierToKeysym: Convert modifier to keysym. */
extern KeySym ModifierToKeysym();

/* convert a button number to a button mask */
#define ButtonToMask(b) (1<<(b+7))

#define AnyButtonMask \
    (Button1Mask|Button2Mask|Button3Mask|Button4Mask|Button5Mask)

/* given a ButtonRelease event, determines whether all buttons are now up. */
#define AllButtonsUp(e) \
  (!((e)->xbutton.state & ~ButtonToMask((e)->xbutton.button) & AnyButtonMask))

/* given a ButtonPress event, determine whether it's the first button down. */
#define FirstButtonDown(e) \
  (((e)->xbutton.state & AnyButtonMask) == 0)

/* timeouts */
typedef void (*TimeoutFunc)(void *closure);

/*
 * declared in evbind.c
 */
extern Bool mouselessSuspended;
struct _wingeneric;
struct _wingeneric *lookupWindow(XEvent *event);
/* saveTimestamp: Process save timestamp. */
void saveTimestamp(XEvent *event);
/* handleMappingNotify: Process handle mapping notify. */
void handleMappingNotify(Display *dpy, XEvent *e);
/* EventLoop: Process event loop. */
void EventLoop(Display *dpy);
/* PropagateEventToParent: Convert propagate event to parent. */
int PropagateEventToParent(Display *dpy, XEvent *event, struct _wingeneric *win);
/* PropagatePressEventToChild: Convert propagate press event to child. */
void PropagatePressEventToChild(Display *dpy, XButtonPressedEvent *event, struct _wingeneric *win);
/* FindModifierMask: Return modifier mask. */
unsigned int FindModifierMask(KeyCode kc);
KeySym ModifierToKeysym(unsigned int mod);
/* AwaitEvents: Process await events. */
Bool AwaitEvents(Display *dpy, struct timeval *timeout);
/* InstallInterposer: Install interposer. */
void InstallInterposer(InterposerFunc func,void * cl);
/* UninstallInterposer: Uninstall interposer. */
void UninstallInterposer(void);
InterposerFunc InterposerInstalled(void);
/* EnableInterposerDelegation: Enable interposer delegation. */
void EnableInterposerDelegation(void);
/* TimeoutRequest: Request timeout. */
void TimeoutRequest(int t, TimeoutFunc f, void *closure);
/* TimeoutCancel: Process timeout cancel. */
void TimeoutCancel(void);
/* InitEvents: Initialize events. */
void InitEvents(Display *dpy);

#endif /* _OLWM_EVENTS_H */
