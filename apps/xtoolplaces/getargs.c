/*
 * Implements the getargs routines used by the xtoolplaces application.
 *
 * Copyright (c) 1993-1998 Xerox Corporation.  All Rights Reserved.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#include <stdio.h>
#include <pwd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <unistd.h>
#include "xtoolplaces.h"
#include "patchlevel.h"

int num_op_entries = 10;                /*Number of entries in OP table*/
char use_display[256];                  /*Display to use*/
XrmDatabase cmmd_line_db;               /*Command line arguments*/
XrmDatabase database;                   /*Database containing ALL resources*/
XrmOptionDescRec op_table[] = {         /*Argument operation resource table*/
                {"-a", ".addon",     XrmoptionSepArg, (caddr_t) NULL},
                {"-c", ".console",   XrmoptionIsArg,  (caddr_t) NULL},
                {"-display", ".display",   XrmoptionSepArg, (caddr_t) NULL},
                {"-f", ".saveto",    XrmoptionSepArg, (caddr_t) NULL},
		{"-i", ".ignore",    XrmoptionSepArg, (caddr_t) NULL},
		{"-idt", ".ignoredt",XrmoptionIsArg,  (caddr_t) NULL},
                {"-m", ".missing",   XrmoptionSepArg, (caddr_t) NULL},
                {"-r", ".remote",    XrmoptionSepArg, (caddr_t) NULL},
                {"-s", ".screens",   XrmoptionSepArg, (caddr_t) NULL},
                {"-v", ".version",   XrmoptionIsArg,  (caddr_t) NULL},
		{"-w", ".toolwait",  XrmoptionIsArg,  (caddr_t) NULL},
		{"-p", ".toolwaitprog",XrmoptionSepArg,(caddr_t)NULL}
};
extern char *program;
extern Display *display;

char *getenv();

/*This function will gather all the arguments for this program, either through
  the command line or from resource databases.  The order for overriding options
  is as follows:

        1) Hardcoded defaults
        2) Values in /usr/lib/X11/app-defaults/Xtoolplaces
        3) Values in $XUSERFILESEARCHPATH/Xtoolplaces or
           $XAPPLRESDIR/Xtoolplaces
        4) Values set using xrdb, either through the
           XResourceMangerString macro or, if that's empty, the
           .Xdefaults file in the user's home directory
        5) Values in the file specified by the XENVIRONMENT
           environment variable or, if that's not set, the
           .Xdefaults-hostname file in the user's home directory
           (where hostname is the name of the machine)
        6) Command line arguments

  Each order overrides anything before it (i.e. command line arguments override
  everything, while /usr/lib/X11/app-defaults/Xtoolplaces only override
  hardcoded defaults).

  Inputs:  argc - number of command line arguments
           argv - command line arguments
  Outputs: none
  Locals:  none
  Globals: none
*/
void
getargs(argc,argv)
int argc;
char *argv[];
{
        /*Initialize resource manager*/
        XrmInitialize();

        /*Parse command line first so we can open the display*/
        parse_cmmd_args(&argc,argv);

        /*Get server defaults, program defaults, .Xdefaults,
          command line, etc. and merge them*/
        merge_databases();

        /*Extract values from database for use*/
        extract_options();
}

