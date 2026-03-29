/*
 * Implements the getinfo routines used by the xtoolplaces application.
 *
 * Copyright (c) 1993-1998 Xerox Corporation.  All Rights Reserved.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include "vroot/vroot.h"
#include "xtoolplaces.h"

int mult_screens,screen_number;
char hostname[50];
Atom wm_save_yourself,wm_protocols,wm_client_machine;
Display *display;
Window root_window;
extern int console_checking, ignoredt;
extern char *program,*addon;
extern FILE *fp;

/*This function will initialize the interface to X, lock the pointer and
  change it to a watch, start the recursive procedure of going through
  the window tree list, and then release the pointer and change the cursor
  back to normal when done.

  Inputs:  none
  Outputs: none
  Locals:  child             - list of child window IDs
           cursor            - X cursor bitmap
           hostname          - name of host
           loop              - loop through screen number array
           num_child         - number of children under a parent
           parent            - parent window ID of child in the window tree
           root              - top window ID to start in window tree from
           screens           - array of screens to save from
  Globals: display           - interface info to X display
           mult_screens      - flag indicating if saving more than 1 screen
           program           - name of currently executing program
           root_window       - root window ID
           screen_number     - number of the screen this will be working under
           stderr            - standard error
           wm_client_machine - WM_CLIENT_MACHINE atom
           wm_protocols      - WM_PROTOCOLS atom
           wm_save_yourself  - WM_SAVE_YOURSELF atom
           CurrentTime       - indicates to execute action immediately
           False             - indicates not to do something
           GrabModeAsync     - indicates to grab device asynchronously
           GrabModeSync      - indicates to grab device synchronously
           None              - indicates no choice for an item
           NULL              - 0
           XC_watch          - indicates the watch bitmap
*/
getinfo()
{
        int *screens,loop;
        unsigned int num_child;
        Cursor cursor;
        Window root,parent,*child;
        int *make_list();

        /*Get hostname*/
        if(gethostname(hostname,50)) {
          perror(program);
          exit(1);
        }

        /*Find out what screens to save application states from*/
        screens = make_list(DefaultScreen(display),ScreenCount(display));
        if(screens[1] != -1) mult_screens = 1;
        else mult_screens = 0;

        /*Set up ATOMS to use*/
        wm_save_yourself = XInternAtom(display,"WM_SAVE_YOURSELF",False);
        wm_protocols = XInternAtom(display,"WM_PROTOCOLS",False);
        wm_client_machine = XInternAtom(display,"WM_CLIENT_MACHINE",False);

        /*Change cursor to a watch and ring bell once to let user know
          that the server is tied up and the process has begun*/
        root_window = RootWindow(display,DefaultScreen(display));
#ifndef DEBUG
        cursor = XCreateFontCursor(display,XC_watch);
        XGrabPointer(display,root_window,False,0,GrabModeSync,GrabModeAsync,
                     None,cursor,CurrentTime);
#endif
        XBell(display,50);

        /*Write Bourne shell header*/
        write_header();

        /*Loop through the screens to save from*/
        for(loop = 0;screens[loop] != -1;++loop) {
          /*Get information about root window*/
          screen_number = screens[loop];
          root_window = RootWindow(display,screen_number);

          /*Get list of root window's child*/
          if(!XQueryTree(display,root_window,&root,&parent,&child,&num_child)) {
            fprintf(stderr,"%s: couldn't get root window's child (window 0x%lx)\n",program,root_window);
            exit(1);
          }

          /*Loop through children windows*/
          for(;num_child;--num_child)
            search(display,child[num_child-1],root_window);
  
          /*Free resources*/
          XFree((caddr_t) child);
        }

        /*Change cursor back to normal and ring bell twice to signal that
          the process of saving the desktop is over*/
#ifndef DEBUG
        XUngrabPointer(display,CurrentTime);
#endif
        XBell(display,50);
        XBell(display,50);
        XFlush(display);

        /*Close the connection to the X server*/
        XCloseDisplay(display);
}

