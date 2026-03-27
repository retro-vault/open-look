/*
 * evbind.h: declarations and shared types for the evbind module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef EVBIND_H
#define EVBIND_H

SemanticAction ResolveMouseBinding(Display *dpy, XEvent *pevent, unsigned long ignoremask);
/* stringToModifier: Convert string to modifier. */
unsigned int stringToModifier(Display *dpy, char *word);
/* polyStringToModifier: Convert poly string to modifier. */
unsigned int polyStringToModifier(Display *dpy, char *str);
KeyBinding *LookupKeyBinding(SemanticAction action);
SemanticAction FindKeyboardAction(Display *dpy, XEvent *event);
/* ExecuteKeyboardFunction: Execute keyboard function. */
Bool ExecuteKeyboardFunction(Display *dpy, XEvent *event);
/* GrabKeys: Grab keys. */
void GrabKeys(Display *dpy, Bool grab);
/* RefreshKeyGrabs: Refresh key grabs. */
void RefreshKeyGrabs(Display *dpy, XrmDatabase rdb);
/* GrabButtons: Grab buttons. */
void GrabButtons(Display *dpy, grab);
/* RefreshButtonGrabs: Refresh button grabs. */
void RefreshButtonGrabs(Display *dpy);
/* UpdateBindings: Set bindings. */
Bool UpdateBindings(Display *dpy, XrmDatabase newDB, Bool regrabKeys);
/* InitBindings: Initialize bindings. */
void InitBindings(Display *dpy);

#endif

