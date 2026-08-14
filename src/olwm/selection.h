/*
 * selection.h: declarations and shared types for the selection module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef _OLWM_SELECTION_H
#define _OLWM_SELECTION_H

extern	Time	SelectionTime;

/* IsSelected: Check whether selected. */
extern	Bool	IsSelected(/*  client  */);
/* AddSelection: Add selection. */
extern	int	AddSelection(/*  client, time  */);
/* RemoveSelection: Remove selection. */
extern	Bool	RemoveSelection(/*  client  */);
/* ToggleSelection: Toggle selection. */
extern	Bool	ToggleSelection(/*  client, time  */);
/* ClearSelections: Clear selections. */
extern	void	ClearSelections(/*  dpy  */);
/* EnumSelections: Process enum selections. */
extern	Client* EnumSelections(/*  void*  */);

/* SelectionInit: Initialize selection. */
extern	void	SelectionInit(void);
extern	void	SelectionRegister(Atom selection,
				  void (*handler)(XEvent *event));
/* SelectionResponse: Response selection. */
extern	void	SelectionResponse(/*  event  */);

#endif /* _OLWM_SELECTION_H */