/*This function will recursively search through the window tree.  It will check
  each window to see if it's state should be saved with a WM_SAVE_YOURSELF
  message and if it has a WM_COMMAND property to save.  A window is saved
  if:
        1.  it has WM_NAME set (so we know it's not a pop-up, etc)
        2.  it's a group leader or assume it's ok if group leader isn't set
        3.  it's parent is the root window
        4.  it has a WM_COMMAND property to save or is on a list specified
            with the '-m' option
	5.  is not a CDE dt* program when the "ignore dt" switch is given

  Inputs:  display            - interface info to X display
           parent_window      - parent window ID of window we are examining
           window             - window we are examining
  Outputs: none
  Locals:  argc               - number of WM_COMMAND arguments
           argv               - WM_COMMAND arguments
           child              - list of child window IDs
           command_line       - command line arguments in a single string
           console            - indicates if a window is a console
           count              - number of protocols that a window can receive
           event              - event information
           group_leader       - window ID of group leader
           hints              - window hints
           name               - X window name
           num_child          - number of children under a parent
           parent             - parent window ID of child in the window tree
           protocols          - list of protocols that a window can receive
           root               - top window ID to start in window tree from
           save_yourself      - flag indicating if window saved itself or not
           status             - status of get calls
  Globals: addon              - name of file containing commands to add on
           console_checking   - indicates if the check console flag was given
           fp                 - file pointer to write window information to
	   ignore	      - file containing X apps to NOT save
	   ignoredt	      - indicates if we should ignore CDE dt* apps
	   ilist	      - list of X apps to ignore
	   inumvals	      - number of X apps in ilist
	   missing	      - file containing X apps to make sure are saved
	   mlist	      - list of X apps to make sure are saved
	   mnumvals	      - number of X apps in mlist
           mult_screens       - flag indicating if saving more than 1 screen
           on_missing_list    - flag indicating if window is a missing app
           program            - name of currently executing program
           root_window        - root window ID
           screen_number      - number of the screen this will be working under
           stderr             - standard error
           wm_protocols       - WM_PROTOCOLS atom
           wm_save_yourself   - WM_SAVE_YOURSELF atom
           ClientMessage      - event that indicates a message from 1 client
                                to another
           False              - indicates not to do something
           PropertyChangeMask - indicates a window property has changed
           PropertyNotify     - event that indicates a property has changed
           WindowGroupHint    - mask for group hint bit in hint flags
*/
search(display,window,parent_window)
Display *display;
Window window,parent_window;
{
        int save_yourself,on_missing_list,console = 0;
        unsigned int num_child;
        int count,argc,group_leader = 0;
        char *command_line,*name;
        char **argv;
        Atom *protocols;
        Status status;
        void getsignal();
        Window root,parent,*child;
        XEvent event;
        XWMHints *hints;
        char *combine(),*getgeom(),*getname(),*lower(),*get_emacs_name(),
			 *fix_command2();
	extern int inumvals, mnumvals, ignoredt;
	extern char *ignore, *ilist[MAXAPPS], *missing, *mlist[MAXAPPS];


        /*If a child had children of it's own, loop thru it's children
          recursively*/
        if(XQueryTree(display,window,&root,&parent,&child,&num_child)) {
          for(;num_child;--num_child)
            search(display,child[num_child-1],parent_window);

          /*Free resources (if allocated)*/
          if(child) XFree((caddr_t) child);
        }

	/*Don't report window if it doesn't have a name (in which case it's
	  probably a pop-up or unmapped child).  Need to do this separately
	  from the other checks because we want to free the name if the
	  call works.*/
	if (name = getname(window))
	  XFree(name);
	else
	  return;

        /*Don't report window if the app is on the ignore list or if the
	  "-idt" or "ignoredt" resource flag is given and the window belongs
	  to a CD dt* app.  CDE create windows which hang XNextEvent().  We
	  can eliminate those by looking for windows with a size of 1x1 or
	  a class of "Dtterm"*/
        if((ignore && on_list(window, ilist, inumvals)) ||
	   is_dtterm(window) || is_1x1(window) ||
	   (ignoredt && is_dtapp(window)))
	  return;

        /*Don't report window if it's not a group leader or isn't a top
          window*/
        hints = XGetWMHints(display,window);

        /*Also check if program didn't set #!$% group leader, in this case
          assume it's ok (I'm sure that's asking for trouble!)*/
        if(hints) {
          if((hints->window_group == window) ||
             !(hints->flags & WindowGroupHint))
            group_leader = 1;
          XFree((caddr_t) hints);
        }
        if(!group_leader || (parent_window != root_window)) return;

        /*Determine if window has WM_SAVE_YOURSELF ATOM and send it if
          it does to save it's state*/
        status = XGetWMProtocols(display,window,&protocols,&count);
        if(status && is_save_yourself(protocols,count)) {
          save_yourself = 1;

          /*If window does have WM_SAVE_YOURSELF, send it a message to
            save it's state and wait for the PropertyNotify response*/
          event.type = ClientMessage;
          event.xclient.window = window;
          event.xclient.message_type = wm_protocols;
          event.xclient.format = 32;
          event.xclient.data.l[0] = (long) wm_save_yourself;
          XSelectInput(display,window,PropertyChangeMask);
          XSendEvent(display,window,False,0L,&event);
          XNextEvent(display,&event);
          if(event.type == PropertyNotify) {
            if(event.xproperty.window != window) {
              fprintf(stderr,"%s: unexpected PropertyNotify event\n",program);
              exit(1);
            }
          } else {
                   fprintf(stderr,"%s: unexpected event\n",program);
                   exit(1);
                 }
        } else save_yourself = 0;

        /*Free resources*/
        if(status) XFree((caddr_t) protocols);

        /*Determine if window is on missing apps list*/
	on_missing_list = (missing) ? on_list(window, mlist, mnumvals) : 0;

        /*Get WM_COMMAND.  If window doesn't have it set, isn't EMACS, or
          isn't a console tool, skip it, otherwise write it out.  We check
          for EMACS *specifically* since even though it does EVERYTHING, it
          doesn't set WM_COMMAND (sheesh, even xterm does that!).  Must
          be a security feature :-{)*/
        status = XGetCommand(display,window,&argv,&argc);
        if((status && !isnull(window)) || on_missing_list ||
	   is_emacs(window) || is_console(window,NULL)) {
          /*Put arguments into a single string and free up argv.  Makes it
            easier to check for things*/
          if(status) {
	    if(argc) {
	      command_line = combine(argc,argv);
              XFreeStringList(argv);
	    } else command_line = lower(getname(window));
          } else {
                   /*It is ASSUMED that if we have to save a window that
                    doesn't have WM_COMMAND set, it's name is the basename
                    of WM_NAME in all lower case (i.e. xman sets WM_NAME to
                    Xman).  There is, of course, the EMACS exception.  It
                    doesn't put it's executable name in WM_NAME but instead
                    uses <executable@hostname>.*/
                   if(is_emacs(window))
                     command_line = get_emacs_name(getname(window));
                   else
                     command_line = lower(getname(window));
                   save_yourself = 0;
                 }

          if(!save_yourself) command_line = getgeom(window,command_line);

          /*If console checking flag was given, check each window to see
            if it's a console window*/
          if(console_checking)
            console = is_console(window,command_line);

          /*If this window is a console window, write out the special
            'if ... fi' clause*/
          if(console)
            fputs("if [ \"`tty`\" = \"/dev/console\" ]\nthen\n  ",fp);

          /*Check to see if window is a remote application*/
          is_remote(window);

          /*Fix Xview's long label problem*/
          fix_command(command_line);

	  /*Escape pound signs*/
          command_line = fix_command2(command_line);

          /*If saving from more than 1 screen, make sure each application
            has the correct -display option to put it back onto the correct
            screen*/

          /*Write out all the arguments in WM_COMMAND*/
	  if(do_toolwait)
	    fprintf(fp,"%s %s ",tw_str,command_line);
	  else
            fprintf(fp,"%s ",command_line);

          if(mult_screens)
            adddisplay(command_line,screen_number);

          /*If -a option given, search for any addon commands for this window*/
          if(addon) addto(command_line);

          /*Write out ampersand to put command into the background if
	    not using a toolwait program*/
	  if(do_toolwait)
	    fputs("\n",fp);
	  else
            fputs("&\n",fp);

          if(console)
            fputs("fi\n",fp);

          /*Free resources*/
          free(command_line);
        }
}