/*This function will parse the command line options to get a display (-d)
  option if one is given.  It will also check for the '-v' option and act
  on it if it's given.

  Inputs:  argc           - number of command line arguments
           argv           - command line arguments
  Outputs: none
  Locals:  resource       - name of resource to find
           str_type       - representation type (ignored)
           value          - value in the database
  Globals: display        - interface info to X display
           cmmd_line_db   - command line arguments in database form
           num_op_entries - number of entries in the OP table
           op_table       - argument operation resource table
           program        - name of currently executing program
           stderr         - standard error
           use_display    - display to use
           PATCHLEVEL     - current program patchlevel
           NULL           - 0
           USAGE          - usage line
           VERSION        - current program version
*/
void
parse_cmmd_args(argc,argv)
int *argc;
char *argv[];
{
        char *resource;
        char *str_type[20];
        XrmValue value;

        /*Allocate space for the names of the 2 resources we'll be looking
          for (the display and version resources)*/
        if((resource = malloc(strlen(program)+9)) == NULL) {
          perror(program);
          exit(1);
        }

        use_display[0] = '\0';

        /*Put command line arguments into database form*/
        XrmParseCommand(&cmmd_line_db,op_table,num_op_entries,argv[0],
                        argc,argv);

        /*Usage error if any arguments left*/
        if(*argc != 1) {
          fprintf(stderr,"usage: %s %s\n",program,USAGE);
          exit(1);
        }

        /*If the -v option is given, just print the version and quit*/
        sprintf(resource,"%s.version",program);
        if(XrmGetResource(cmmd_line_db,resource,"xtoolplaces.version",
           str_type,&value) == True) {
          fprintf(stderr,"%s: version %s, patchlevel %s\n",program,VERSION,
                  PATCHLEVEL);
          exit(0);
        }

        /*Get display now because we need it to get other databases*/
        sprintf(resource,"%s.display",program);
        if(XrmGetResource(cmmd_line_db,resource,"xtoolplaces.display",
           str_type,&value) == True)
          strcpy(use_display,value.addr);

        /*Open a connection to the X server*/
        if(!(display = XOpenDisplay(use_display))) {
          fprintf(stderr,"%s: can't open display (%s)\n",program,
                  XDisplayName(use_display));
          exit(1);
        }

        free(resource);
}

/*This function will get resources from all the standard X resource
  places in the standard X order of looking and merge them into one
  database.  Resources are read in and merged into the global database
  according to the following order:

        1) Values in APPSDIR/app-defaults/Xtoolplaces
	   (where APPSDIR is defined in xtoolplaces.h)
        2) Values in $XUSERFILESEARCHPATH/Xtoolplaces or
           $XAPPLRESDIR/Xtoolplaces
        3) Values set using xrdb, either through the
           XResourceMangerString macro or, if that's empty, the
           .Xdefaults file in the user's home directory
        4) Values in the file specified by the XENVIRONMENT
           environment variable or, if that's not set, the
           .Xdefaults-hostname file in the user's home directory
           (where hostname is the name of the machine)
        5) Command line arguments

  Inputs:  none
  Outputs: none
  Locals:  db           - database of read in resource file
           db_file      - name of resource file to read in
           env_var      - environment variable value
           home         - user's home directory
  Globals: cmmd_line_db - command line arguments in database form
           database     - database containing ALL resources for this application
           display      - interface info to X display
           program      - name of currently executing program
           stderr       - standard error
           APPSDIR      - default resource directory
           APPSFILENAME - class name of this application
           NULL         - 0
*/
void
merge_databases(void)
{
        char *env_var,*home;
        char db_file[1024];
        XrmDatabase db;
        char *gethomedir();

        /*Get application defaults file, if any*/
        sprintf(db_file,"%s/%s",APPSDIR,APPSFILENAME);
        db = XrmGetFileDatabase(db_file);
        XrmMergeDatabases(db,&database);

        /*Merge $XUSERFILESEARCHPATH/Xtoolplaces, if XUSERFILESEARCHPATH
          is defined; otherwise merge $XAPPLRESDIR/Xtoolplaces if it's
          defined*/
        if((env_var = getenv("XUSERFILESEARCHPATH")) == NULL)
          env_var = getenv("XAPPLRESDIR");
        if(env_var) {
          sprintf(db_file,"%s/%s",env_var,APPSFILENAME);
          db = XrmGetFileDatabase(db_file);
          XrmMergeDatabases(db,&database);
        }

        /*Merge server defaults (these are created by xrdb).  If not
          defined, use ~/.Xdefaults*/
        if(XResourceManagerString(display) != NULL)
          db = XrmGetStringDatabase(XResourceManagerString(display));
        else {
               /*Open .Xdefaults file and merge into existing database*/
               if((home = gethomedir()) == NULL) {
                 fprintf(stderr,"%s: can't find home directory\n",program);
                 exit(1);
               }
               sprintf(db_file,"%s/.Xdefaults",home);

               db = XrmGetFileDatabase(db_file);
             }
        XrmMergeDatabases(db,&database);

        /*Open XENVIRONMENT file if defined or the ~/.Xdefaults-hostname file
          if not*/
        if((env_var = getenv("XENVIRONMENT")) == NULL) {
          if((home = gethomedir()) == NULL) {
            fprintf(stderr,"%s: can't find home directory\n",program);
            exit(1);
          }
          sprintf(db_file,"%s/.Xdefaults-",home);

          gethostname(&db_file[strlen(db_file)],1024-strlen(db_file));

        } else strcpy(db_file,env_var);
        db = XrmGetFileDatabase(db_file);
        XrmMergeDatabases(db,&database);

        /*Command line overrides everything, merge that in last*/
        XrmMergeDatabases(cmmd_line_db,&database);
}

