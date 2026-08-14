/*
 * Implements the is routines used by the xtoolplaces application.
 *
 * Copyright (c) 1993-1998 Xerox Corporation.  All Rights Reserved.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include "xtoolplaces.h"

extern char *remote,hostname[];
extern Atom wm_save_yourself,wm_client_machine;
extern Display *display;
extern FILE *fp;

/*This function will check a list of protocols to see if the WM_SAVE_YOURSELF
  protocol is in the list

  Inputs:  count            - number of protocols in list
           protocols        - protocol list
  Outputs: 1 if WM_SAVE_YOURSELF is found, 0 otherwise
  Locals:  none
  Globals: wm_save_yourself - WM_SAVE_YOURSELF atom
*/
int
is_save_yourself(protocols,count)
int count;
Atom *protocols;
{
        for(;count;--count)
          if(protocols[count-1] == wm_save_yourself)
            return(1);

        return(0);
}

/*This function will read in WM_COMMAND from a window and determine if
  WM_COMMAND is set, but null.  This is a problem which XGetCommand()
  doesn't handle.  If WM_COMMAND is null, XGetCommand() still returns
  an argv pointer with the number of arguments equal to 1.  This code
  is lifted (and modified) from xprop (i.e. I didn't think of it myself).

  This routine is needed because some windows are EXACTLY like a top-level
  window to save EXCEPT their WM_COMMAND property is null (i.e. Filemgr's
  wastebasket and Mailtool's Compose window -- both from the OpenWindow
  deskset).

  Inputs:  window        - window we are examining
  Outputs: 1 if null (or no WM_COMMAND), 0 otherwise
  Locals:  actual_format - actual data type of the returned data
           actual_type   - actual type of WM_COMMAND
           bytes_after   - number of bytes remaining to be read in if a
                           partial read was performed
           nitems        - actual number of items returned in 'prop'.
           prop          - pointer to the data in WM_COMMAND
           status        - status of get call
*/
int
isnull(window)
Window window;
{
        int status;
        int actual_format;
        unsigned long nitems,bytes_after;
        unsigned char *prop;
        Atom actual_type;

        status = XGetWindowProperty(display,window,XA_WM_COMMAND,0,2500,False,
                                    AnyPropertyType,&actual_type,
                                    &actual_format,&nitems,&bytes_after,&prop);

        if((status == Success) && nitems) return(0);
        else return(1);
}


/*This function will determine if a window is acting as a console window.  It
  does this by first checking the class of the window.  Since there is no
  standard class for consoles, maybe this program will start one.  The window
  is considered a console if it's class is XConsole.  If the class isn't
  XConsole (which is most likely the case), functions are called to check
  for each console type that I could think of.  This includes contool,
  Sun's shelltool & cmdtool, and xterm.  Each (of course) has a different
  way of identifying when it's on console mode.  To add other consoles, just
  add another function call here and the function to make the check.  A 1
  is returned if the window is a console, 0 otherwise.

  Inputs:  command_line  - WM_COMMAND arguments in a single string
           window        - ID of the window that we are going to check
  Outputs: console       - indicates if a window is a console
  Locals:  class_hints   - window class
           console       - indicates if a window is a console
           status        - status of get call
  Globals: display       - interface info to X display
           CONSOLE_CLASS - class name for console windows
*/
int
is_console(window,command_line)
char *command_line;
Window window;
{
        int console = 0;
        Status status;
        XClassHint class_hints;

        /*Get window class and check it against what a class for a console
          window should be*/
        status = XGetClassHint(display,window,&class_hints);
        if(status && !strcmp(class_hints.res_class,CONSOLE_CLASS))
          console = 1;

        /*Free resources if allocated*/
        if(status) {
          XFree(class_hints.res_name);
          XFree(class_hints.res_class);
        }

        /*If the class doesn't match, check for each type of console
          window possible*/
        if(!console)
          if(!(console = check_contool(window)))
            if(command_line && !(console = check_sun_tools(window)))
              console = check_xterm(window,command_line);

        return(console);
}

/*This function will check to see if a window is the contool window.  Since
  contool is always a console, we only have to check the window name to see
  if the word 'Contool' is in the name.  A 1 is returned if it is a contool
  window, 0 otherwise.

  Inputs:  window       - ID of the window that we are going to check
  Outputs: contool      - flag indicating if window is contool window or not
  Locals:  contool      - flag indicating if window is contool window or not
           name         - X window name
  Globals: display      - interface info to X display
           CONTOOL_NAME - name of contool window
*/
int
check_contool(window)
Window window;
{
        int contool = 0;
        char *name;
        char *getname();

        /*Get the name of the window to compare it against the name contool
          gives it's window*/
        name = getname(window);
        if(name && strstr(name,CONTOOL_NAME)) {
          contool = 1;
          XFree(name);
        }

        return(contool);
}