/*This function will convert all letters in a string to lower case (if not
  already lower case).  It will also take the basename of the string.

  Inputs:  string    - string to turn all characters to lower case
  Outputs: string    - string of all lower case characters
  Locals:  loop      - loop through string
           newstring - lower case basename of original string
           pointer   - pointer to directory slash to get basename
  Globals: program   - name of currently executing program
*/
char *lower(string)
char *string;
{
        int loop;
        char *pointer,*newstring;

        if((pointer = rindex(string,'/')) == NULL) pointer = string;
        else ++pointer;

        if((newstring = strdup(pointer)) == NULL) {
          perror(program);
          exit(1);
        }

        for(loop = 0;loop < strlen(newstring);++loop)
          if(isupper(newstring[loop]))
            newstring[loop] = tolower(newstring[loop]);

        return(newstring);
}

/*This function will retrieve the executable name from the EMACS WM_NAME
  atom.  The form (at least with emacs 19) seems to be <executable@hostname>.

  Inputs:  name - WM_NAME atom value
  Outputs: exec    - name of executable
  Locals:  exec    - name of executable
           pointer - pointer to '@' to get name of executable
  Globals: program - name of currently executing program
*/
char *get_emacs_name(name)
char *name;
{
        char *pointer,*exec;

        /*If for some reason, the @ isn't there, we'll assume the whole
          name field is the name of the executable.  I don't suppose that
          would happen, but you never know*/
        pointer = index(name,'@');

        if(pointer) *pointer = '\0';

        if((exec = strdup(name)) == NULL) {
          perror(program);
          exit(1);
        }

        if(pointer) *pointer = '@';

        return(exec);
}
