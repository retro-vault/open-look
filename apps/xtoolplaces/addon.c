/*
 * Implements the addon routines used by the xtoolplaces application.
 *
 * Copyright (c) 1993-1998 Xerox Corporation.  All Rights Reserved.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#include <stdio.h>
#include <ctype.h>
#include "xtoolplaces.h"
#include "addon.h"

ADDON *head = NULL;
extern char *program,*addon;
extern FILE *fp;

/*This function will read in a file containing strings to put onto the
  command line.  This is used when WM_COMMAND isn't complete.  In the case
  of xrolo, the data file is never listed in WM_COMMAND and must be put
  added.  I don't know if this is an XVIEW bug or not.  I haven't seen it
  with any other program (yet).

  Inputs:  none
  Outputs: none
  Locals:  fp      - file pointer to read from
           line    - line of text read in
  Globals: addon   - name of file containing commands to add on
           program - name of currently executing program
           stderr  - standard error
           NULL    - 0
*/
void
read_addon(void)
{
        char line[200];
        FILE *fp;

        /*Open the file for reading*/
        if((fp = fopen(addon,"r")) == NULL) {
          fprintf(stderr,"%s: can't open {%s}\n",program,addon);
          exit(1);
        }

        /*Read lines from file until there is no more*/
        while(fgets(line,200,fp) != NULL)
          switch(line[0]) {
            case '#':                   /*Comment character - ignore line*/
            case '\n':                  /*Blank line - ignore line*/
                        break;
            default:
                        line[strlen(line)-1] = '\0';  /*Take out NL char*/
                        insert(line);

                        break;
          }
}

/*This function will insert commands to be added into a linked list.

  Inputs:  line    - line containing program name and commands to be added
  Outputs: none
  Locals:  next    - next available structure entry to fill in
           pointer - point to : to separater program from commands and to
                     skip over white space to point to commands
  Globals: addon   - name of file containing commands to add on
           head    - pointer to head of linked list
           program - name of currently executing program
           stderr  - standard error
           ADDON   - structure holding program and command line info
           NULL    - 0
*/
void
insert(line)
char *line;
{
        char *pointer;
        ADDON *next;

        /*Find colon to make it a NULL thereby making line equal to just
          the program name.  Syntax error if the colon isn't found*/
        if((pointer = index(line,':')) == NULL) {
          fprintf(stderr,"%s: syntax error in {%s}\n",program,addon);
          exit(1);
        }

        /*Insert new entry*/
        if(!head) {
          if((head = (ADDON *) calloc(1,sizeof(ADDON))) == NULL) {
            perror(program);
            exit(1);
          }

          next = head;
        } else {
                 for(next = head;next->next;next = next->next)
                   ;

                 if((next->next = (ADDON *) calloc(1,sizeof(ADDON))) == NULL) { 
                   perror(program);
                   exit(1);
                 }

                 next = next->next;
               }

        /*Make colon NULL so that line equals just the program name*/
        *pointer = '\0';

        if((next->program = strdup(line)) == NULL) {
          perror(program);
          exit(1);
        }

        /*Put colon back and skip past whitespace to get to first character
          in command line*/
        *(pointer) = ':';
        while(isspace(*(++pointer))) ;

        if((next->cmmd_line = strdup(pointer)) == NULL) {
          perror(program);
          exit(1);
        }

        next->next = (ADDON *) NULL;
}

/*This function will find the corresponding addon commands to the program
  being written out and at the commands to the command line being written
  out.

  Inputs:  name - name of program being written out
  Outputs: none
  Locals:  list    - pointer to go through linked list
           pointer - pointer to make name equal to just the first word in string
  Globals: fp      - file pointer to write window information to
           head    - pointer to head of linked list
           NULL    - 0
*/
void
addto(name)
char *name;
{
        char *pointer;
        ADDON *list;

        /*If there is whitespace in the name string then put in a NULL to
          make name equal to just the first word in the string, otherwise
          assume the string is only made up of one word*/
        if((pointer = index(name,' ')) != NULL)
          *pointer = '\0';

        /*Loop through linked list to match the program information saved with
          the name of the program being written out.  Write out saved addon
          command line when we find a match*/
        for(list = head;list;list = list->next)
          if(!strcmp(name,list->program)) {
            fprintf(fp,"%s ",list->cmmd_line);
            break;
          }

        /*Put whitespace back to put the string back to normal*/
        if(pointer) *pointer = ' ';
}
