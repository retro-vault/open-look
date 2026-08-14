/*
 * Implements the getgeom routines used by the xtoolplaces application.
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
#include "xtoolplaces.h"

char *strip_arg2();
char *strip_arg_nocol();

extern char *program;
extern Display *display;

/*This function will get the window and icon geometry and add them to the
  command line.  For window geometry, it will add position and size.  For
  icon geometry, it will only add position.

  Inputs:  command_line  - command line arguments
           window        - ID of the window that we are going to check
  Outputs: command_line  - command line arguments with geometry info added
  Locals:  adj_x         - X size adjustment
           adj_y         - Y size adjustment
           base_height   - base height of window (needed for xterm)
           base_width    - base width of window (needed for xterm)
           dummy         - dummary variable for return value we don't want
           dummywin      - dummary variable for return value we don't want
           geometry_args - geometry arguments to add to command line
           height        - height of window (in pixels)
           height_inc    - height increment
           hints         - window size hints
           icon_arg      - icon geometry argument
           icon_hints    - icon position hints
           icon_x        - X coordinate of icon
           icon_y        - Y coordinate of icon
           ptr           - pointer to characters within a string
           rearrange     - rearrange args so geometry is first
           root          - root window of this window
           status        - status of get call
           supplied      - flag telling which size hints are available
           tx            - translated X coordinate
           ty            - translated Y coordinate
           width         - width of window (in pixels)
           width_inc     - width increment
           x             - X coordinate of window
           y             - Y coordinate of window
  Globals: display       - interface info to X display
           program       - name of currently executing program
           NULL          - 0
           PResizeInc    - flag bit to check if resize increment hint is avail.
*/
char *getgeom(window,command_line)
char *command_line;
Window window;
{
        int width_inc,height_inc,base_width,base_height;
        int x,y,tx,ty,icon_x,icon_y,adj_x,adj_y;
        unsigned int width,height,dummy;
        long supplied;
        char *icon_arg,*geom_arg,*rearrange,*ptr;
        char geometry_args[100];
        Status status;
        Window root,dummywin;
        XSizeHints hints;
        XWMHints *icon_hints;
        char *strip_geometry(),*strip_xterm_geometry(),*strip_emacs_geometry();

        /*Get icon position*/
        icon_x = icon_y = -1;
        icon_hints = XGetWMHints(display,window);
        if(icon_hints) {
          icon_x = icon_hints->icon_x;
          icon_y = icon_hints->icon_y;
          XFree((caddr_t) icon_hints);
        }

        /*Get window geometry*/
        status = XGetGeometry(display,window,&root,&x,&y,&width,&height,&dummy,
                              &dummy);

        if(!status) return(command_line);

        if(status) {
          width_inc = 1;
          height_inc = 1;
          base_width = 0;
          base_height = 0;
          adj_x = 0;
          adj_y = 0;
          geom_arg = "-geom";
          icon_arg = "-iconGeom ";

          /*Translate coordinate relative to root window*/
          XTranslateCoordinates(display,window,root,0,0,&tx,&ty,&dummywin);

          /*Xterm is a special case (isn't it always?).  Unlike other programs,
            xterm takes it's dimensions in characters, not pixels.  This
            means we have to get it's increment size (i.e. font size) and
            divide the size of the window (in pixels) by it's font size to
            determine it's character size.  In addition, the argument to
            specify icon position is different from normal.  Another addition,
            because you can specify scroll bars and the distance between text
            and scroll bars, we have to subtract the base size from the size
            in pixels BEFORE we divide by the increment size to actually
            figure out the geometry to start it with.*/
          if(is_xterm(window) == 1) {
            /*Get increment (i.e. font size)*/
            status = XGetWMNormalHints(display,window,&hints,&supplied);
            if(!status || !(supplied & PResizeInc)) return(command_line);

            width_inc = hints.width_inc;
            height_inc = hints.height_inc;
            base_width = hints.base_width;
            base_height = hints.base_height;
            icon_arg = "#";

            /*Strip old geometry arguments out of command line (if the
              command line has any geometry arguments)*/
            command_line = strip_xterm_geometry(command_line);
          } else if(is_emacs(window) == 1) {
                   /*Get increment (i.e. font size), like xterm, emacs also
                     takes size in characters, not pixels.  For some reason,
                     the sizes specifed by EMACS 19 is always 2 off what it
                     should be.  Hence, the adjustments.*/
                   status = XGetWMNormalHints(display,window,&hints,&supplied);
                   if(!status || !(supplied & PResizeInc)) return(command_line);

                   width_inc = hints.width_inc;
                   height_inc = hints.height_inc;
                   adj_x = 2;
                   adj_y = 2;
                   icon_x = -1;                         /*No icon arg*/
                   geom_arg = "-geometry";

                   /*Strip old geometry arguments out of command line (if the
                     command line has any geometry arguments)*/
                   command_line = strip_emacs_geometry(command_line);
                 } else command_line = strip_geometry(command_line);

          /*Calctool is special in that it thinks it has icon geometry, and
            it doesn't.  Also, it doesn't set the increment right.*/
          if(is_calctool(window) == 1) {
            icon_x = -1;
            width_inc = height_inc = 1;
          }

          /*Put geometry information together in a string.  Put in icon
            information is there was icon information*/
          if(icon_x == -1)
            sprintf(geometry_args," %s %dx%d+%d+%d",geom_arg,
                    (width - base_width)/width_inc - adj_x,
                    (height - base_height)/height_inc - adj_y,tx-x,ty-y);
          else
            sprintf(geometry_args," %s %dx%d+%d+%d %s+%d+%d",geom_arg,
                    (width - base_width)/width_inc - adj_x,
                    (height - base_height)/height_inc - adj_y,
                     tx-x,ty-y,icon_arg,icon_x,icon_y);

          /*Add window and icon geometry as first arguments*/
          if((rearrange = malloc(strlen(command_line) +
                                 strlen(geometry_args) + 1)) == NULL) {
            perror(program);
            exit(1);
          }
          if(ptr = index(command_line,' ')) {
            *ptr = '\0';
            sprintf(rearrange,"%s%s %s",command_line,geometry_args,ptr + 1);
            *ptr = ' ';
          } else sprintf(rearrange,"%s%s",command_line,geometry_args);
        }

        free(command_line);

        return(rearrange);
}

