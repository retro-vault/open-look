/*
 * Declares the xtoolplaces interfaces used by the xtoolplaces application.
 *
 * Copyright (c) 1993-1998 Xerox Corporation.  All Rights Reserved.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

/*User modifiable defines*/
#define APPSDIR "/usr/lib/X11/app-defaults"     /*Default resource directory*/
#define APPSFILENAME "Xtoolplaces"              /*Class name*/
#define CONSOLE_CLASS "XConsole"                /*Class of console windows*/
#define DEFAULT_FILENAME ".xtoolplaces"         /*Default name to save to*/
#define DEFAULT_PROGRAM "xtoolwait"		/*Default toolwait program*/
#define DEFAULT_REMOTE "rsh -n"                 /*Default remote command*/
#define HOME "HOME"                             /*Home environment variable*/
#define MAXAPPS 50				/*Max number in missing & ignore
						  lists*/
#define USER "USER"                             /*User environment variable*/

/*3 popular console terminal windows and the method to identify when
  they are present and in console mode since there is no console class*/
#define CONTOOL_NAME "Contool"                  /*Name given to contool window*/
#define SUN_TOOLS_ICON_NAME "CONSOLE"           /*Icon name for sun consoles*/
#define XTERM_OPTION "-C"                       /*Console option for xterm*/


/*Do not modify anything below this line*/

/*Global definitions*/
#define USAGE "[-a filename] [-c] [-display host:display.screen] [-f filename] [-i filename] [-m filename] [-p program] [-r command] [-s {list | all}] [-v] [-w]"
#define VERSION "1.7.1"                      	/*Version*/

#include <X11/Xos.h>
#include <stdlib.h>
#include <string.h>

extern unsigned int do_toolwait;
extern char *tw_str;
