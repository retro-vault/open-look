/*
 * notice.h: declarations and shared types for the notice module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef _OLWM_NOTICE_H
#define _OLWM_NOTICE_H

#define NOTICE_CANCEL	(-1)

#define NOTICE_BUTTON_COUNT(b)	sizeof((b))/sizeof(char *)

typedef struct _noticeBox {
	int	numButtons;	/* number of buttons */
	int	defaultButton;	/* index into buttonText array */
	Text	**buttonText;	/* array of strings for button text */
	Text	*msgText;
	int	boxX;		/* box origin (-1 =use default/centered) */
	int	boxY;		/* box origin (-1 =use default/centered) */
} NoticeBox;

/* function declarations */
/* UseNoticeBox: Process use notice box. */
extern int UseNoticeBox();
void UseNoticeBoxSync(Display *dpy, int screen, NoticeBox *noticeBox,
                      void (*callback)());

#endif /* _OLWM_NOTICE_H */