/*This will strip out the geometry arguments for a normal X application.

  Inputs:  command_line - command line arguments
  Outputs: command_line - command line arguments without geometry info
  Locals:  none
  Globals: none
*/
char *strip_geometry(command_line)
char *command_line;
{
        /*Strip window geometry*/
        command_line = strip_arg2(command_line,"-g");

        /*Strip icon geometry*/
        command_line = strip_arg2(command_line,"-iconGeom");

        return(command_line);
}

/*This will strip out the geometry arguments for an xterm window.

  Inputs:  command_line - command line arguments
  Outputs: command_line - command line arguments without geometry info
  Locals:  none
  Globals: none
*/
char *strip_xterm_geometry(command_line)
char *command_line;
{
        /*Strip window geometry*/
        command_line = strip_arg2(command_line,"-g");

        /*Strip icon geometry*/
        command_line = strip_arg_nocol(command_line);

        return(command_line); 
}

/*This will strip out the geometry arguments for an emacs window.

  Inputs:  command_line - command line arguments
  Outputs: command_line - command line arguments without geometry info
  Locals:  none
  Globals: none
*/
char *strip_emacs_geometry(command_line)
char *command_line;
{
        /*Strip window geometry*/
        command_line = strip_arg2(command_line,"-geometry");
        command_line = strip_arg2(command_line,"-w");

        return(command_line); 
}

/*This function will strip two arguments from a string.

  Inputs:  arg              - argument to start stripping at
           command_line     - command line arguments
  Outputs: new_command_line - command line without 'arg' & following arg
  Locals:  new_command_line - command line without 'arg' & following arg
           numbytes         - number of bytes to copy from old command line
           pointer1         - pointer to start of 'arg'
           pointer2         - pointer to any arguments after 2nd arg
  Globals: program          - name of currently executing program
           NULL             - 0
*/
char *strip_arg2(command_line,arg)
char *command_line,*arg;
{
        int numbytes;
        char *pointer1,*pointer2,*new_command_line;

        /*If the argument to strip isn't here, then just return the original
          string*/
        if((pointer1 = strstr(command_line,arg)) == NULL)
          return(command_line);
        else {
               /*Find next character after argument*/
               pointer2 = pointer1 + strlen(arg);

	       /*The next character after the argument _could* be a
		 space.  Index from pointer2 itself*/
               pointer2 = index(pointer2,' ');        /*After 'arg'*/
               if(pointer2)
                 pointer2 = index(pointer2+1,' ');      /*After 2nd arg*/

               /*Find out how much space to allocate for command line minus
                 the argument to strip*/
               /*Null out byte before argument to be able to use strlen()*/
               *pointer1 = '\0';
               numbytes = strlen(command_line);
               /*Count bytes after argument (if there are any) plus 1 for NULL*/
               /*Have to use pointer2+1 instead of doing ++pointer2 so we
                 can check if pointer2 = NULL (++pointer2 would have it = 1)*/
               if(pointer2) numbytes += strlen(pointer2+1)+1;

               /*Allocate space and copy over all arguments except argument*/
               if((new_command_line = malloc(numbytes + 1)) == NULL) {
                 perror(program);
                 exit(1);
               }
               strcpy(new_command_line,command_line);
               if(pointer2) strcat(new_command_line,pointer2+1);

               /*Restore command_line to it's original condition so we can
                 free up all the space it's using*/
               *pointer1 = '-';
               free(command_line);
             }

        return(new_command_line);
}

