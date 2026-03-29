/*
 * Implements the readfile routines used by the xtoolplaces application.
 *
 * Copyright (c) 1993-1998 Xerox Corporation.  All Rights Reserved.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "xtoolplaces.h"

extern char *program;
extern Display *display;

/*This function will read in the list of X applications.

  Inputs:  file         - file to read
	   indx		- index into list
	   list		- list of X applications
  Outputs: none
  Locals:  fp           - file pointer to read from
           line         - line of text read in
  Globals: program      - name of currently executing program
           stderr       - standard error
           NULL         - 0
*/
readfile(file, list, indx)
int *indx;
char *file, *list[50];
{
        char line[200];
        FILE *fp;

        /*Open the file for reading*/
        if((fp = fopen(file,"r")) == NULL) {
          fprintf(stderr,"%s: can't open {%s}\n",program,file);
          exit(1);
        }

        *indx = 0;

        /*Read lines from file until there is no more*/
        while(fgets(line,200,fp) != NULL)
          switch(line[0]) {
            case '#':                   /*Comment character - ignore line*/
            case '\n':                  /*Blank line - ignore line*/
                        break;
            default:
                        line[strlen(line)-1] = '\0';  /*Take out NL char*/
                        if((list[(*indx)++] = strdup(line)) == NULL) {
                          perror(program);
                          exit(1);
                        }

                        break;
          }
}

/*This function will determine if an application's window is the specified
  applications list.  A 1 is returned if on the list, 0 if not.

  Inputs:  list	  - list of X applications to check against
	   numval - number of values in list
	   window - ID of the window that we are going to check
  Outputs: result - 1 if application is on the list, 0 if not
  Locals:  loop   - loop through name list
           name   - name of window
           result - flag indicating if window is on list or not
	   status - status of get call
  Globals: none
*/
on_list(window, list, numval)
char *list[50];
int numval;
Window window;
{
        int loop,result;
        char *name, *class;
        char *getname();
	Status status;
	XClassHint class_hints;

        result = 0;

	/*Get information about the window*/
	name = getname(window);
	if((status = XGetClassHint(display, window, &class_hints)))
	  class = class_hints.res_class;
	else
	  class = NULL;

	/*See if we can find the name or class on the list*/
        if(name || class)
          for(loop = 0;!result && (loop < numval);++loop)
            if((name && !strcmp(name,list[loop])) ||
	       (class && !strcmp(class, list[loop])))
	      result = 1;

	/*Clean up*/
	if(name) XFree(name);
	if(status) {
	  XFree(class_hints.res_name);
	  XFree(class_hints.res_class);
	}

        return(result);
}

