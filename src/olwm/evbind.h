#ifndef EVBIND_H
#define EVBIND_H

SemanticAction ResolveMouseBinding(Display *dpy, XEvent *pevent, unsigned long ignoremask);
unsigned int stringToModifier(Display *dpy, char *word);
unsigned int polyStringToModifier(Display *dpy, char *str);
KeyBinding *LookupKeyBinding(SemanticAction action);
SemanticAction FindKeyboardAction(Display *dpy, XEvent *event);
Bool ExecuteKeyboardFunction(Display *dpy, XEvent *event);
void GrabKeys(Display *dpy, Bool grab);
void RefreshKeyGrabs(Display *dpy, XrmDatabase rdb);
void GrabButtons(Display *dpy, grab);
void RefreshButtonGrabs(Display *dpy);
Bool UpdateBindings(Display *dpy, XrmDatabase newDB, Bool regrabKeys);
void InitBindings(Display *dpy);

#endif

