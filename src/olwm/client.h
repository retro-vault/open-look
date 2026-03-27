/*
 * client.h: declarations and shared types for the client module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef _OLWM_CLIENT_H
#define _OLWM_CLIENT_H

/* InitClients: Initialize clients. */
void InitClients(Display *dpy);
/* ClientDefaultWindowState: Default client window state. */
void ClientDefaultWindowState(Display *dpy);
/* ClientSetWindowState: Set client window state. */
void ClientSetWindowState(Client *cli);
/* ClientGetWindowState: Return window state. */
void ClientGetWindowState(Client *cli);
/* ClientUpdateWindowState: Update client window state. */
void ClientUpdateWindowState(Client *cli, XPropertyEvent *event);
/* ClientSendProtocol: Send client protocol. */
void *ClientSendProtocol(Client *cli, Atom proto, Time evtime);
/* ClientKill: Kill client. */
void *ClientKill(Client *cli, Bool pforce);
/* ClientShutdown: Process client shutdown. */
void *ClientShutdown(Client *cli, void *junk);
/* UnparentClient: Process unparent client. */
void *UnparentClient(Client *cli, void *junk);
/* ClientCreate: Create client. */
Client *ClientCreate(Display *dpy, int screen);
/* ClientSetInstanceVars: Set client instance vars. */
void ClientSetInstanceVars(Client *cli);
/* DestroyClient: Destroy client. */
void  DestroyClient(Client *cli);
/* ClientConfigure: Process client configure. */
void ClientConfigure(Client *cli, WinGeneric *win, XConfigureRequestEvent *pxcre);
/* ClientSetWMState: Set client wm state. */
void ClientSetWMState(Client *cli, WMState wmState);
/* ClientProcessDragDropInterest: Process client drag drop interest. */
void ClientProcessDragDropInterest(Client *cli, int state);
/* ClientUpdateDragDropInterest: Update client drag drop interest. */
void ClientUpdateDragDropInterest(Client *cli, XPropertyEvent *event);
/* ClientPane: Process client pane. */
Window ClientPane(Client *cli);
/* ClientOpenCloseToggle: Open client close toggle. */
void ClientOpenCloseToggle(Client *cli, Time timestamp);
/* ClientFullRestoreSizeToggle: Toggle client full restore size. */
void ClientFullRestoreSizeToggle(Client *cli, Time timestamp);
/* ClientMove: Move client. */
void ClientMove(Client *cli, XEvent *trigger);
void ClientResize(Client *cli, XEvent *trigger, WhichResize which, void (*callback)(), void *cbarg);
/* ClientRaiseTransients: Raise client transients. */
Client *ClientRaiseTransients(Client *cli);
/* ClientLowerTransients: Lower client transients. */
void ClientLowerTransients(Client *cli);
/* ClientFront: Process client front. */
void ClientFront(Client *cli);
/* ClientBack: Process client back. */
void ClientBack(Client *cli);
/* ClientToggleStacking: Toggle client stacking. */
void ClientToggleStacking(Client *cli);
/* ClientRefresh: Refresh client. */
void ClientRefresh(Client *cli);
/* ClientFlashOwner: Flash client owner. */
void ClientFlashOwner(Client *cli);
/* ClientTogglePin: Toggle client pin. */
Bool ClientTogglePin(Client *cli);
/* ClientInBox: Process client in box. */
void *ClientInBox(Client *cli, ClientInBoxClosure *close);
/* ClientUpdateBusy: Update client busy. */
void ClientUpdateBusy(Client *cli, XPropertyEvent *event);
void VisitPinnedMenuClients(void (*pproc)(), void *extra);
/* DestroyPinnedMenuClients: Destroy pinned menu clients. */
void DestroyPinnedMenuClients(void);
/* ClientInhibitFocus: Inhibit client focus. */
void ClientInhibitFocus(Bool inhibit);
/* ClientSetFocus: Set client focus. */
void ClientSetFocus(Client *cli,Bool  sendTF, Time evtime);
/* ClientSetCurrent: Set client current. */
void ClientSetCurrent(Client *cli);
/* ClientGetLastCurrent: Return last current. */
Client * ClientGetLastCurrent(void);
/* ClientActivate: Process client activate. */
void ClientActivate(Display *dpy, Client *cli, Time time);
/* ClientFocusTopmost: Focus client topmost. */
void ClientFocusTopmost(Display *dpy, ScreenInfo *scrinfo, Time time);
/* ClientDistributeProperty: Process client distribute property. */
void ClientDistributeProperty(Client *cli, XPropertyEvent *event);

#endif