/*This function will extract the options out of the database.

  Inputs:  none
  Outputs: none
  Locals:  filename         - name of file to write window info to
           resource         - name of resource to find
           str_type         - representation type (ignored)
           value            - value in the database
  Globals: addon            - name of file containing commands to add on
           console_checking - indicates if the check console flag was given
           database         - database containing ALL resources
	   ignore	    - name of file containing X applications
           list             - option list of screens to save from
           missing          - name of file containing X applications
           program          - name of currently executing program
           remote           - command to use for remote applications
           stderr           - standard error
           NULL             - 0
           True             - 'true' value
*/
void
extract_options(void)
{
        char *resource,*filename,*tw_program;
        char *str_type[20];
        XrmValue value;
        extern int console_checking, ignoredt;
        extern char *addon,*ignore,*missing,*remote,*list;

        /*Allocate space for the names of the resources we'll be looking
          for*/
        if((resource = malloc(strlen(program)+14)) == NULL) {
          perror(program);
          exit(1);
        }

        /*Get addon information (-a)*/
        addon = NULL;
        sprintf(resource,"%s.addon",program);
        if(XrmGetResource(database,resource,"xtoolplaces.addon",
                          str_type,&value) == True)
          if(value.addr && ((addon = strdup(value.addr)) == NULL)) {
            perror(program);
            exit(1);
          }

        /*Get console checking information (-c)*/
        console_checking = 0;
        sprintf(resource,"%s.console",program);
        if(XrmGetResource(database,resource,"xtoolplaces.console",
                          str_type,&value) == True)
          console_checking = 1;

        /*Get filename to save information to (-f)*/
        filename = NULL;
        sprintf(resource,"%s.saveto",program);
        if(XrmGetResource(database,resource,"xtoolplaces.saveto",
                          str_type,&value) == True)
          if(value.addr && ((filename = strdup(value.addr)) == NULL)) {
            perror(program);
            exit(1);
          }
        open_file(filename);

	/*Get list of applications to ignore (-i)*/
	ignore = NULL;
	sprintf(resource,"%s.ignore",program);
	if(XrmGetResource(database,resource,"xtoolplaces.ignore",
			  str_type,&value) == True)
	  if(value.addr && ((ignore = strdup(value.addr)) == NULL)) {
	    perror(program);
	    exit(1);
	  }

        /*Determine if we should ignore CDE dt* programs (-idt)*/
        ignoredt = 0;
        sprintf(resource,"%s.ignoredt",program);
        if(XrmGetResource(database,resource,"xtoolplaces.ignoredt",
                          str_type,&value) == True)
          ignoredt = 1;

        /*Get missing applications list (-m)*/
        missing = NULL;
        sprintf(resource,"%s.missing",program);
        if(XrmGetResource(database,resource,"xtoolplaces.missing",
                          str_type,&value) == True)
          if(value.addr && ((missing = strdup(value.addr)) == NULL)) {
            perror(program);
            exit(1);
          }

        /*Get remote command to use (-r)*/
        remote = DEFAULT_REMOTE;
        sprintf(resource,"%s.remote",program);
        if(XrmGetResource(database,resource,"xtoolplaces.remote",
                          str_type,&value) == True)
          if(value.addr && ((remote = strdup(value.addr)) == NULL)) {
            perror(program);
            exit(1);
          }

        /*Get list of screens to save window information from (-s)*/
        list = NULL;
        sprintf(resource,"%s.screens",program);
        if(XrmGetResource(database,resource,"xtoolplaces.screens",
                          str_type,&value) == True)
          if(value.addr && ((list = strdup(value.addr)) == NULL)) {
            perror(program);
            exit(1);
          }

	/*Get toolwait option (-w)*/
	sprintf(resource,"%s.toolwait",program);
	if(XrmGetResource(database,resource,"xtoolplaces.toolwait",
			  str_type,&value) == True)
	  do_toolwait = 1;

	/*Get program to start applications with (-p)*/
	tw_program = NULL;
	sprintf(resource,"%s.toolwaitprog",program);
	if(XrmGetResource(database,resource,"xtoolplaces.toolwaitprog",
			  str_type,&value) == True)
	  if(value.addr && ((tw_program = strdup(value.addr)) == NULL)) {
	    perror(program);
	    exit(1);
	  }
	if(tw_program)
	  tw_str = tw_program;
	else
	  tw_str = DEFAULT_PROGRAM;
}

