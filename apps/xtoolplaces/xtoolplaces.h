/*
 * Declares the xtoolplaces interfaces used by the xtoolplaces application.
 *
 * Copyright (c) 1993-1998 Xerox Corporation.  All Rights Reserved.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef XTOOLPLACES_H
#define XTOOLPLACES_H

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
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern unsigned int do_toolwait;
extern char *tw_str;

void adddisplay(char *command_line, int screen_number);
void read_addon(void);
void insert(char *line);
void addto(char *name);
char *combine(int argc, char **argv);
void fix_command(char *string);
char *fix_command2(char *string);
void getargs(int argc, char **argv);
void parse_cmmd_args(int *argc, char **argv);
void merge_databases(void);
void extract_options(void);
char *gethomedir(void);
void open_file(char *filename);
char *getgeom(Window window, char *command_line);
char *strip_geometry(char *command_line);
char *strip_xterm_geometry(char *command_line);
char *strip_emacs_geometry(char *command_line);
char *strip_arg2(char *command_line, char *argument);
char *strip_arg_nocol(char *command_line);
void getinfo(void);
int is_save_yourself(Atom *protocols, int count);
int isnull(Window window);
int is_console(Window window, char *command_line);
int check_contool(Window window);
int check_sun_tools(Window window);
int check_xterm(Window window, char *command_line);
int is_xterm(Window window);
int is_dtterm(Window window);
int is_dtapp(Window window);
int is_1x1(Window window);
int is_emacs(Window window);
int is_calctool(Window window);
void is_remote(Window window);
char *getname(Window window);
int *make_list(int default_screen, int total_screens);
int count_spaces(char *string);
void readfile(char *file, char **list, int *index);
int on_list(Window window, char **list, int count);
char *lower(char *string);
char *get_emacs_name(char *name);
void write_header(void);

#endif