/*This function will check to see if a window is either a shelltool or
  cmdtool in the console mode.  Since the icon name is CONSOLE only when
  either is started up in console mode, we can check that.  A 1 is returned
  if it is in console mode, 0 otherwise.

  Inputs:  window              - ID of the window that we are going to check
  Outputs: sun_tools           - flag indicating if window is in console mode
  Locals:  status              - status of get call
           sun_tools           - flag indicating if window is in console mode
           tp                  - window's text property
  Globals: display             - interface info to X display
           SUN_TOOLS_ICON_NAME - icon name for {shell,cmd}tool in console mode
*/
int
check_sun_tools(window)
Window window;
{
        int sun_tools = 0;
        Status status;
        XTextProperty tp;

        /*Get window's icon name and check it against what Sun labels the icon
          of either shelltool or cmdtool in console mode*/
        status = XGetWMIconName(display,window,&tp);
        if(status && !strcmp(tp.value,SUN_TOOLS_ICON_NAME))
          sun_tools = 1;

        /*Free resources if allocated*/
        if(status) XFree(tp.value);

        return(sun_tools);
}

/*This function will check to see if a window is xterm in the console mode.
  The only way to determine if xterm is in console mode is to check the
  options to see if it was started with the -C option.  A 1 is returned
  if it is in console mode, 0 otherwise.

  Inputs:  command_line - WM_COMMAND arguments in a single string
           window       - ID of the window that we are going to check
  Outputs: xterm        - flag indicating if window is xterm in console mode
  Locals:  xterm        - flag indicating if window is xterm in console mode
  Globals: XTERM_OPTION - xterm option to put it into console mode
*/
int
check_xterm(window,command_line)
char *command_line;
Window window;
{
        int xterm = 0;

        /*See if it's xterm*/
        if(is_xterm(window) == 1)
          /*If window is xterm, then check argument list for xterm console
            mode option*/
          if(strstr(command_line,XTERM_OPTION)) xterm = 1;

        return(xterm);
}

/*This function determines if a particular window is an xterm window.  To
  see if a window is an xterm window, we check it's class.  Xterm always sets
  it's class to "XTerm".  1 is returned if it is an xterm window, 0 if it
  isn't, -1 if there is an error in getting the class of the window.

  Inputs:  window   - ID of the window that we are going to check
  Outputs: 1 if xterm, 0 if not xterm, -1 if error in getting class
  Locals:  match    - flag indicating if window is xterm window or not
           status   - status of get call
           tp       - window's text property
  Globals: display  - interface info to X display
*/
int
is_xterm(window)
Window window;
{
        int match;
        Status status;
        XClassHint class_hints;

        /*Get window class to see if it's xterm*/
        status = XGetClassHint(display,window,&class_hints);

        if(status) {
          match = strcmp(class_hints.res_class,"XTerm");
          XFree(class_hints.res_name);
          XFree(class_hints.res_class);

          return((!match)?1:0);
        } else return(-1);
}

/*This function determines if a particular window is an dtterm window.  To
  see if a window is an dtterm window, we check it's class.  Dtterm always sets
  it's class to "Dtterm".  1 is returned if it is an dtterm window, 0 if it
  isn't, -1 if there is an error in getting the class of the window.

  Inputs:  window   - ID of the window that we are going to check
  Outputs: 1 if dtterm, 0 if not dtterm, -1 if error in getting class
  Locals:  match    - flag indicating if window is dtterm window or not
           status   - status of get call
           tp       - window's text property
  Globals: display  - interface info to X display
*/
int
is_dtterm(window)
Window window;
{
        int match;
        Status status;
        XClassHint class_hints;

        /*Get window class to see if it's xterm*/
        status = XGetClassHint(display,window,&class_hints);

        if(status) {
          match = strcmp(class_hints.res_class,"Dtterm");
          XFree(class_hints.res_name);
          XFree(class_hints.res_class);

          return((!match)?1:0);
        } else return(-1);
}

/*This function determines if a particular window is a window from a CDE
  dt* app (i.e. dtmail, dtfile, etc.).  To see if a window is from a CDE
  dt* app, we check it's class.  The class name always starts with "Dt".
  1 is returned if it is a CDE dt* app window, 0 if it isn't, -1 if there
  is an error in getting the class of the window.

  Inputs:  window   - ID of the window that we are going to check
  Outputs: 1 if dt* app, 0 if not, -1 if error in getting class
  Locals:  match    - flag indicating if window is dt* app window or not
           status   - status of get call
           tp       - window's text property
  Globals: display  - interface info to X display
*/
int
is_dtapp(window)
Window window;
{
        int match;
        Status status;
        XClassHint class_hints;

        /*Get window class to see if it's xterm*/
        status = XGetClassHint(display,window,&class_hints);

        if(status) {
	  if((tolower(class_hints.res_class[0]) == 'd') &&
	     (tolower(class_hints.res_class[1]) == 't'))
            match = 1;
	  else match = 0;

          XFree(class_hints.res_name);
          XFree(class_hints.res_class);

          return(match);
        } else return(-1);
}

