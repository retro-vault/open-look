/*
 * Implements the fix_command routines used by the xtoolplaces application.
 *
 * Copyright (c) 1993-1998 Xerox Corporation.  All Rights Reserved.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#include <stdio.h>
#include <ctype.h>
#include "xtoolplaces.h"

extern char *program;

/*This routine will fix a problem caused by xview.  This routine is needed
  since xview seems to screw up WM_COMMAND when a command is started using
  the -Wl option (window label) with a string of more than 23 characters.  It
  does this by putting some of the label string after the last number for the
  icon position.  For exmaple, if a WM_SAVE_YOURSELF is sent to xrolo,
  WM_COMMAND is set to:

xrolo -Wp 29 278 -Ws 592 309 -WP 1085 241 1)" -Wl "Rolo - .work  (Card 1 of 1)"

  The ' 1)"' after -WP 1085 241 shouldn't be there.  It seems to do this only
  with WM_COMMAND strings that are a single string (as opposed to xcalctool
  for example which breaks up the WM_COMMAND string where each word is it's
  own string).

  To fix the problem, the offending characters are replaces with spaces.

  Inputs:  string  - WM_COMMAND string
  Outputs: none
  Locals:  pointer - pointer to go through string
  Globals: NULL    - 0
*/
fix_command(string)
char *string;
{
        char *pointer;

        /*See if we have a string with the potential -WP problem*/
        if((pointer = strstr(string,"-WP")) != NULL) {
          /*Get to 2nd icon position*/
          pointer = strstr(pointer," ");
          pointer = strstr(++pointer," ");

          /*Skip over Y icon position, which can consist of numbers or
            the + or - signs*/
          while(isdigit(*(++pointer)) || (*pointer == '+') ||
                (*pointer == '-'))
            ;

          /*Change screwed up characters to spaces*/
          while(*pointer && *pointer != '-' && *pointer != '+')
            *(pointer++) = ' ';
        }
}

/*This routine will fix a problem with # signs.  Colors that are specified
  with hexidecimals, and geometries under xterm, involve the use of # in
  the command line string.  In the resulting .xtoolplaces file, the shell
  will interpret everything after # as a comment unless escaped with \.

  Inputs:  string           - WM_COMMAND string
  Outputs: new_command_line - fixed command line
  Locals:  new_command_line - fixed command line
           numbytes         - number of bytes to put in new command line
           pointer          - pointer to go through string
  Globals: NULL             - 0
	   program          - name of currently executing program
*/
char *fix_command2(string)
char *string;
{
	char *new_command_line;
	int numbytes;
        char *pointer;
    
        numbytes = strlen(string);

        /* Adjust numbytes by number of '#' characters found in string */
        pointer = string;
        while (pointer = strchr(pointer,'#')) {
          numbytes++;
          pointer++;
        }

        /*Allocate space and copy over all arguments except 'argument'*/
        if((new_command_line = malloc(numbytes + 1)) == NULL) {
          perror(program);
          exit(1);
        }

        pointer = strtok (string,"#"); /* Get first token */
        strcpy(new_command_line,pointer);

        /* Loop on tokens */
        while (pointer = strtok(NULL,"#")) {
          strcat(new_command_line,"\\#");
          strcat(new_command_line,pointer);
        }

        free(string);

        return(new_command_line);
}