/*This function will strip icon geometries.  It does not strip colors.

  Inputs:  command_line     - command line arguments
  Outputs: new_command_line - command line without 'arg'
  Locals:  
	   color	    - integer value of a potential color
	   colorstr	    - reconverted string value for a potential color
	   new_command_line - command line without 'arg'
	   numbytes	    - number of bytes to copy from old command line
	   pointer1	    - pointer to start of 'arg'
	   pointer2	    - pointer to any arguments after arg
	   pointer2val      - whether pointer2 pos is orig null (0) or space (1)
	   pointer3	    - traversal pointer for converting to lower case
  Globals: program	    - name of currently executing program
	   NULL		    - 0
*/
char *strip_arg_nocol(command_line)
char *command_line;
{
	int numbytes,color,pointer2val = 0;
	char *pointer1,*pointer2,*pointer3,*new_command_line;
	char colorstr[7];

	/*If the argument to strip isn't here, then just return the
	  original string*/
	if((pointer1 = strstr(command_line,"#")) == NULL)
	  return(command_line);

	while(pointer1 != NULL) {
	  /*The next character after the argument _could_ be a space,
	    Index from pointer1+1 itself*/
	  pointer2 = index(pointer1 + 1,' ');	/*After 'arg'*/

	  /*Pointer1 is in front of the string, Pointer2 is behind it,
	    or NULL if there are no more spaces.  Make sure our arg
	    ends with a NULL for the moment.*/
	  if(pointer2) {
	    pointer2val = 1;
	    *pointer2 = '\0';
	  } else pointer2val = 0;

	  /*Convert our string to complete lowercase*/
	  pointer3 = pointer1 + 1;

	  while(*pointer3 != '\0') {
	    *pointer3 = tolower(*pointer3);
	    ++pointer3;
	  }

	  /*Convert the argument to decimal and back to hex*/
	  sscanf(pointer1 + 1,"%x",&color);
	  sprintf(colorstr,"%06x",color);

	  /*If we enter this code then the result of our conversion and
	    retroconversion did not match the original string; we may
	    assume it is not a hexidecimal color representation*/
	  if(strcmp(pointer1 + 1,colorstr)) {
	    /*Convert pointer2 back if it was a space*/
	    *pointer2 = ' ';

	    /*Find out how much space to allocate for command line minus
	      the argument to strip.  Null out byte before argument to be
	      able to use strlen()*/
	    *pointer1 = '\0';
	    numbytes = strlen(command_line);

	    /*Count bytes after argument (if there are any) plus 1 for NULL.
	      Have to use pointer2+1 instead of doing ++pointer2 so we
	      can check if pointer2 = NULL (++pointer2 would have it = 1).*/
	    if(pointer2) numbytes += strlen(pointer2 + 1) + 1;

	    /*Allocate space and copy over all arguments except 'argument'*/
	    if((new_command_line = malloc(numbytes + 1)) == NULL) {
	      perror(program);
	      exit(1);
	    }

	    strcpy(new_command_line,command_line);
	    if(pointer2) strcat(new_command_line,pointer2 + 1);

	    /*Restore command_line to its original condition so we can
	      free up all the space it's using*/
	    *pointer1 = '-';
	    free(command_line);

	    return(new_command_line);		/*Bail out*/
	  } else {
		   /*What we found was a color*/

		   /*If pointer2 was a space, change it back*/
		   if(pointer2val) {
		     *pointer2 = ' ';

		     /*Look for another argument starting with #*/
		     pointer1 = index(pointer2,'#');
		   } else pointer1 = NULL;
		 }
	}

	return(command_line);
}