/*This function will get the user's home directory, either from the HOME
  environment variable, or if that's not set, based on the USER environment
  variable (going through the password file).  If neither HOME or USER are
  set, the home directory is gotten by getting the user's uid and then going
  through the password file.  If this also fails, a NULL is returned,
  otherwise a valid home directory is returned.

  Inputs:  none
  Outputs: ptr  - user's home directory if found, NULL if not
  Locals:  ptr  - user's home directory
           pw   - password entry of user
           uid  - uid of user
  Globals: HOME - environment variable containing the user's home directory
           NULL - 0
           USER - user's user name
*/
char *gethomedir()
{
        int uid;
        char *ptr;
        struct passwd *pw;

        if((ptr = getenv(HOME)) == NULL) {
          if((ptr = getenv(USER)) == NULL) {
             uid = getuid();
             pw = getpwuid(uid);
          } else pw = getpwnam(ptr);

          if(pw) ptr = pw->pw_dir;
          else ptr = NULL;
        }

        return(ptr);
}

/*This will open the file to save information to.  If the filename isn't
  given on the command line (or resource files) (-f option), then use
  the default name of ~/.defaults.  If '-' is given as a filename, write
  to stdout.

  Inputs:  filename         - name of file to write window info to
  Outputs: none
  Locals:  home             - user's home directory
  Globals: fp               - file pointer to write window information to
           program          - name of currently executing program
           stderr           - standard error
           stdout           - standard output
           DEFAULT_FILENAME - default filename to save to
           NULL             - 0
*/
void
open_file(filename)
char *filename;
{
        char *home;
        char *gethomedir();
        extern FILE *fp;

        /*If filename isn't given, use default ~/.xtoolplaces
          else if filename matches -, write to stdout, else
          open the filename given for writing*/
        if(!filename) {
          /*Get home directory for default filename*/
          if((home = gethomedir()) == NULL) {
            fprintf(stderr,"%s: can't find home directory\n",program);
            exit(1);
          }

          /*Allocate space for home directory, directory slash, and
            default filename*/
          if((filename = malloc(strlen(home)+strlen(DEFAULT_FILENAME)+2))
             == NULL) {
            perror(program);
            exit(1);
          }
            
          strcpy(filename,home);
          strcat(filename,"/");
          strcat(filename,DEFAULT_FILENAME);
        }
          
        if(strcmp(filename,"-")) {
          if((fp = fopen(filename,"w")) == NULL) {
           fprintf(stderr,"%s: can't open {%s} for writing\n",program,filename);           exit(1);
          }
        } else fp = stdout;
}
