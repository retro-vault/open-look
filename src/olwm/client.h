#ifndef _OLWM_CLIENT_H
#define _OLWM_CLIENT_H

void InitClients(Display *dpy);
void ClientDefaultWindowState(Display *dpy);
void ClientSetWindowState(Client *cli);
void ClientGetWindowState(Client *cli);
void ClientUpdateWindowState(Client *cli, XPropertyEvent *event);
void *ClientSendProtocol(Client *cli, Atom proto, Time evtime);
void *ClientKill(Client *cli, Bool pforce);
void *ClientShutdown(Client *cli, void *junk);
void *UnparentClient(Client *cli, void *junk);
Client *ClientCreate(Display *dpy, int screen);
void ClientSetInstanceVars(Client *cli);
void  DestroyClient(Client *cli);
void ClientConfigure(Client *cli, WinGeneric *win, XConfigureRequestEvent *pxcre);
void ClientSetWMState(Client *cli, WMState wmState);
void ClientProcessDragDropInterest(Client *cli, int state);
void ClientUpdateDragDropInterest(Client *cli, XPropertyEvent *event);
Window ClientPane(Client *cli);
void ClientOpenCloseToggle(Client *cli, Time timestamp);
void ClientFullRestoreSizeToggle(Client *cli, Time timestamp);
void ClientMove(Client *cli, XEvent *trigger);
void ClientResize(Client *cli, XEvent *trigger, WhichResize which, void (*callback)(), void *cbarg);
Client *ClientRaiseTransients(Client *cli);
void ClientLowerTransients(Client *cli);
void ClientFront(Client *cli);
void ClientBack(Client *cli);
void ClientToggleStacking(Client *cli);
void ClientRefresh(Client *cli);
void ClientFlashOwner(Client *cli);
Bool ClientTogglePin(Client *cli);
void *ClientInBox(Client *cli, ClientInBoxClosure *close);
void ClientUpdateBusy(Client *cli, XPropertyEvent *event);
void VisitPinnedMenuClients(void (*pproc)(), void *extra);
void DestroyPinnedMenuClients(void);
void ClientInhibitFocus(Bool inhibit);
void ClientSetFocus(Client *cli,Bool  sendTF, Time evtime);
void ClientSetCurrent(Client *cli);
Client * ClientGetLastCurrent(void);
void ClientActivate(Display *dpy, Client *cli, Time time);
void ClientFocusTopmost(Display *dpy, ScreenInfo *scrinfo, Time time);
void ClientDistributeProperty(Client *cli, XPropertyEvent *event);

#endif
