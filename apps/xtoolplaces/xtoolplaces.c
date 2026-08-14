/*
 * Implements the xtoolplaces desktop state helper used by the XView app suite.
 *
 * Copyright (c) 1993-1998 Xerox Corporation.  All Rights Reserved.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#include <stdio.h>
#include <sys/time.h>
#include "xtoolplaces.h"

int console_checking;                   /*Console checking flag*/
unsigned int do_toolwait = 0;		/*Whether to use xtoolwait*/
int ignoredt;				/*Whether to ignore CDE dt* programs*/
char *addon;                            /*File containing stuff to add*/
char *ignore;				/*File containing apps to ignore*/
char *ilist[MAXAPPS];			/*List of X apps to ignore*/
int inumvals;				/*Number of apps in ilist*/
char *list;                             /*List of screens to save*/
char *missing;                          /*File containing apps to save*/
char *mlist[MAXAPPS];			/*List of X apps that need to be saved*/
int mnumvals;				/*Number of apps in mlist*/
char *remote;                           /*Command to use for remote apps*/
char *program;                          /*Program name*/
char *tw_str;				/*Name of alternative toolwait program*/
FILE *fp;                               /*File pointer to write to*/

/*This program will write the current state of every window on an X desktop to
  a file (or stdout).  It is the X equivalent to Sunview's toolplaces program.


  Inputs:  argc    - number of command line arguments
           argv    - command line arguments
  Outputs: 0
  Locals:  none
  Globals: addon   - name of file containing commands to add on
           fp      - file pointer to write window information to
           missing - name of file containing X applications
           program - name of currently executing program
           stdout  - standard output
*/
int
main(argc,argv)
int argc;
char *argv[];
{
        char *pr_ptr;

	if((pr_ptr = rindex(argv[0],'/')) != NULL) {
	  program = pr_ptr + 1;
	  argv[0] = program;	/*So that XrmParseCommand() works right*/
	} else program = argv[0];

        /*Get and parse arguments and resources*/
        getargs(argc,argv);

        /*If -a option given, read in addon file*/
        if(addon) read_addon();

	/*If -i option given, read in ignore file*/
	if(ignore) readfile(ignore, ilist, &inumvals);
 
        /*If -m option given, read in missing apps file*/
        if(missing) readfile(missing, mlist, &mnumvals);
 
        /*Get application information*/
        getinfo();

        /*If we were writing to a file (as opposed to stdout), close the file*/
        if(fp != stdout) fclose(fp);

        exit(0);
}

/*This function will write out the header for a Bourne shell script.  It will
  also write out the current date and time.

  Inputs:  none
  Outputs: none
  Locals:  date - current date and time
           tp   - date & time info structure
  Globals: fp   - file pointer to write window information to
           NULL - 0
*/
void
write_header(void)
{
        char *date;
        struct timeval tp;

        /*Get current date & time*/
        gettimeofday(&tp,NULL);
        date = asctime(localtime(&tp.tv_sec));

        fputs("#!/bin/sh\n",fp);
        fputs("#\n",fp);
        fprintf(fp,"# Created by 'xtoolplaces' on %s",date);
        fputs("#\n\n",fp);
}