/*This function determines if a particular window has a size of 1x1.  This
  is the size created by CDE that will hang xtoolplaces if not caught.  I
  don't know if any other window manager creates windows of size 1x1.  1 is
  returned if it is a window of size 1x1, 0 if it isn't, -1 if there is an
  error in getting the size of the window.

  Inputs:  window   - ID of the window that we are going to check
  Outputs: 1 if size = 1x1, 0 if not, -1 if error in getting size
  Locals:  match    - flag indicating if window size = 1x1
           status   - status of get call
           tp       - window's text property
  Globals: display  - interface info to X display
*/
int
is_1x1(window)
Window window;
{
	int dummy;
	unsigned int width, height, udummy;
        Status status;
	Window root;

	/*Only get the geometry information we care about, the width & height*/
	status = XGetGeometry(display, window, &root, &dummy, &dummy,
			      &width, &height, &udummy, &udummy);

	/*Check the width and height if we were able to get the size*/
	if(status) {
	  if((width == 1) && (height == 1))
	    return(1);
	  else
	    return(0);
	} else return(-1);
}

/*This function determines if a particular window is an emacs window.  To
  see if a window is an emacs window, we check it's class.  Emacs always sets
  it's class to "Emacs".  1 is returned if it is an emacs window, 0 if it
  isn't, -1 if there is an error in getting the name of the window.

  Inputs:  window   - ID of the window that we are going to check
  Outputs: 1 if emacs, 0 if not emacs, -1 if error in getting class
  Locals:  match    - flag indicating if window is emacs window or not
           status   - status of get call
           tp       - window's text property
  Globals: display  - interface info to X display
*/
int
is_emacs(window)
Window window;
{
        int match,propstatus;
        int actual_format;
        unsigned long nitems,bytes_after;
        unsigned char *prop;
        Atom epoch_current;
        Atom actual_type;
        Status status;
        XClassHint class_hints;

        /*Get window class to see if it's emacs*/
        status = XGetClassHint(display,window,&class_hints);

        if(status) {
          match = strcmp(class_hints.res_class,"Emacs");

          epoch_current = XInternAtom(display,"EPOCH_CURRENT",True);
          if(epoch_current != None) {
            propstatus = XGetWindowProperty(display,window,epoch_current,0,1,
                                            False,AnyPropertyType,&actual_type,
                                            &actual_format,&nitems,&bytes_after,
                                            &prop);

          /*It's weird that XGetWindowProperty() would return Success without
            sending back any properties (prop = NULL)*/
          if((propstatus == Success) && prop && strcmp(prop,"minibuf"))
            match = 0;
          else match = 1;

          XFree(class_hints.res_name);
          XFree(class_hints.res_class);
          if(prop) XFree(prop);
          }

          return((!match)?1:0);
        } else return(-1);
}

/*This function determines if a particular window is a calctool window.  To
  see if a window is a calctool window, we check it's name.  Calctool always
  sets it's name to "calctool".  1 is returned if it is a calctool window, 0
  if it isn't, -1 if there is an error in getting the name of the window.

  Inputs:  window    - ID of the window that we are going to check
  Outputs: 1 if calctool, 0 if not calctool, -1 if error in getting name
  Locals:  status    - status of get call
           text_prop - window's text property
  Globals: display   - interface info to X display
*/
int
is_calctool(window)
Window window;
{
        Status status;
        XTextProperty text_prop;

        /*Get window name to see if it's calctool*/
        status = XGetWMName(display,window,&text_prop);

        if(status) return((!strcmp(text_prop.value,"calctool"))?1:0);
        else return(-1);
}

/*This function checks to see if an application is being executed remotely.
  If it is, it prints out the remote execution command (typically 'rsh -n') and
  the name of the remote machine to execute on.

  Inputs:  window            - ID of the window that we are going to check
  Outputs: none
  Locals:  status            - status of get call
           tp                - window's text property
  Globals: display           - interface info to X display
           fp                - file pointer to write window information to
           hostname          - name of host
           remote            - command to use for remote applications
           wm_client_machine - WM_CLIENT_MACHINE atom
*/
void
is_remote(window)
Window window;
{
        Status status;
        XTextProperty tp;

        status = XGetTextProperty(display,window,&tp,wm_client_machine);

        /*Check if WM_CLIENT_MACHINE exists and isn't the name of the
          machine we are currently executing on*/
        if(status && strcmp(tp.value,hostname)) {
          /*Print out the remote command if there is one*/
          if(!remote) fprintf(fp,"%s ",tp.value);
          else fprintf(fp,"%s %s ",remote,tp.value);
        }
}

/*This function gets the name of an X window and returns it.

  Inputs:  window   - ID of the window that we are going to check
  Outputs: tp.value - name of X window
  Locals:  status   - status of get call
           tp       - window's text property
  Globals: display  - interface info to X display
           NULL     - 0
*/
char *getname(window)
Window window;
{
        Status status;
        XTextProperty tp;

        /*Get the name of the window*/
        status = XGetWMName(display,window,&tp);

        if(status) return((char *) tp.value);
        else return(NULL);
}
