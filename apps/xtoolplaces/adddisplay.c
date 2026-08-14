/*
 * Implements the adddisplay routines used by the xtoolplaces application.
 *
 * Copyright (c) 1993-1998 Xerox Corporation.  All Rights Reserved.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#include <stdio.h>
#include "xtoolplaces.h"

extern FILE *fp;

/*This function will add the '-display' to windows that don't already have
  it if xtoolplaces is saving from more than 1 screen.  This is done
  because a user might start an application in 1 screen (so that -display
  isn't needed) and run xtoolplaces from another.  To insure that the
  applications starts up on the right screen, '-display' is added to all
  applications.

  Three assumptions are made:

  1) if a colon (:) is found on the command line, it belongs to
     HOST:SERVER.SCREEN and not to some other option
  2) if a window already has a '-display' option (i.e. a colon was
     found in the command line), it points to the correct screen
     and nothing is changed or added
  3) every window that has '-display' added used '-display' (and
     not '-d' or something else)

  Inputs:  command_line   - command line to see if it already has option or not
           screen_number  - number of the screen this window is on
  Outputs: none
  Locals:  none
  Globals: fp             - file pointer to write window information to
*/
void
adddisplay(command_line,screennum)
int screennum;
char *command_line;
{
        if(!index(command_line,':'))
          fprintf(fp,"-display :0.%d ",screennum);
}
