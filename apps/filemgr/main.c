#ifndef lint
static char sccsid[]="@(#)main.c	1.214 07/02/97 Copyright (c) 1987-1992 Sun Microsystems, Inc." ;
#endif

/*  Copyright (c) 1987-1992 Sun Microsystems, Inc.
 *  All Rights Reserved.
 *
 *  Sun considers its source code as an unpublished, proprietary
 *  trade secret, and it is available only under strict license
 *  provisions.  This copyright notice is placed here only to protect
 *  Sun in the event the source is deemed a published work.  Dissassembly,
 *  decompilation, or other means of reducing the object code to human
 *  readable form is prohibited by the license agreement under which
 *  this code is provided to the user or company in possession of this
 *  copy.
 *
 *  RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the
 *  Government is subject to restrictions as set forth in subparagraph
 *  (c)(1)(ii) of the Rights in Technical Data and Computer Software
 *  clause at DFARS 52.227-7013 and in similar clauses in the FAR and
 *  NASA FAR Supplement.
 */

#include <stdio.h>
#include <ctype.h>
#include <values.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <link.h>
/* Linux portability definitions are in linux_port.h. */
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pwd.h>
#include <grp.h>

#ifdef __STDC__
#include <stdarg.h>
#endif /*__STDC__*/

#ifdef SVR4
/* Linux portability definitions are in linux_port.h. */
#include <sys/statvfs.h>
/* Linux portability definitions are in linux_port.h. */
#include <dirent.h>
#include <netdb.h>
/* Linux portability definitions are in linux_port.h. */
#include <rpcsvc/rquota.h>
#else
#include <mntent.h>
#include <sys/vfs.h>
#include <sun/dkio.h>
#include <a.out.h>
#endif /*SVR4*/

#include <rpc/rpc.h>
#include <rpc/pmap_clnt.h>
#include <sys/socket.h>
#include <nfs/nfs.h>
#include <rpcsvc/mount.h>

#include <xview/notice.h>
#include <xview/alert.h>
#include <xview/tty.h>
#include <xview/textsw.h>
#include <xview/server.h>
#include <xview/notify.h>
#include <xview/seln.h>
#include <xview/icon.h>
#include <xview/win_screen.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "defs.h"
#include "fm.h"
#include "xdefs.h"
#include "patchlevel.h"
#include "tree.h"
#include <X11/xpm.h>
#include "ds_colors.h"
#include "ds_popup.h"
#include "ds_xlib.h"

#include "support/ce_err.h"

FmVars Fm ;
FMXlib X ;
extern char *Str[] ;

const unsigned short panning_image[] = {
#include "images/panning.cursor"
} ;

static const unsigned short Dir_list_bits[] = {
#include "images/folder_list.icon"
} ;
static const unsigned short Doc_list_bits[] = {
#include "images/doc_list.icon"
} ;
static const unsigned short App_list_bits[] = {
#include "images/app_list.icon"
} ;

static const int inith[] = { 391, 461, 531, 709 } ;  /* Initial default heights. */
static const int initw[] = { 480, 640, 640, 960 } ;  /* Initial default widths. */

extern char *defaults_get_string() ;

Rb_tree *IconTree = NULL ;         /* Icon root */

const char *ERRFILE   = "SUNW_DESKSET_FM_ERR" ;
const char *LABELFILE = "SUNW_DESKSET_FM_LABEL" ;
const char *MSGFILE   = "SUNW_DESKSET_FM_MSG" ;

static Image_Ptr create_icon_entry      P((char *)) ;
static Image_Ptr get_generic_icon       P((int, int)) ;
static Image_Ptr get_generic_list_icon  P((int, int)) ;
static Image_Ptr get_icon               P((File_Pane_Object *, int)) ;

static int strwidth                     P((register WCHAR *)) ;
static int xview_error_proc             P((Xv_object, Attr_avlist)) ;

#ifdef SVR4
static Notify_value run_child P((Notify_client, int, int *, void *)) ;
#else
static Notify_value run_child P((Notify_client, int, union wait *, void *)) ;
#endif /*SVR4*/

static Notify_value destroy_tree        P((Frame, Destroy_status)) ;
static Notify_value rename_panel_interpose_proc  P((Panel, Event *,
                                        Notify_arg, Notify_event_type)) ;
static Notify_value wb_quit             P((Frame, Destroy_status)) ;

static Server_image server_image        P((const unsigned short *, int)) ;

static void fio_set_message_value       P((Panel_item, char *, int)) ;
static void fio_set_text_value          P((Panel_item, char *, int)) ;
static void fm_el_paste                 P((Xv_opaque, int, enum item_type)) ;
static void fm_exec_cmd_run             P((Panel_item, Event *)) ;
static void fm_exec_your_cmd            P((Menu, Menu_item)) ;
static void fm_folder_join              P((Xv_Window)) ;
static void fm_folder_split             P((Xv_Window, Xv_Window, int)) ;
static void fm_tree_join                P((Xv_Window)) ;
static void fm_tree_split               P((Xv_Window, Xv_Window, int)) ;
static void open_item                   P((Event_Context, Dnd)) ;
static void set_global_vars             P(()) ;
static void x_error_proc                P((Display *, XErrorEvent *)) ;

static XPixel convert_color_string         P((char *)) ;

static Xv_Cursor ds_get_predefined_cursor  P((int)) ;
static void check_icon_font 		P(()) ;

static int fm_handle_xio_errors(display)
    Display *display;
{
    yield_shelf();
    return xv_handle_xio_errors(display);
}

/* Use installed defaults and make staged context help visible to XView. */
static int
setup_environment(void)
{
    const char *prefix = getenv("OPENWINHOME");
    const char *existing_help = getenv("HELPPATH");
    char *help_path;
    size_t length;
    int result;

    if (!prefix || !*prefix) {
        if (setenv("OPENWINHOME", OPENWINHOME_DEFAULT, 1) != 0)
            return -1;
        prefix = OPENWINHOME_DEFAULT;
    }
    length = strlen(prefix) + sizeof("/lib/help:") +
             (existing_help ? strlen(existing_help) : 0);
    help_path = malloc(length);
    if (!help_path) return -1;
    snprintf(help_path, length, "%s/lib/help%s%s", prefix,
             existing_help ? ":" : "", existing_help ? existing_help : "");
    result = setenv("HELPPATH", help_path, 1);
    free(help_path);
    return result;
}

int
main(argc, argv)
int argc ;
char **argv ;
{
  char dispenv[MAXLINE], domain_path[512], *path, *ptr ;
  int fd, i, info[MAXINFO], psinfo[MAXINFO] ;

#ifdef START_POPUP
  time_check() ;          /* Only time-bomb the newstop version. */
#endif /*START_POPUP*/

  if (setup_environment() != 0) {
    perror("filemgr: environment");
    return EXIT_FAILURE;
  }
  ds_expand_pathname("$OPENWINHOME/lib/locale", domain_path) ;
  bindtextdomain(MSGFILE,   domain_path) ;
  bindtextdomain(LABELFILE, domain_path) ;
  bindtextdomain(ERRFILE,   domain_path) ;

  Fm = (FmVars) LINT_CAST(fm_malloc(sizeof(Fm_Object))) ;
  Fm->appname = NULL ;
  if ((ptr = strrchr(argv[0], '/')) != NULL)
    read_str(&Fm->appname, ptr+1) ;
  else read_str(&Fm->appname, argv[0]) ;

/*  Search through all the command line arguments looking for ones to do
 *  with window size, position or state (open/iconic). If found, then ignore
 *  users possible saved window information in their ~/.desksetdefaults.
 *  Note that this has to be done here, because xv_init() will strip them
 *  all away.
 */

  Fm->pos_or_size = check_ps_args(argc, argv, psinfo) ;

/*  Search through all the command line arguments, looking for -name.
 *  If it's present, then this name with be used, when looking for X resources
 *  for this application. When the rest of the command line arguments are
 *  checked later on, then the -name argument (if found) is ignored.
 */

  for (i = 0; i < argc; i++)
    if (EQUAL(argv[i], "-name"))
      {
        if ((i+1) > argc) usage(Fm->appname) ;
        read_str(&Fm->appname, argv[i+1]) ;
        break ;
      }

  xv_init(XV_USE_LOCALE,         TRUE,
          XV_INIT_ARGC_PTR_ARGV, &argc, argv,
          XV_ERROR_PROC,         xview_error_proc,
          XV_X_ERROR_PROC,       x_error_proc,
          0) ;

  Fm->multi_byte = FALSE ;

	(void) XSetIOErrorHandler(fm_handle_xio_errors);

  X = (FMXlib) LINT_CAST(fm_malloc((unsigned) sizeof(XObject))) ;
  X->base_frame = (Frame) xv_create(XV_NULL,    FRAME,
         			    FRAME_WM_COMMAND_ARGC_ARGV, 0, -1,
                                    XV_SHOW, FALSE,
                                    0) ;

  resize_arrays(TRUE, MAXWIN) ;
  resize_X_arrays(TRUE, MAXWIN) ;
  resize_history(TRUE, FM_MAXHISTORY) ;

  Fm->wno_key       = make_key() ;
  Fm->goto_key      = make_key() ;
  Fm->goto_path_key = make_key() ;

  X->server    = XV_SERVER_FROM_WINDOW(X->base_frame) ;
  X->panpix    = 0 ;
  X->display   = (Display *) xv_get(X->base_frame, XV_DISPLAY) ;
  X->visual    = (Visual *)  xv_get(X->base_frame, XV_VISUAL) ;

  Fm->screen_width  = DisplayWidth(X->display,  DefaultScreen(X->display)) ;
  Fm->screen_height = DisplayHeight(X->display, DefaultScreen(X->display)) ;

/* Fixup possible negative positional values. */

  if (Fm->pos_or_size)
    {
      if (psinfo[(int) FM_INFO_XOPEN] < -1)
        psinfo[(int) FM_INFO_XOPEN] =
          Fm->screen_width + psinfo[(int) FM_INFO_XOPEN] -
                             psinfo[(int) FM_INFO_WIDTH] ;
      if (psinfo[(int) FM_INFO_YOPEN] < -1)
        psinfo[(int) FM_INFO_YOPEN] =
          Fm->screen_height + psinfo[(int) FM_INFO_YOPEN] -
                              psinfo[(int) FM_INFO_HEIGHT] ;
    }

  SPRINTF(dispenv, "DISPLAY=%s", DisplayString(X->display)) ;
  read_str(&Fm->display_name, dispenv) ;
  putenv(Fm->display_name) ;

/* Is this a color monitor? */

  i = (int) xv_get(X->base_frame, WIN_DEPTH) ;
  if (i < 8) Fm->color = FALSE ;
  else       Fm->color = TRUE ;

  init_filemgr() ;            /* Initialise the filemgr object structure. */

  for (i = 0; i < MAXDEFS; i++) set_default(i) ;

  load_resources() ;          /* Get resources from various places. */
  read_resources() ;          /* Read resources from merged database. */
  get_options(argc, argv) ;   /* Extract command line options. */

  set_home() ;                /* Store real value of $HOME. */
  read_user_goto_menu() ;     /* Get goto menu entries. */
  make_pipe() ;               /* Open named pipe for vold CD notification. */
  create_fm_dirs() ;          /* Create .fm directory (if not there). */
  set_global_vars() ;         /* Setup global variables. */
  init_ce(Fm->use_ce) ;       /* Connect with the classing engine. */
  fm_font_and_sv_init() ;
  fm_create_menus() ;
  if (Fm->treeview) fm_create_tree_canvas() ;
  init_gcs() ;
  make_busy_cursor() ;
  make_panning_cursor() ;
  fm_tree_font_and_color() ;
  fm_set_folder_colors() ;
  init_atoms() ;              /* For new dnd events. */

  fm_init_font() ;
  fm_init_path_tree(WNO_TREE) ;

  check_icon_font();
  if (Fm->confirm_delete)
    FM_CREATE_WASTEBASKET() ;   /* Only create if user wants it. */

  Fm->num_win = 1 ;             /* Irrespective of whether there is a waste. */

  for (i = 0; i < MAXINFO; i++) info[i] = -1 ;
  if (Fm->pos_or_size)
    for (i = 0; i < MAXINFO; i++) info[i] = psinfo[i] ;

  if (!Fm->pos_or_size && Fm->dirpath == NULL && Fm->no_saved_wins)
    read_window_positions(Fm->no_saved_wins) ;
  else
    {
           if (Fm->dirpath)                    path = Fm->dirpath ;
	else if( Fm->pos_or_size && Fm->no_saved_wins == 1){
	  char  str[MAXPATHLEN];
	  int n = WIN_SPECIAL + 1;
		if (get_str_win_info(n, DGET("Path"), str) &&
               access(str, F_OK) == 0) path = str ;
	}

      else if ((path = getenv("PWD")) == NULL) path = (char *) Fm->home ;
      Fm->no_start_win = TRUE ;
	if( Fm->pos_or_size && Fm->no_saved_wins == 1)
      	    FM_NEW_FOLDER_WINDOW("", path, NULL, FM_DISK, 0, TRUE, info) ;
	else
      	    FM_NEW_FOLDER_WINDOW("", path, NULL, FM_DISK, 0, FALSE, info) ;
    }
  Fm->pos_or_size = FALSE ;


  if (Fm->treeview) do_show_tree(FALSE, TRUE) ;

  Fm->running   = TRUE ;
  fm_name_icon(X->fileX[Fm->curr_wno]->frame,
               (char *) Fm->file[Fm->curr_wno]->path) ;

  Fm->volmgt = TRUE;	/* fm_verify_volmgt() will correct if untrue */
  if (fm_verify_volmgt() && Fm->show_cd)
    check_media(FM_CD, MAXCD, &Fm->cd_mask, 0);

  garbage_collect() ;
  sync_server() ;

#ifdef START_POPUP
  start_popup() ;
#endif /*START_POPUP*/

  if (Fm->confirm_delete)
  	set_frame_attr(WASTE, FM_FRAME_SHOW, TRUE) ;
  start_tool() ;
  exit(0) ;
/*NOTREACHED*/
}

static void
check_icon_font()
{
  char *tempstr ;

  if (Fm->iconfont == NULL || Fm->iconfont[0] == '\0')
	if (defaults_exists("icon.font.name", "Icon.Font.Name")) {
      		tempstr = (char *) defaults_get_string("icon.font.name",
                                             "Icon.Font.Name", "") ;
      		read_str(&Fm->iconfont, tempstr) ;
    	}
}

void
check_args(wno)
int wno ;
{
  Icon frame_icon ;
  char *tempstr ;
  int height, scale, width ;

/*  If the user hasn't defined a specific size (width or height) at startup
 *  time, then we need to provide that, dependant upon the current scale.
 */

  width = -1 ;
  if (defaults_exists("window.width", "Window.Width"))
    width = defaults_get_integer("window.width", "Window.Width", -1) ;

  height = -1 ;
  if (defaults_exists("window.height", "Window.Height"))
    height = defaults_get_integer("window.height", "Window.Height", -1) ;

  scale = S_MEDIUM ;
  if (defaults_exists("window.scale", "Window.Scale"))
    {
      tempstr = (char *) defaults_get_string("window.scale",
                                             "Window.Scale", "medium") ;

           if (EQUAL(tempstr, "small"))       scale = S_SMALL ;
      else if (EQUAL(tempstr, "medium"))      scale = S_MEDIUM ;
      else if (EQUAL(tempstr, "large"))       scale = S_LARGE ;
      else if (EQUAL(tempstr, "extra_large")) scale = S_EXTRALARGE ;
    }

  if (width < 0)  XV_SET(X->fileX[wno]->frame, XV_WIDTH,  initw[scale], 0) ;
  if (height < 0) XV_SET(X->fileX[wno]->frame, XV_HEIGHT, inith[scale], 0) ;

  frame_icon = (Icon) xv_get(X->fileX[wno]->frame, FRAME_ICON) ;

  if (defaults_exists("icon.footer", "Icon.Footer"))
    {
      tempstr = (char *) defaults_get_string("icon.footer", "Icon.Footer", "") ;      if (tempstr == NULL || *tempstr == '\0') tempstr = " " ;
      read_str(&Fm->iconlabel, tempstr) ;
      XV_SET(frame_icon, XV_LABEL, Fm->iconlabel, 0) ;
    }

  check_icon_font();

  if (!defaults_exists("icon.pixmap", "Icon.Pixmap"))
    XV_SET(X->fileX[wno]->frame, FRAME_ICON, frame_icon, 0) ;
  else
    {
      tempstr = (char *) defaults_get_string("icon.pixmap",
                                             "Icon.Pixmap", "") ;
      if (tempstr == NULL || *tempstr == '\0' || access(tempstr, R_OK) == -1)
        {
          fm_msg(TRUE, Str[(int) M_NO_IFNAME], tempstr) ;
          xv_usage(Str[(int) M_FILEMGR]) ;
          exit(1) ;
        }
    }
}


void
create_frame_drop_site(frame, wno)    /* Create a filemgr frame drop site. */
Frame frame ;
int wno ;
{
  Xv_drop_site ds = 0 ;
  Rect *r ;

  r = (Rect *) xv_get((Icon) xv_get(X->fileX[wno]->frame, FRAME_ICON),
                       ICON_IMAGE_RECT) ;

/*  When iconic, a dnd drop will be forwarded to this drop site
 *  (the file pane drop site cannot be used since the window manager
 *  delivers improper X & Y locations on iconic drops).  Since the
 *  drop site is created with the frame as a parent, the event will
 *  be delivered to the frame_event proc, which will in turn send
 *  it to the canvas_event (file pane event) proc.
 */

  ds = (Xv_drop_site) xv_create(frame,           DROP_SITE_ITEM,
                        DROP_SITE_ID,            wno,
                        DROP_SITE_EVENT_MASK,    DND_MOTION | DND_ENTERLEAVE,
                        DROP_SITE_DEFAULT,       TRUE,
                        DROP_SITE_DELETE_REGION, NULL,

/* Some region must be set for this to work. */

                        DROP_SITE_REGION,        &r,
                        0) ;

  if (!ds) FPRINTF(stderr, Str[(int) M_NO_FDROP], wno) ;
}


/*  Intercept XView error calls.
 *
 *  Note: you cannot use the name "xv_error_proc"
 */

/*ARGSUSED*/
static int
xview_error_proc(object, avlist)
Xv_object object ;
Attr_avlist avlist ;
{
  Attr_avlist attrs ;
  Error_severity severity = ERROR_RECOVERABLE ;

  if (Fm->debug)
    FPRINTF(stderr, "\nXView Error (Intercepted)\n") ;

  for (attrs = avlist; *attrs ; attrs = attr_next(attrs))
    {
      switch((int) attrs[0])
        {
          case ERROR_BAD_ATTR :
            if (Fm->debug)
              FPRINTF(stderr, "Bad Attribute:%s\n", attr_name(attrs[1])) ;
            break ;

          case ERROR_BAD_VALUE :
            if (Fm->debug)
              FPRINTF(stderr, "Bad Value (0x%lx) for attribute: %s\n",
                              attrs[1], attr_name(attrs[2])) ;
            break ;

          case ERROR_INVALID_OBJECT :
            if (Fm->debug)
              FPRINTF(stderr, "Invalid Object: %s\n", (char *) attrs[1]) ;
            break ;

          case ERROR_STRING :
            {
              char *c = (char *) attrs[1] ;

              if (c[strlen(c)] == '\n')
                c[strlen(c)] = '\0' ;
              FPRINTF(stderr, "Error: %s\n", (char *) attrs[1]) ;
            }
          break ;

          case ERROR_PKG :
            if (Fm->debug)
              FPRINTF(stderr, "Package: %s\n", ((Xv_pkg *) attrs[1])->name) ;
            break ;

          case ERROR_SEVERITY : severity = attrs[1] ;
                                break ;

          default :
            if (Fm->debug)
              FPRINTF(stderr, "Unknown XView error Attribute (%s)\n",
                      (char *) attrs[1]) ;
        }
    }

/* If a critical error or debugging, then core dump. */

  if (severity == ERROR_NON_RECOVERABLE || Fm->debug)
    {
      char *cwd ;    /* Current working directory. */

      if (!Fm->debug) exit(1) ;
      if (severity == ERROR_NON_RECOVERABLE)
        FPRINTF(stderr, "Non-recoverable Error: \ndumping core file") ;
      else
        FPRINTF(stderr, "Debug mode: Recoverable Error: \ndumping core file") ;

      cwd = (char *) getcwd(NULL, MAXPATHLEN) ;
      if (cwd) FPRINTF(stderr, " in %s\n", cwd ) ;
      else     FPRINTF(stderr, "...\n") ;

      abort() ;
    }
  else
    {
      if (Fm->debug)
        FPRINTF(stderr, "Recoverable Error: continuing...\n") ;
      return(XV_OK) ;
    }
/*NOTREACHED*/
}


static void
x_error_proc(display, error)
Display *display ;
XErrorEvent *error ;
{
  char msg[80] ;
  char *cwd ;    /* Current working directory. */

/*  There are two types of error handlers in Xlib; one to handle fatal
 *  conditions, and one to handle error events from the event server.
 *  This function handles the latter, and aborts with a core dump so
 *  we can determine where the X Error really occurred.
 *
 *  Also: you cannot directly or indirectly perform any operations on the
 *  server while in this error handler.  This means no updating the status
 *  line of the filemgr;  instead send messages to stderr.
 */

  XGetErrorText(display, error->error_code, msg, 80) ;
  FPRINTF(stderr, DGET("\nX Error (intercepted): %s\n"), msg) ;
  FPRINTF(stderr, DGET("Major Request Code   : %d\n"), error->request_code) ;
  FPRINTF(stderr, DGET("Minor Request Code   : %d\n"), error->minor_code) ;
  FPRINTF(stderr, DGET("Resource ID (XID)    : %lu\n"), error->resourceid) ;
  FPRINTF(stderr, DGET("Error Serial Number  : %lu\n"), error->serial) ;

  if (Fm->debug)
    {
      FPRINTF(stderr, "Debug mode: dumping core file") ;

      cwd = (char *) getcwd(NULL, MAXPATHLEN) ;
      if (cwd) FPRINTF(stderr, " in %s\n", cwd) ;
      else     FPRINTF(stderr, "...\n") ;

      abort() ;
    }
  else FPRINTF(stderr, "Ignoring X error...\n") ;
}


void
version(progname)
char *progname ;
{
  unsigned short cvn, rvn ;     /* compiled & runtime version number */
  char *rvs ;                   /* compiled & runtime version string */
  int cmajor, cminor, cdot, cpatch ;
  int rmajor, rminor, rdot, rpatch ;
  char buf[16] ;

  cvn = XV_VERSION_NUMBER ;
  SPRINTF(buf, "%d", cvn) ;
  cmajor = (int) (buf[0] - 48) ;   /* Subtract 48 to get from ascii to int. */
  cminor = (int) (buf[1] - 48) ;
  cdot   = (int) (buf[2] - 48) ;
  cpatch = (int) (buf[3] - 48) ;

  rvn = xv_version_number ;
  SPRINTF(buf, "%d", rvn) ;
  rmajor = (int) (buf[0] - 48) ;   /* Subtract 48 to get from ascii to int. */
  rminor = (int) (buf[1] - 48) ;
  rdot   = (int) (buf[2] - 48) ;
  rpatch = (int) (buf[3] - 48) ;

  rvs = strdup(xv_version_string) ;

  FPRINTF(stderr, "%s version %d.%d.%d rev %d running on %s\n",
          progname, cmajor, cminor, cdot, PATCHLEVEL, rvs ? rvs : "ERROR") ;
  if (cvn != rvn)
    {
      FPRINTF(stderr, "warning: compiled under %d.%d.%d.%d",
              cmajor, cminor, cdot, cpatch) ;
      FPRINTF(stderr, ", running under %d.%d.%d.%d\n",
              rmajor, rminor, rdot, rpatch) ;
    }
  exit(0) ;
}


static void
set_global_vars()
{
  char *buf ;
  extern Rb_tree *IconTree ;
  extern Comparison compare_str() ;

/* Check for placement of scrollbar. */

  buf = defaults_get_string("openwindows.scrollbarplacement",
                            "OpenWindows.ScrollbarPlacement", "right") ;

  if ((*buf == 'L') || (*buf == 'l')) Fm->left_scrollbar = TRUE ;
  else                                Fm->left_scrollbar = FALSE ;

  Fm->black = (unsigned long) xv_get((Cms) xv_get(X->base_frame, WIN_CMS),
                                                  CMS_FOREGROUND_PIXEL) ;
  Fm->white = (unsigned long) xv_get((Cms) xv_get(X->base_frame, WIN_CMS),
                                                  CMS_BACKGROUND_PIXEL) ;

/* Setup an error file to write into. */
  if (fm_run_str("/bin/rm -f /tmp/.FM*", FALSE))
    fm_showerr("/bin/rm -f /tmp/.FM*") ;

  SPRINTF(Fm->err_file, "/tmp/.FM%d", getpid()) ;

/* Save the name of the host we are on. */

  gethostname(Fm->hostname, MAXHOSTNAMELEN) ;

/* Create the global icon tree pointer. */

  IconTree = rb_create(get_icon_name, compare_str) ;
}


Notify_value
Fm_frame_event(frame, event, arg, type)     /* Catch key events. */
Frame frame ;
Event *event ;
Notify_arg arg ;
Notify_event_type type ;
{
  Tree_Pane_Object *t_p ;
  XEvent *xev ;
  int i ;
  int wno    = (int) xv_get(frame, XV_KEY_DATA, Fm->wno_key) ;
  char *path = (char *) Fm->file[wno]->path ;

  X->event = event ;
  switch (event_action(event))
    {
      case WIN_REPARENT_NOTIFY : if (wno != WASTE) fm_name_icon(frame, path) ;
                                 break ;

      case ACTION_CLOSE : Fm->openwins-- ;

/* Turn off the timer if all windows are closed. */

                          if (!Fm->openwins) set_timer(0) ;

/* Update the icon label. */

                          if (wno != WASTE) fm_name_icon(frame, path) ;
                          t_p = path_to_node(WNO_TREE, (WCHAR *) path) ;
                          set_tree_icon(t_p, FALSE) ;
                          if (Fm->treeview)
                            draw_folder(WNO_TREE, t_p, FT_DIR, FALSE) ;
                          for (i = 0; i < MAXPOPUPS; i++)
                            if (Fm->popup_wno[i] == wno)
                              {
                                XV_SET(X->items[Fm->popup_item[i]],
                                       FRAME_CMD_PUSHPIN_IN, FALSE,
                                       0) ;
                                XV_SET(X->items[Fm->popup_item[i]],
                                     XV_SHOW, FALSE,
                                     0) ;
                              }
                         break ;

      case ACTION_OPEN : Fm->openwins++ ;
                         set_timer(Fm->interval) ;
                         t_p = path_to_node(WNO_TREE, (WCHAR *) path) ;
                         set_tree_icon(t_p, TRUE) ;
                         if (Fm->treeview)
                           draw_folder(WNO_TREE, t_p, FT_DIR_OPEN, FALSE) ;
                         for (i = 0; i < MAXPOPUPS; i++)
                           if (Fm->popup_wno[i] == wno)
                             {
                               XV_SET(X->items[Fm->popup_item[i]],
                                    XV_SHOW, TRUE,
                                    0) ;
                               XV_SET(X->items[Fm->popup_item[i]],
                                       FRAME_CMD_PUSHPIN_IN, TRUE,
                                       0) ;
                             }
                         break ;

      case ACTION_DRAG_COPY :       /* New style dnd drop event. */
      case ACTION_DRAG_MOVE :       /* New style dnd drop event. */
        {
          Event_Context ec ;

/* Create/initialize event context. */

          ec = setup_event_context(frame, event, FALSE) ;

/*  Set x & y so next routine will not accidentally find the
 *  drop was over some item.
 */

          ec->x = ec->y = -1 ;
          do_new_dnd_drop(ec) ;
          break ;
        }

      case ACTION_DRAG_PREVIEW :    /* New style dnd preview events */
        break ;                     /* Ignore. */

      case ACTION_DRAG_LOAD :       /* Old style dnd drop event. */
        break ;                     /* Ignore. */


      case WIN_VISIBILITY_NOTIFY :
        xev = (XEvent *) event_xevent(event) ;
        if (xev->xvisibility.state == VisibilityFullyObscured)
          Fm->unobscured-- ;
        else
          Fm->unobscured++ ;
        if (!Fm->unobscured) set_timer(0) ;
        else                 set_timer(Fm->interval) ;
        Fm->file[wno]->can_paint = TRUE ;
        break ;

      case WIN_RESIZE : reposition_status(wno) ;
                        resize_path_text_item(wno) ;
                        break ;

      default : if (event_is_down(event))
		  fm_check_keys() ;
    }
  return(notify_next_event_func(frame, (Notify_event) event, arg, type)) ;
}


Notify_value
Fm_tree_event(frame, event, arg, type)     /* Catch key events. */
Frame frame ;
Event *event ;
Notify_arg arg ;
Notify_event_type type ;
{
  int i;

  switch (event_action(event))
    {
      case WIN_VISIBILITY_NOTIFY : Fm->tree.can_paint = TRUE ;
				   break;
      case ACTION_CLOSE: Fm->openwins-- ;
                         if (!Fm->openwins) set_timer(0) ;
			 for (i = 0; i < MAXPOPUPS; i++)
			    if (Fm->popup_wno[i] == WNO_TREE)
			      {
				XV_SET(X->items[Fm->popup_item[i]],
				       FRAME_CMD_PUSHPIN_IN, FALSE,
                                       0) ;

				XV_SET(X->items[Fm->popup_item[i]],
				       XV_SHOW, FALSE,
				       0);
			      }
			 break;
      case ACTION_OPEN:  Fm->openwins++ ;
			 set_timer(Fm->interval) ;
			 for (i = 0; i < MAXPOPUPS; i++)
			    if (Fm->popup_wno[i] == WNO_TREE)
			      {
				XV_SET(X->items[Fm->popup_item[i]],
				       XV_SHOW, TRUE,
				       0);

				XV_SET(X->items[Fm->popup_item[i]],
				       FRAME_CMD_PUSHPIN_IN, TRUE,
                                       0) ;
			      }
			 break;
      default:           break;
    }
  return(notify_next_event_func(frame, (Notify_event) event, arg, type)) ;
}


void
fm_check_keys()     /* Props, Copy, Paste, Find & Cut keys. */
{
  int action = event_action(X->event) ;
  enum action_type fm_action ;
  Event_Context ec ;

  switch (action)
    {
      case ACTION_PROPS :
             if (Fm->tree.selected) do_file_info_popup(WNO_TREE) ;
        else if (Fm->file[Fm->curr_wno]->path_pane.selected ||
                 get_first_selection(Fm->curr_wno) != NULL)
          do_file_info_popup(Fm->curr_wno) ;
        else
          (void) fm_props((Menu_item) NULL, MENU_NOTIFY) ;
        break ;

      case ACTION_CUT   :
      case ACTION_COPY  :
      case ACTION_PASTE :

/* Create/initialize event context. */

        ec = setup_event_context(event_window(X->event), X->event, FALSE) ;

/*  Set x & y so next routine will not accidentally find the
 *  drop was over some item.
 */

        ec->x = ec->y = -1 ;
             if (action == ACTION_CUT)   cut_like_dnd(ec) ;
        else if (action == ACTION_COPY)  copy_like_dnd(ec) ;
        else if (action == ACTION_PASTE) paste_like_dnd(ec) ;
        break ;

      case ACTION_FIND_FORWARD  :      /* Find object. */
      case ACTION_FIND_BACKWARD : find_object() ;
                                  break ;

      case ACTION_GO_LINE_BACKWARD :                           /* Home key. */
      case ACTION_GO_LINE_END      :                           /* End  key. */
      case ACTION_GO_PAGE_FORWARD  :                           /* PgDn key. */
      case ACTION_GO_PAGE_BACKWARD :                           /* PgUp key. */
        ec = setup_event_context(event_window(X->event), X->event, FALSE) ;
             if (action == ACTION_GO_LINE_BACKWARD) fm_action = FM_HOME ;
        else if (action == ACTION_GO_LINE_END)      fm_action = FM_END ;
        else if (action == ACTION_GO_PAGE_FORWARD)  fm_action = FM_PGDN ;
        else if (action == ACTION_GO_PAGE_BACKWARD) fm_action = FM_PGUP ;
        fm_canvas_page_scroll(ec->wno, fm_action) ;
        break ;
    }
}


void
set_window_dims(wno, info)
int wno, info[MAXINFO] ;
{
  int val, valx, valy ;
  Frame f ;
  Rect *rect ;

  if (wno == WNO_TREE) f = X->treeX.frame ;
  else                 f = X->fileX[wno]->frame ;

  val = info[(int) FM_INFO_ISTATE] ;
  if (val == 0 || val == 1)               /* Window state - open or closed. */
    XV_SET(f, FRAME_CLOSED, val, 0) ;

  valx = info[(int) FM_INFO_XICON] ;
  valy = info[(int) FM_INFO_YICON] ;
  if (valx != -1 || valy != -1)
    {
      rect = (Rect *) xv_get(f, FRAME_CLOSED_RECT) ;
      if (valx >= 0 && valx < Fm->screen_width)
        rect->r_left = valx ;                           /* Icon X position. */
      if (valy >= 0 && valy < Fm->screen_height)
        rect->r_top = valy ;                            /* Icon Y position. */
      XV_SET(f, FRAME_CLOSED_RECT, rect, 0) ;
    }

  valx = info[(int) FM_INFO_XOPEN] ;
  valy = info[(int) FM_INFO_YOPEN] ;
  if (valx >= 0 && valx < Fm->screen_width)
    XV_SET(f, WIN_X, valx, 0) ;                        /* Window X position. */
  if (valy >= 0 && valy < Fm->screen_height)
    XV_SET(f, WIN_Y, valy, 0) ;                        /* Window Y position. */
  val = info[(int) FM_INFO_WIDTH] ;
  if (val >= 0 && val < Fm->screen_width)
    XV_SET(f, WIN_WIDTH, val, 0) ;                     /* Window width. */
  val = info[(int) FM_INFO_HEIGHT] ;
  if (val >= 0 && val < Fm->screen_height)
    XV_SET(f, WIN_HEIGHT, val, 0) ;                    /* Window height. */
}


/*  Setup the goto menu with the contents of the user goto menu array plus
 *  the latest directories visited. The first menu item is always "Home".
 */

void
set_goto_button_menu()
{
  Menu m = X->menus[(int) FM_GOTO_MENU] ;
  Menu_item mi ;
  int i, nitems ;

  mi = (Menu_item) xv_get(m, MENU_NTH_ITEM, 1) ;
  XV_SET(mi, XV_KEY_DATA, Fm->goto_key,      MAIN_WIN, 0) ;
  XV_SET(mi, XV_KEY_DATA, Fm->goto_path_key, Fm->home, 0) ;

  nitems = (int) xv_get(m, MENU_NITEMS) - GOTO_MENU_TITLED_OR_PINNED ;

/* Remove all the menu items except the initial "Home" entry. */

  for (i = nitems; i > 1; i--)
    {
      mi = xv_get(m, MENU_NTH_ITEM, i) ;
      XV_SET(m, MENU_REMOVE, i, 0) ;
      XV_DESTROY_SAFE(mi) ;
    }

/* Insert entries from the users goto menu array. */

  for (i = 0; i < Fm->no_user_goto; i++)
    {
      mi = xv_create(XV_NULL,       MENUITEM,
                     MENU_STRING,   Fm->user_goto_label[i],
                     MENU_GEN_PROC, goto_menu_proc,
                     XV_KEY_DATA,   Fm->goto_key,      MAIN_WIN,
                     XV_KEY_DATA,   Fm->goto_path_key, Fm->user_goto[i],
                     XV_HELP_DATA,  "filemgr:Goto_User",
                     0) ;
      XV_SET(m, MENU_INSERT, (int) xv_get(m, MENU_NITEMS), mi, 0) ;
    }

/* Insert a blank menu item. */

  mi = (Menu_item) xv_create(XV_NULL,       MENUITEM,
                             MENU_STRING,   "",
                             MENU_FEEDBACK, FALSE,
                             MENU_INACTIVE, TRUE,
                             0) ;
  XV_SET(m, MENU_APPEND_ITEM, mi, 0) ;

/* Insert the application specific entries; Tree and Wastebasket. */

  mi = (Menu_item) xv_create(XV_NULL,       MENUITEM,
                             MENU_STRING,   Str[(int) M_GOTOAPP_TREE],
                             MENU_GEN_PROC, goto_menu_proc,
                             XV_KEY_DATA,   Fm->goto_key, WNO_TREE,
                             XV_HELP_DATA,  "filemgr:Goto_Tree",
                             0) ;
  XV_SET(m, MENU_APPEND_ITEM, mi, 0) ;

  mi = (Menu_item) xv_create(XV_NULL,       MENUITEM,
                             MENU_STRING,   Str[(int) M_GOTOAPP_WASTE],
                             MENU_GEN_PROC, goto_menu_proc,
                             XV_KEY_DATA,   Fm->goto_key, WASTE,
                             XV_HELP_DATA,  "filemgr:Goto_Waste",
                             0) ;
  XV_SET(m, MENU_APPEND_ITEM, mi, 0) ;

/* Insert the currently mounted CD and floppy entries. */

  for (i = 0; i < MAXCD; i++)
    if (Fm->cmntpt[i] != NULL)
      XV_SET(m, MENU_APPEND_ITEM,
           xv_create(XV_NULL,       MENUITEM,
                     MENU_STRING,   Fm->cmntpt[i],
                     MENU_GEN_PROC, goto_menu_proc,
                     XV_KEY_DATA,   Fm->goto_key,      MEDIA_WIN,
                     XV_KEY_DATA,   Fm->goto_path_key, Fm->cmntpt[i],
                     XV_HELP_DATA,  "filemgr:Goto_Media",
                     0),
           0) ;

  for (i = 0; i < MAXFLOPPY; i++)
    if (Fm->fmntpt[i] != NULL)
      XV_SET(m, MENU_APPEND_ITEM,
           xv_create(XV_NULL,       MENUITEM,
                     MENU_STRING,   Fm->fmntpt[i],
                     MENU_GEN_PROC, goto_menu_proc,
                     XV_KEY_DATA,   Fm->goto_key,      MEDIA_WIN,
                     XV_KEY_DATA,   Fm->goto_path_key, Fm->fmntpt[i],
                     XV_HELP_DATA,  "filemgr:Goto_Media",
                     0),
           0) ;

/* Insert a blank menu item. */

  mi = (Menu_item) xv_create(XV_NULL,       MENUITEM,
                             MENU_STRING,   "",
                             MENU_FEEDBACK, FALSE,
                             MENU_INACTIVE, TRUE,
                             0) ;
  XV_SET(m, MENU_APPEND_ITEM, mi, 0) ;

/* Insert entries from the latest goto menu history array. */

  for (i = 0; i < Fm->nhistory; i++)
    {
      if (i == Fm->maxgoto) break ;
      if (strcmp((char *) Fm->history[i], (char *) Fm->file[WASTE]->path))
        XV_SET(m, MENU_APPEND_ITEM,
             xv_create(XV_NULL,       MENUITEM,
                       MENU_STRING,   Fm->history[i],
                       MENU_GEN_PROC, goto_menu_proc,
                       XV_KEY_DATA,   Fm->goto_key,      MAIN_WIN,
                       XV_KEY_DATA,   Fm->goto_path_key, Fm->history[i],
                       XV_HELP_DATA,  "filemgr:Goto_Latest",
                       0),
             0) ;
    }
}


/* Bring up a floating menu on either a path or tree pane. */

void
tree_path_menu(wno)
int wno ;
{
  Xv_Window win ;
  enum menu_type m ;

  if (wno == WNO_TREE) win = X->treeX.pw ;
  else                 win = X->fileX[wno]->pathX.pw ;

  if (wno == WNO_TREE) m = get_tree_pane_menu() ;
  else                 m = get_path_pane_menu(wno) ;

  set_menu_window_no(m, wno) ;
  menu_show(X->menus[(int) m], win, X->event, 0) ;
}


/* Bring up a floating menu on a given file pane. */

void
file_pane_menu(wno)
int wno ;
{
  enum menu_type m ;

  if (wno == WASTE) m = get_waste_pane_menu() ;
  else              m = get_file_pane_menu(wno) ;

  set_menu_window_no(m, wno) ;
  menu_show(X->menus[(int) m], X->fileX[wno]->pw, X->event, 0) ;
}


void
adjust_path_height(wno)   /* Force path pane to shortest possible height. */
int wno ;
{
  int diff, cur_height, min_height, y ;

  min_height = GLYPH_HEIGHT +
               (int) xv_get(X->tree_font, FONT_DEFAULT_CHAR_HEIGHT) +
               MARGIN*3 + sb_width(wno) ;
  cur_height = (int) xv_get(X->fileX[wno]->pathX.canvas, WIN_HEIGHT) ;
  diff       = cur_height - min_height ;
  XV_SET(X->fileX[wno]->pathX.canvas, WIN_HEIGHT, min_height, 0) ;
  y          = (int) xv_get(X->fileX[wno]->status_panel, XV_Y) ;
  XV_SET(X->fileX[wno]->status_panel, XV_Y, y - diff, 0) ;
  y          = (int) xv_get(X->fileX[wno]->canvas, XV_Y) ;
  XV_SET(X->fileX[wno]->canvas, XV_Y, y - diff, 0) ;
}


/* Create and return a server image from a bitmap. */

static Server_image
server_image(image, size)
const unsigned short *image ;
int size ;
{
  return((Server_image) xv_create(0,                 SERVER_IMAGE,
                                  XV_WIDTH,          size,
                                  XV_HEIGHT,         size,
                                  SERVER_IMAGE_BITS, image,
                                  0)) ;
}


Server_image
fm_server_image(type)     /* Return file type's server image. */
SBYTE type ;
{
  switch (type)
    {
      case FT_DIR : return(X->Fm_list_image[FT_DIR]) ;
      case FT_DOC : return(X->Fm_list_image[FT_DOC]) ;
      case FT_APP : return(X->Fm_list_image[FT_APP]) ;
    }
  return(0) ;
}


void
fm_init_font()
{
  register int i ;                /* Index */
  char s[2] ;
  register XFontStruct *f_info ;

/* Calculate individual font widths. */

  X->file_font = (Xv_Font )      xv_get(X->base_frame, XV_FONT) ;
  f_info       = (XFontStruct *) xv_get(X->file_font,  FONT_INFO) ;

/* Save for global use. */

  s[1] = '\0' ;
  for (i = ' '; i <= '~'; i++)
    {
      *s = i ;
      if (f_info->per_char)
        Fm->font_width[i-' '] =
                         f_info->per_char[i-f_info->min_char_or_byte2].width ;
      else Fm->font_width[i-' '] = f_info->min_bounds.width ;
    }
  if (Fm->multi_byte)   /* Widest character */
     Fm->font_sizeW = xv_get(X->file_font, FONT_DEFAULT_CHAR_WIDTH);
  else
     Fm->font_sizeW = Fm->font_width['m'-' '] ;
  Fm->font_sizeH = (int) xv_get(X->file_font, FONT_DEFAULT_CHAR_HEIGHT) ;
  Fm->fixed_font = (Fm->font_width['m'-' '] == Fm->font_width['i'-' ']) ;

  X->Fm_list_image[FT_DIR] = server_image(Dir_list_bits, 16) ;
  X->Fm_list_image[FT_DOC] = server_image(Doc_list_bits, 16) ;
  X->Fm_list_image[FT_APP] = server_image(App_list_bits, 16) ;
}


/*ARGSUSED*/
static void
fm_exec_your_cmd(m, mi)      /* Execute a custom user command. */
Menu m ;
Menu_item mi ;
{
  char default_msg[MAXPATHLEN], *label, *prompt, *ptext1 ;
  Panel panel ;
  Panel_item run_pi ;
  int row = 1 ;

  X->Cmd_exec_mi = mi ;
  STRCPY(default_msg, Str[(int) M_ENTER_CMD_ARGS]) ;

  label  = (char *) xv_get(mi, MENU_STRING) ;
  prompt = (char *) xv_get(mi, XV_KEY_DATA, PROMPT_KEY) ;
  ptext1 = (char *) xv_get(mi, XV_KEY_DATA, PTEXT1_KEY) ;

  if (!prompt || isempty((WCHAR *) prompt) || EQUAL(prompt, "false"))
    {
      fm_exec_cmd_run((Panel_item) NULL, (Event *) NULL) ;
      return ;
    }

  if (!ptext1 || isempty((WCHAR *) ptext1))
    ptext1 = default_msg ;

  XV_DESTROY_SAFE(X->Cmd_run_frame) ;

  X->Cmd_run_frame = (Frame) xv_create(X->fileX[Fm->curr_wno]->frame, FRAME_CMD,
                                    FRAME_CMD_PUSHPIN_IN, TRUE,
                                    XV_LABEL,             label,
                                    FRAME_SHOW_FOOTER,    FALSE,
                                    0) ;

  panel = (Panel) xv_get(X->Cmd_run_frame, FRAME_CMD_PANEL, 0) ;
  if (!panel)
    {
      XV_DESTROY_SAFE(X->Cmd_run_frame) ;
      fm_msg(TRUE, Str[(int) M_WIN]) ;
      return ;
    }

  X->Cmd_run_message = xv_create(panel,      PANEL_MESSAGE,
                      PANEL_LABEL_STRING, ptext1,
                      PANEL_LABEL_BOLD,   TRUE,
                      PANEL_ITEM_Y,       xv_row(panel, row),
                      XV_HELP_DATA,  "filemgr:Create_Command_Prompt_Question",
                      0) ;

  X->Cmd_run_args = xv_create(panel,              PANEL_TEXT,
                   PANEL_VALUE_DISPLAY_LENGTH, 30,
                   PANEL_VALUE_STORED_LENGTH,  255,
                   PANEL_ITEM_Y,               xv_row(panel, row++),
                   XV_HELP_DATA, "filemgr:Create_Command_Prompt_Question",
                   0) ;

/* XXX need to add a cancel button. */

  run_pi = (Panel_item) xv_create(panel,      PANEL_BUTTON,
                          PANEL_LABEL_STRING, Str[(int) M_RUN],
                          PANEL_ITEM_Y,       xv_row(panel, row),
                          PANEL_NOTIFY_PROC,  fm_exec_cmd_run,
                          XV_HELP_DATA, "filemgr:Create_Command_Prompt_Run",
                          0) ;

  window_fit(panel) ;
  window_fit(X->Cmd_run_frame) ;

  ds_resize_text_item(panel, X->Cmd_run_args) ;
  ds_center_items(panel, row, run_pi, NULL) ;
  ds_position_popup(X->fileX[Fm->curr_wno]->frame,
                    X->Cmd_run_frame, DS_POPUP_RIGHT) ;
  XV_SET(X->Cmd_run_frame, XV_SHOW, TRUE, 0) ;
}


/*ARGSUSED*/
static void
fm_exec_cmd_run(item, event)
Panel_item item ;
Event *event ;
{

/*  User wants to execute a custom command. Three shell variables have to be
 *  setup first:
 *
 *  1) $ARG   will be set to whatever the user typed in.
 *  2) $FILE  will be set to whatever is currently selected.
 *  3) $FMPWD will be set to the current directory.
 *
 *  If there are no current selections, and the command has a $FILE, then
 *  bitch to the user. We could wind up having the command reading our stdin,
 *  which wouldn't do!
 */

  File_Pane_Object **f_p, **l_p ;
  char *args = NULL, *alias = NULL, *cmdline = NULL, *output = NULL ;
  char *cmd, *buf ;
  char *s_p = NULL ;
  int bufsize ;
  int len = 0 ;
  int sel = 0 ;
  int wno = Fm->curr_wno ;
  Boolean need_file ;                /* Does command need selected files? */

  cmdline = (char *) xv_get(X->Cmd_exec_mi, XV_KEY_DATA, CMDLINE_KEY) ;
  if (!cmdline)
    {
      fm_msg(TRUE, Str[(int) M_NO_CMD]) ;
      return ;
    }

  need_file = FALSE ;
  for (s_p = cmdline; *s_p; s_p++)
    if (*s_p == '$' && strncmp(s_p+1, "FILE", 4) == 0)
      need_file = TRUE ;

  buf = quote_file_sel(wno) ;
  bufsize = (buf) ? strlen(buf) : MAXLINE ;
  cmd = fm_malloc((unsigned) (bufsize + MAXPATHLEN + MAXPATHLEN)) ;

  if (need_file && buf == NULL)
    {

/* Let the user know "You must select something first". */

      fm_msg(TRUE, Str[(int) M_SELFIRST]) ;
      FREE(buf) ;
      return ;
    }

  output = (char *) xv_get(X->Cmd_exec_mi, XV_KEY_DATA, OUTPUT_KEY) ;

  if (X->Cmd_run_args)
    args = (char *) xv_get(X->Cmd_run_args, PANEL_VALUE) ;

  SPRINTF(cmd, Str[(int) M_CMD_ARGS],
          args ? args : "", buf ? buf : "", Fm->file[Fm->curr_wno]->path) ;

  if (output != NULL && EQUAL(output, "true"))
    {
      STRCAT(cmd, "cmdtool") ;
      STRCAT(cmd, " -Wl \"") ;
      STRCAT(cmd, Str[(int) M_CCMD_TITLE]) ;
      STRCAT(cmd, "\" sh -c \"") ;
    }
  STRCAT(cmd, cmdline) ;
  if (output != NULL && EQUAL(output, "true")) STRCAT(cmd, ";read ans\"") ;

/* Show human readable command alias on footer if possible. */

  alias = (char *) xv_get(X->Cmd_exec_mi, XV_KEY_DATA, ALIAS_KEY) ;
  if (buf != NULL)
    FREE(buf); /* freeing pre-allocated buffer */
  buf = fm_malloc(MAXLINE);  /* allocate memory of buf */
  SPRINTF(buf, Str[(int) M_RUNNING], alias ? alias : cmd) ;
  fm_msg(FALSE, buf) ;

  if (fm_run_str(cmd, FALSE)) fm_showerr(cmd) ;
  XV_DESTROY_SAFE(X->Cmd_run_frame) ;
  X->Cmd_run_frame = 0 ;
  X->Cmd_run_args  = 0 ;
  FREE(cmd) ;
  FREE(buf) ;
}


void
build_cmd_panel_list(itype)
enum item_type itype ;
{
  Custom_Command *cc ;
  Menu m = X->menus[(int) FM_CC_MENU] ;
  Menu_item mi ;
  Panel list = X->items[(int) itype] ;
  char *label, *ptr ;
  int i, j, total ;

  total = (int) xv_get(m, MENU_NITEMS) - 2 ;    /* Number of user commands. */
  j = 0 ;
  for (i = 0; i < total; i++)
    {
      mi = (Menu_item) xv_get(m, MENU_NTH_ITEM, i+1) ;
      if (mi != 0)
        {

/* Get label (either alias or UNIX command). */

          if ((label = (char *) xv_get(mi, XV_KEY_DATA, ALIAS_KEY)) == NULL)
            label = (char *) xv_get(mi, XV_KEY_DATA, CMDLINE_KEY) ;

          if (label)
            {
              cc = (Custom_Command *)
                   LINT_CAST(fm_malloc((unsigned) sizeof(Custom_Command))) ;

              ptr = (char *) xv_get(mi, XV_KEY_DATA, ALIAS_KEY) ;
              if (ptr == NULL) ptr = "" ;
              cc->alias = strdup(ptr) ;

              ptr = (char *) xv_get(mi, XV_KEY_DATA, CMDLINE_KEY) ;
              if (ptr == NULL) ptr = "" ;
              cc->command = strdup(ptr) ;

              ptr = (char *) xv_get(mi, XV_KEY_DATA, PTEXT1_KEY) ;
              if (ptr == NULL) ptr = "" ;
              cc->prompt = strdup(ptr) ;

              ptr = (char *) xv_get(mi, XV_KEY_DATA, PROMPT_KEY) ;
              if (ptr == NULL) ptr = "" ;
              cc->is_prompt = strdup(ptr) ;

              ptr = (char *) xv_get(mi, XV_KEY_DATA, OUTPUT_KEY) ;
              if (ptr == NULL) ptr = "" ;
              cc->is_output = strdup(ptr) ;

              XV_SET(list,
                     PANEL_LIST_INSERT,        j,
                     PANEL_LIST_STRING,        j, label,
                     PANEL_LIST_CLIENT_DATA,   j, cc,
                     0) ;
              j++ ;
            }
        }
    }
}


static void
fm_el_paste(list, row, itype)
Xv_opaque list ;
int row ;
enum item_type itype ;
{
  Custom_Command *cc;

  if (Fm->list_label == NULL) return ;
  if (itype == FM_PO_GOTO_LIST)
  XV_SET(list,
         PANEL_LIST_INSERT,      row,
         PANEL_LIST_STRING,      row, strdup(Fm->list_label),
         PANEL_LIST_CLIENT_DATA, row, strdup(Fm->client_data),
         0) ;
  else  /* FM_PO_CMD_LIST */
    {
      cc = (Custom_Command *)
	LINT_CAST(fm_malloc((unsigned) sizeof(Custom_Command))) ;
      MEMCPY((char *)cc, (char *)Fm->client_data, sizeof(Custom_Command)) ;
      XV_SET(list,
         PANEL_LIST_INSERT,      row,
         PANEL_LIST_STRING,      row, strdup(Fm->list_label),
         PANEL_LIST_CLIENT_DATA, row, cc,
         0) ;
    }
}


/*ARGSUSED*/
void
do_fm_cc_goto_menu_proc(item, val)
Menu_item item ;
int val ;
{
  Xv_opaque list ;
  char alias[MAXLINE], client_data[MAXPATHLEN], *ptr ;
  Custom_Command *cc;
  enum item_type itype ;
  int row, total ;

  if (Fm->cur_prop_sheet == PROPS_STACK_GOTO) itype = FM_PO_GOTO_LIST ;
  else                                        itype = FM_PO_CMD_LIST ;
  list = X->items[(int) itype] ;

  if ((row = get_list_first_selected(itype)) != -1)
    {
      *client_data = '\0' ;
      ptr = (char *) xv_get(list, PANEL_LIST_CLIENT_DATA, row) ;

      if (itype == FM_PO_GOTO_LIST)
	{
	  if (ptr) STRCPY(client_data, ptr) ;
	}
      else  /* Custom Command */
	{
	  cc = (Custom_Command *)
	    LINT_CAST(fm_malloc((unsigned) sizeof(Custom_Command)));
	  MEMCPY((char *)cc, (char *)ptr, sizeof(Custom_Command));
	}

      *alias = '\0' ;
      ptr = (char *) xv_get(list, PANEL_LIST_STRING, row) ;
      if (ptr) STRCPY(alias, ptr) ;
    }
  else if (val != M_EL_PASTE_TOP && val != M_EL_PASTE_BOTTOM) return ;

  total = (int) xv_get(list, PANEL_LIST_NROWS) ;
  switch (val)
    {
      case M_EL_PASTE_BEFORE : fm_el_paste(list, row, itype) ;
                               break ;
      case M_EL_PASTE_AFTER  : fm_el_paste(list, row+1, itype) ;
                               break ;
      case M_EL_PASTE_TOP    : row = 0 ;
                               if (Fm->cur_prop_sheet == PROPS_STACK_GOTO)
                                 row++ ;
                               fm_el_paste(list, row, itype) ;
                               break ;
      case M_EL_PASTE_BOTTOM : fm_el_paste(list, total, itype) ;
                               break ;
      case M_EL_CUT          : XV_SET(list, PANEL_LIST_DELETE, row, 0) ;
      case M_EL_COPY         : if (Fm->list_label) FREE(Fm->list_label) ;
                               Fm->list_label = strdup(alias) ;
                               if (Fm->client_data) FREE(Fm->client_data) ;
	                       if (itype == FM_PO_GOTO_LIST)
                                Fm->client_data = strdup(client_data) ;
	                       else Fm->client_data = (char *)cc ;
                               break ;
      case M_EL_DELETE       : XV_SET(list, PANEL_LIST_DELETE, row, 0) ;
                               ZFREE(Fm->list_label) ;
    }
}


/*  Read in custom command entries from the user's ~/.fmcmd file and setup
 *  custom menu.
 */

void
fm_include_old(fname)
char *fname ;
{
  Menu m = X->menus[(int) FM_CC_MENU] ;
  Menu_item mi ;
  int i, insert_pt, total ;
  char *alias_str, *command_str, *is_output_str, *is_prompt_str, *prompt_str ;

  load_old_ccmd_defs(fname) ;
  i = 0 ;
  for (;;)
    {
      alias_str     = get_old_ccmd_str_resource(i, "alias") ;
      command_str   = get_old_ccmd_str_resource(i, "command") ;
      prompt_str    = get_old_ccmd_str_resource(i, "prompt1") ;
      is_prompt_str = get_old_ccmd_str_resource(i, "prompt") ;
      if (is_prompt_str && EQUAL(is_prompt_str, "1")) is_prompt_str = "true" ;
      else                                            is_prompt_str = "false" ;
      is_output_str = get_old_ccmd_str_resource(i, "output") ;
      if (is_output_str && EQUAL(is_output_str, "1")) is_output_str = "true" ;
      else                                            is_output_str = "false" ;

      if (alias_str == NULL) alias_str = command_str ;
      if (alias_str == NULL) break ;

      mi = (Menu_item) xv_create(XV_NULL,                  MENUITEM,
                                 MENU_NOTIFY_PROC,         fm_exec_your_cmd,
                                 MENU_STRING,              alias_str,
                                 XV_KEY_DATA, ALIAS_KEY,   alias_str,
                                 XV_KEY_DATA, CMDLINE_KEY, command_str,
                                 XV_KEY_DATA, PROMPT_KEY,  is_prompt_str,
                                 XV_KEY_DATA, PTEXT1_KEY,  prompt_str,
                                 XV_KEY_DATA, OUTPUT_KEY,  is_output_str,
                                 0) ;
      insert_pt = (int) xv_get(m, MENU_NITEMS) - 2 ;
      XV_SET(m, MENU_INSERT, insert_pt, mi, 0) ;
      i++ ;
    }

  total = (int) xv_get(m, MENU_NITEMS) ;
  mi    = (Menu_item) xv_get(m, MENU_NTH_ITEM, total) ;
  XV_SET(m, MENU_DEFAULT_ITEM, mi, 0) ;
  Fm->add_old = FALSE ;
}


/*  Read in custom command entries from the user's ~/.desksetdefaults file
 *  and setup custom menu.
 */

void
fm_process_cmdfile()
{
  Menu m = X->menus[(int) FM_CC_MENU] ;
  Menu_item mi ;
  int i, insert_pt, total ;
  char *alias_str, *command_str, *is_output_str, *is_prompt_str, *prompt_str ;

  for (i = 0; i < Fm->no_ccmds; i++)
    {
      alias_str     = get_ccmd_str_resource(i, "Alias") ;
      command_str   = get_ccmd_str_resource(i, "Command") ;
      prompt_str    = get_ccmd_str_resource(i, "Prompt") ;
      is_prompt_str = get_ccmd_str_resource(i, "IsPrompt") ;
      is_output_str = get_ccmd_str_resource(i, "IsOutput") ;
      if (alias_str == NULL) alias_str = command_str ;

      if (alias_str)
        mi = (Menu_item) xv_create(XV_NULL,                  MENUITEM,
                                   MENU_NOTIFY_PROC,         fm_exec_your_cmd,
                                   MENU_STRING,              alias_str,
                                   XV_KEY_DATA, ALIAS_KEY,   alias_str,
                                   XV_KEY_DATA, CMDLINE_KEY, command_str,
                                   XV_KEY_DATA, PROMPT_KEY,  is_prompt_str,
                                   XV_KEY_DATA, PTEXT1_KEY,  prompt_str,
                                   XV_KEY_DATA, OUTPUT_KEY,  is_output_str,
                                   0) ;
      insert_pt = (int) xv_get(m, MENU_NITEMS) - 2 ;
      XV_SET(m, MENU_INSERT, insert_pt, mi, 0) ;
    }

  total = (int) xv_get(m, MENU_NITEMS) ;
  mi    = (Menu_item) xv_get(m, MENU_NTH_ITEM, total) ;
  XV_SET(m, MENU_DEFAULT_ITEM, mi, 0) ;
}


/* Linux filesystem capacity; Solaris quota RPC/ioctls are unavailable. */
void
set_status_info(int wno)
{
    struct statvfs fs;
    char buf[MAXLINE];
    Fm->file[wno]->prev_num_objects = Fm->file[wno]->num_objects;
    snprintf(buf, sizeof(buf), Str[(int) M_CONTAINS],
             Fm->file[wno]->num_objects);
    xv_set(X->fileX[wno]->leftmess, PANEL_LABEL_STRING, buf, 0);
    if (statvfs((char *) Fm->file[wno]->path, &fs) == 0) {
        unsigned long long available = (unsigned long long) fs.f_bavail *
                                       fs.f_frsize / (1024 * 1024);
        snprintf(buf, sizeof(buf), "%llu MiB available", available);
    } else {
        snprintf(buf, sizeof(buf), "%s", strerror(errno));
    }
    xv_set(X->fileX[wno]->rightmess, PANEL_LABEL_STRING, buf, 0);
}


/* Returns width (in pixels) of panel items's label, based on its font. */

int
get_label_width(itype)
enum item_type itype ;
{
  Xv_font font ;
  Xv_opaque pi = X->items[(int) itype] ;
  Font_string_dims font_size ;
  char *string ;

  font   = (Xv_Font) xv_get(pi, PANEL_LABEL_FONT) ;
  string = (char *) xv_get(pi, PANEL_VALUE) ;
  XV_GET(font, FONT_STRING_DIMS, string, &font_size) ;
  return(font_size.width) ;
}


/* Returns width (in pixels) of a string based on the panel items's font. */

int
get_panel_item_value_width(itype, s)
enum item_type itype ;
char *s ;
{
  Xv_font font ;
  Xv_opaque pi = X->items[(int) itype] ;
  Font_string_dims font_size ;

  font = (Xv_Font) xv_get(pi, PANEL_LABEL_FONT) ;
  XV_GET(font, FONT_STRING_DIMS, s, &font_size) ;
  return(font_size.width) ;
}


void
set_toggle_xvals(col_x, tab)
int col_x, tab ;
{
  int j ;

  for (j = 0; j < NUM_TOGGLES; j++)
    XV_SET(X->toggle[j], PANEL_ITEM_X, col_x + ((j % 3) * tab) + 5, 0) ;
}


void
init_toggles()
{
  int i ;

  X->toggle[0] = X->items[(int) FM_FIO_OWNER_R] ;
  X->toggle[1] = X->items[(int) FM_FIO_OWNER_W] ;
  X->toggle[2] = X->items[(int) FM_FIO_OWNER_E] ;
  X->toggle[3] = X->items[(int) FM_FIO_GROUP_R] ;
  X->toggle[4] = X->items[(int) FM_FIO_GROUP_W] ;
  X->toggle[5] = X->items[(int) FM_FIO_GROUP_E] ;
  X->toggle[6] = X->items[(int) FM_FIO_WORLD_R] ;
  X->toggle[7] = X->items[(int) FM_FIO_WORLD_W] ;
  X->toggle[8] = X->items[(int) FM_FIO_WORLD_E] ;

  for (i = 0; i < NUM_TOGGLES; i++)
    XV_SET(X->toggle[i], PANEL_CLIENT_DATA, i, 0) ;
}


static void
fio_set_text_value(item, s, first_file)
Panel_item item ;
char *s ;
int first_file ;
{
  char *value_p ;

  if (first_file)
    XV_SET(item,
           PANEL_VALUE,    s,
           PANEL_INACTIVE, FALSE,
           0) ;
  else
    {
      value_p = (char *) xv_get(item, PANEL_VALUE) ;
      if (value_p && !EQUAL(value_p, s))
        XV_SET(item, PANEL_INACTIVE, TRUE, 0) ;
    }
}


static void
fio_set_message_value(item, s, first_file)
Panel_item item ;
char *s ;
int first_file ;
{
  char *value_p ;

  if (first_file)
    XV_SET(item,
           PANEL_VALUE,    s,
           PANEL_INACTIVE, FALSE,
           0) ;
  else
    {
      value_p = (char *) xv_get(item, PANEL_VALUE) ;
      if (value_p && !EQUAL(value_p, s))
          XV_SET(item, PANEL_INACTIVE, TRUE, 0) ;
    }
}


int
fio_compare_and_update_panel(fpo, wno, first_file)
File_Pane_Object *fpo ;
int wno ;
Boolean first_file ;        /* First file checked? */
{
  Panel_item item ;
  Server_image icon_si, old_icon_si ;
  char buf[MAXPATHLEN], *nptr, *sp ;
  Boolean show, sym_link ;
  Boolean broken_link = FALSE ;
  int blk_size, j, uid ;
#ifdef SVR4
  struct mnttab *mount_pt, *fio_mount_point() ;
  struct statvfs fsbuf ;  /* File system status */
#else
  struct mntent *mount_pt, *fio_mount_point() ;
  struct statfs fsbuf ;  /* File system status */
#endif /*SVR4*/
  void fio_mount_free() ;

  uid = getuid() ;

/* Get full path name and base name of selected object. */

  if (!fpo)
    {
      if (wno == WNO_TREE) fm_getpath(Fm->tree.selected, (WCHAR *) buf) ;
      else fm_getpath(Fm->file[wno]->path_pane.selected, (WCHAR *) buf) ;
    }
  else
    SPRINTF(buf, "%s%s%s", (char *) Fm->file[wno]->path,
            Fm->file[wno]->path[1] ? "/" : "", (char *) fpo->name) ;
  sp = buf ;

  ZFREE(Fm->fullname) ;
  Fm->fullname = (char *) fm_malloc((unsigned) strlen(sp)+1) ;
  if (!Fm->fullname) return(0) ;
  STRCPY(Fm->fullname, sp) ;

  Fm->basename = (Fm->fullname[1] == 0 ? Fm->fullname : strrchr(Fm->fullname, '/') + 1) ;

/* Get the file status (check if sym link first) and mount point. */

  if (lstat(Fm->fullname, &Fm->status) == -1)
    {

/* lstat failed -- return. */

      fm_msg(TRUE, Str[(int) M_READSTR], Fm->fullname, strerror(errno)) ;
      return(0) ;
    }

  sym_link = ((Fm->status.st_mode & S_IFMT) == S_IFLNK) ;

  if (broken_link == FALSE)
    if (!fpo) mount_pt = fio_mount_point(Fm->fullname) ;
    else      mount_pt = fio_mount_point((char *) Fm->file[wno]->path) ;


/* icon_name */

  if (!fpo) icon_si = get_generic_icon_image(FT_DIR, FALSE) ;
  else      icon_si = get_icon_server_image(fpo, FALSE) ;

  if (first_file)
    XV_SET(X->items[(int) FM_FIO_ICON_ITEM],
           PANEL_LABEL_IMAGE, icon_si,
           XV_SHOW,           TRUE,
           PANEL_INACTIVE,    FALSE,
           0) ;
  else
    {
      old_icon_si = (Server_image)
        xv_get(X->items[(int) FM_FIO_ICON_ITEM], PANEL_LABEL_IMAGE) ;

      if (icon_si != old_icon_si)
        set_item_int_attr(FM_FIO_ICON_ITEM, FM_ITEM_INACTIVE, TRUE) ;
    }
  if (broken_link == TRUE) return(1) ;

/* file_name */

  fio_set_text_value(X->items[(int) FM_FIO_FILE_NAME], Fm->basename, first_file) ;
  if (broken_link == TRUE) item = X->items[(int) FM_FIO_FREE_SPACE] ;
  else item = X->items[(int) FM_FIO_FILE_NAME] ;
  XV_SET(item,
         PANEL_READ_ONLY, (uid != (int) Fm->status.st_uid && uid != 0),
         0) ;

/* owner */

  if ((sp = fio_get_name((int) Fm->status.st_uid)) == 0)
    {
      SPRINTF(buf, "%d", Fm->status.st_uid) ;
      sp = buf ;
    }
  fio_set_text_value(X->items[(int) FM_FIO_OWNER], sp, first_file) ;
  XV_SET(X->items[(int) FM_FIO_OWNER],
         PANEL_READ_ONLY, (uid != (int) Fm->status.st_uid && uid != 0),
         0) ;
  if (first_file)
    {

/* Save for later comparison in apply and reset. */

      Fm->f_owner = strdup(sp) ;
      Fm->f_status.st_mode = Fm->status.st_mode ;
    }

/* group */

  if ((sp = fio_get_group((int) Fm->status.st_gid)) == 0)
    {
      SPRINTF(buf, "%d", Fm->status.st_gid) ;
      sp = buf ;
    }
  fio_set_text_value(X->items[(int) FM_FIO_GROUP], sp, first_file) ;
  XV_SET(X->items[(int) FM_FIO_GROUP],
         PANEL_READ_ONLY, (uid != (int) Fm->status.st_uid && uid != 0),
         0) ;
  if (first_file)

/* Save for later comparison in apply and reset. */

    Fm->f_group = strdup(sp) ;

/* bite_size */

  do_fio_folder_by_content() ;

/* mod_time */

  sp = make_time(&Fm->status.st_mtime) ;
  fio_set_message_value(X->items[(int) FM_FIO_MOD_TIME], sp, first_file) ;

/* access_time */

  sp = make_time(&Fm->status.st_atime) ;
  fio_set_message_value(X->items[(int) FM_FIO_ACCESS_TIME], sp, first_file) ;

/* file_type */

  if (!sym_link)
    {
      FILE *fp ;

#ifdef SVR4
      SPRINTF(buf, "/usr/bin/file \"%s\"", Fm->fullname) ;
#else
      SPRINTF(buf, "/usr/bin/file -L \"%s\"", Fm->fullname) ;
#endif /*SVR4*/

      if ((fp = popen(buf, "r")) != NULL)
        {
          *buf = 0 ;
          FGETS(buf, 200, fp) ;
          PCLOSE(fp) ;
        }

/* Skip file name. */

      if ((sp = strrchr(buf, '\t')) != NULL)
        {
          sp++ ;
          if (strcoll(sp, Str[(int) M_PERM_DENIED]) == 0)
            sp = Str[(int) M_UNKNOWN] ;
        }
     else sp = Str[(int) M_UNKNOWN] ;
    }
  else
    {
      STRCPY(buf, Str[(int) M_LINK]) ;
      sp = buf ;
    }
  fio_set_message_value(X->items[(int) FM_FIO_FILE_TYPE], sp, first_file) ;

/* contents */

/* If this is a directory. */

  if ((Fm->status.st_mode & S_IFMT) == S_IFDIR)
    {
      if (first_file) show = TRUE ;
      else
        show = (Boolean) xv_get(X->items[(int) FM_FIO_CONTENTS], XV_SHOW) ;
    }
  else show = FALSE ;

  set_item_int_attr(FM_FIO_CONTENTS, FM_ITEM_SHOW, show) ;
  set_toggles(Fm->status, first_file) ;

/* open_method */

  sp = NULL ;
  if (fpo) sp = (char *) get_open_method(fpo) ;

  if (!sp)
    {
      STRCPY(buf, "") ;
      sp = buf ;
    }

  fio_set_text_value(X->items[(int) FM_FIO_OPEN_METHOD], sp, first_file) ;

/* print_method */

  sp = NULL ;
  if (fpo) sp = (char *) get_print_method(fpo) ;

  if (!sp)
    {
      STRCPY(buf, "") ;
      sp = buf ;
    }

  fio_set_text_value(X->items[(int) FM_FIO_PRINT_METHOD], sp, first_file) ;

/* free_space */

  if (!fpo) nptr = Fm->fullname ;
  else      nptr = (char *) Fm->file[wno]->path ;

#ifdef SVR4
  if (statvfs(nptr, &fsbuf) == 0)
#else
  if (statfs(nptr, &fsbuf) == 0)
#endif /*SVR4*/
    {
#ifdef SVR4
      blk_size = fsbuf.f_frsize ;
#else
      blk_size = fsbuf.f_bsize ;
#endif /*SVR4*/
      j = fsbuf.f_blocks - (fsbuf.f_bfree - fsbuf.f_bavail) ;
      if (!j) SPRINTF(buf, "%s", Str[(int) M_UNDEFINED]) ;
      else
        SPRINTF(buf, Str[(int) M_KBYTES2],
          fio_add_commas((off_t) ((double)(fsbuf.f_bavail * blk_size) / (double)1024)),
          (int) (100-(double)(100*(j - fsbuf.f_bavail))/(double)j)) ;
    }

  fio_set_message_value(X->items[(int) FM_FIO_FREE_SPACE], buf, first_file) ;

/* mount_point */

#ifdef SVR4
  if (mount_pt)
    fio_set_text_value(X->items[(int) FM_FIO_MOUNT_POINT],
                       mount_pt->mnt_mountp, first_file) ;
#else
  if (mount_pt)
    fio_set_text_value(X->items[(int) FM_FIO_MOUNT_POINT],
                       mount_pt->mnt_dir, first_file) ;
#endif /*SVR4*/

/* mount_from */

#ifdef SVR4
  if (mount_pt)
    fio_set_text_value(X->items[(int) FM_FIO_MOUNT_FROM],
                       mount_pt->mnt_special, first_file) ;
#else
  if (mount_pt)
    fio_set_text_value(X->items[(int) FM_FIO_MOUNT_FROM],
                       mount_pt->mnt_fsname, first_file) ;
#endif /*SVR4*/

  if (mount_pt) fio_mount_free(mount_pt) ;
  return(1) ;
}


void
set_toggles(status, first_file)
struct stat status ;                /* File permissions status */
Boolean first_file  ;               /* First file checked? */
{
  static unsigned int toggle_mask[] = {
    S_IREAD,      S_IWRITE,      S_IEXEC,
    S_IREAD >> 3, S_IWRITE >> 3, S_IEXEC >> 3,
    S_IREAD >> 6, S_IWRITE >> 6, S_IEXEC >> 6
  } ;

  int j, toggles[NUM_TOGGLES] ;
  unsigned int x ;

/* Get toggle values for new item. */

  for (j = 0; j < NUM_TOGGLES; j++)
    toggles[j] = ((status.st_mode & toggle_mask[j]) != 0) ;

  if (first_file)
    {
      set_item_int_attr(FM_FIO_PERMISSIONS, FM_ITEM_INACTIVE, FALSE) ;
      set_item_int_attr(FM_FIO_PERM_READ,   FM_ITEM_INACTIVE, FALSE) ;
      set_item_int_attr(FM_FIO_PERM_WRITE,  FM_ITEM_INACTIVE, FALSE) ;
      set_item_int_attr(FM_FIO_PERM_EXE,    FM_ITEM_INACTIVE, FALSE) ;
      set_item_int_attr(FM_FIO_OWNER_PERM,  FM_ITEM_INACTIVE, FALSE) ;
      set_item_int_attr(FM_FIO_GROUP_PERM,  FM_ITEM_INACTIVE, FALSE) ;
      set_item_int_attr(FM_FIO_WORLD_PERM,  FM_ITEM_INACTIVE, FALSE) ;
      for (j = 0; j < NUM_TOGGLES; j++)
        XV_SET(X->toggle[j],
               PANEL_VALUE,    toggles[j],
               PANEL_INACTIVE, FALSE,
               0) ;
    }
  else
    {
      for (j = 0; j < NUM_TOGGLES; j++)
        {

/* Get toggle values for old item. */

          x = (unsigned int) xv_get(X->toggle[j], PANEL_VALUE) ;

/* Compare new and old items, and turn off if not equal. */

          if (toggles[j] != x)
            XV_SET(X->toggle[j], PANEL_INACTIVE, TRUE, 0) ;
        }
    }
}


/* Turn on editable text item if selected. */

void
do_fio_activate_panel_item(item, event)
Panel_item item ;
Event *event ;
{
  if (xv_get(item, PANEL_INACTIVE))
    if (event_action(event) == ACTION_SELECT && event_is_up(event))
      {

/*  Activate item unless its the filename.  You cannot apply the
 *  same filename to multiple files.
 */
        if (item == X->items[(int) FM_FIO_FILE_NAME ]&& Fm->nseln > 1)
          NOTICE_PROMPT(X->items[(int) FM_FIO_FIO_FRAME], NULL,
                        NOTICE_MESSAGE_STRINGS,
                          Str[(int) M_NO_ACT_NAME],
                          0,
                        NOTICE_BUTTON_YES, Str[(int) M_OK],
                        0) ;
        else
          XV_SET(item, PANEL_INACTIVE, FALSE, 0) ;
    }
  panel_default_handle_event(item, event) ;
}


/* Convert file permission toggles to one integer. */

int
fio_toggle_to_int()
{
  int bit, i, l ;

  l = 0 ;
  bit = S_IREAD ;
  for (i = 0; i < NUM_TOGGLES; i++ )
    {
      if (xv_get(X->toggle[i], PANEL_VALUE)) l = l | bit ;
      bit >>= 1 ;
    }
  return(l) ;
}


void
do_fio_apply_proc()
{
  int i, n ;
  int error = 1 ;
  int wno   = Fm->curr_wno ;
  char *s, buf[MAXPATHLEN] ;

  if (Fm->tree.selected)
    {
      error = fio_apply_change(Fm->fullname) ;

/* A name-change redraws the window. */

      s = get_item_str_attr(FM_FIO_FILE_NAME, FM_ITEM_IVALUE) ;
      if (*s && EQUAL(s, (char *) Fm->tree.selected->name))
        {
          i = strlen((char *) Fm->tree.selected->name) ;
          STRNCPY((char *) Fm->tree.selected->name, s, i) ;
          Fm->tree.selected->name[i] = 0 ;
          fm_drawtree(FALSE) ;
        }
    }
  else if (Fm->file[wno]->path_pane.selected)
    {
       fm_getpath(Fm->file[wno]->path_pane.selected, (WCHAR *) buf) ;
       error = fio_apply_change(buf) ;
       s = get_item_str_attr(FM_FIO_FILE_NAME, FM_ITEM_IVALUE) ;
       /* name change */
       if (!error && *s && !EQUAL(s, Fm->basename))
          {
            char *tmp, *old_ptr;
	    char old_path[MAXPATHLEN], new_path[MAXPATHLEN];
	    char goto_path[MAXPATHLEN], tmp_buf[MAXPATHLEN];
	    char tmp_path[MAXPATHLEN];
	    struct stat fstatus ;


	    fm_snprintf(old_path, sizeof(old_path), "%s/%s%s", Fm->home, ".fm", buf);
	    tmp = strrchr(buf, '/');
	    strcpy(tmp_buf, buf);
	    tmp_buf[tmp-buf] = '\0';
	    fm_snprintf(new_path, sizeof(new_path), "%s/%s%s/%s", Fm->home, ".fm", tmp_buf, s);
	    RENAME(old_path, new_path); /* rename .fm directory */

	    /* update the path pane with the new name */
	    strcpy(tmp_path, (char*)Fm->file[wno]->path);
	    strcpy(goto_path, "");
	    old_ptr = (char*)strtok(tmp_path, "/");
	    while (old_ptr)
	      {
		strcat(goto_path, "/");
		strcat(goto_path, old_ptr);
		if (strcmp(tmp_buf, goto_path) == 0)
		  {
		    (char *) strtok(NULL, "/");
		    strcat(goto_path, "/");
		    strcat(goto_path, s);
		    old_ptr = (char *) strtok(NULL, "/");
		    while(old_ptr)
		      {
		        strcat(goto_path, "/");
		        strcat(goto_path, old_ptr);
		        old_ptr = (char *) strtok(NULL, "/");
		      }
		    break;
		  }
                else
		    old_ptr = (char *) strtok(NULL, "/");
	      }
	    strcpy((char*)Fm->file[wno]->path, goto_path);
	    Fm->isgoto_select = TRUE;
	    fm_busy_cursor(TRUE, wno) ;
	    fm_pathdeselect(wno) ;
	    clear_position_info(wno) ;
	    *tmp++;
	    remove_tree_entry(path_to_node(WNO_TREE, (WCHAR*)tmp_buf), (WCHAR*)tmp) ;
	    set_tree_icon(path_to_node(WNO_TREE, Fm->file[wno]->path), FALSE) ;
	    if (fm_openfile(goto_path, (char *) NULL, TRUE, wno))
	        set_path_item_value(wno, "") ;
	    set_menu_items(FM_GOTO_MENU, set_goto_button_menu) ;
	    fio_clear_panel();
	    Fm->isgoto_select = FALSE ;
	    fm_busy_cursor(FALSE, wno) ;
          }
    }
  else
    {
      File_Pane_Object **curr, **last ;
      int nseln = 0 ;
      int nmode = Fm->f_status.st_mode;
      struct stat sbuf;

      last = PTR_LAST(wno) ;
      for (curr = PTR_FIRST(wno); curr != last; curr++)
      {
        if ((*curr)->selected)
          {
            SPRINTF(buf, "%s/%s", Fm->file[wno]->path, (*curr)->name) ;
            if (fm_stat(buf, &sbuf) == 0)
                Fm->f_status.st_mode = sbuf.st_mode;
            error = fio_apply_change(buf) ;
	    nmode = Fm->f_status.st_mode;
	    if (!error) (*curr)->mode = nmode;

/*  If the name field is active and there was a name change, then update the
 *  File_Pane_Object name so that the icon doesn't get "moved" in the folder
 *  pane.
 */

            if (!get_item_int_attr(FM_FIO_FILE_NAME, FM_ITEM_INACTIVE))
              {
                s = get_item_str_attr(FM_FIO_FILE_NAME, FM_ITEM_IVALUE) ;
                if (!error && *s && !EQUAL(s, buf))
                  {
                    FREE((*curr)->name) ;
                    n = strlen(s) ;
                    (*curr)->name = (WCHAR *) fm_malloc((unsigned) n + 1) ;
                    STRCPY((char *) (*curr)->name, s) ;
                    (*curr)->flen = n ;
                    (*curr)->width = fm_strlen((char *) (*curr)->name) ;
                  }
              }
            nseln++ ;
          }
      }
      Fm->f_status.st_mode = nmode;
      if (nseln == 0)
        fm_msg(TRUE, Str[(int) M_SELFIRST]) ;
      if (!error && is_frame(wno))
        fm_display_folder((BYTE) FM_STYLE_FOLDER, wno) ;
    }

  if (!error && !xv_get(X->items[(int) FM_FIO_FIO_FRAME], FRAME_CMD_PUSHPIN_IN))
    fio_done_proc(X->items[(int) FM_FIO_FIO_FRAME]) ;
}


/*ARGSUSED*/
void
do_fio_reset_proc(item, event)
Panel_item item ;
Event *event ;
{
  fio_update_panel(Fm->curr_wno) ;

/* Do this so XView will not dismiss the possibly unpinned popup. */

  XV_SET(item, PANEL_NOTIFY_STATUS, XV_ERROR, 0) ;
}


/*  Function:    ds_get_predefined_cursor
 *
 *  Description: Returns a predefined cursor as described on p285
 *               of O'Reilly`s XView Programming Manual, ver 2.
 *
 *               Predefined cursors are really images from the font
 *               -sun-open look cursor-----12-120-75-75-p-455-sunolcursor-1
 *               These are the set of OpenLook dnd cursors which
 *               should be used for drag and drop operations.
 *
 *  Parameters:  index  Index into the array of glyphs the font
 *                      contains (128 cursors currently available).
 *
 *                      In <xview/cursor.h> are the cursor defines
 *                      prefixed by OLC_ which you should use to
 *                      identify the predefined cursor to use.
 *
 *                      The following are some of the most common
 *                      to use.
 *
 *               OLC_DOC_MOVE_DRAG         document + move + pointer
 *               OLC_DOC_MOVE_DROP         document + move + drop
 *               OLC_DOC_MOVE_NODROP       document + move + no drop
 *
 *               OLC_DOC_COPY_DRAG         document + copy + pointer
 *               OLC_DOC_COPY_DROP         document + copy + drop
 *               OLC_DOC_COPY_NODROP       document + copy + no drop
 *
 *               OLC_DOCS_MOVE_DRAG        document stack + move + pointer
 *               OLC_DOCS_MOVE_DROP        document stack + move + drop
 *               OLC_DOCS_MOVE_NODROP      document stack + move + no drop
 *
 *               OLC_DOCS_COPY_DRAG        document stack + copy + pointer
 *               OLC_DOCS_COPY_DROP        document stack + copy + drop
 *               OLC_DOCS_COPY_NODROP      document stack + copy + no drop
 *
 *  Bugs:       No predefined cursors exist for move/copy folders,
 *              though the components exist to make them.  Need to
 *              expand the function to make them.
 *
 *  Returns:    the created cursor or NULL if unable to create the cursor
 */

static Xv_Cursor
ds_get_predefined_cursor(index)
int index ;
{
  Xv_Cursor c ;

  if (index < OLC_FIRST_INDEX || index > OLC_LAST_INDEX)
    {
      FPRINTF(stderr, "ds_get_predefined_cursor: unknown cursor index %d\n",
              index) ;
      return(0) ;
    }
  else if (index % 2)    /* Odd number indicates a mask cursor. */
    {
      FPRINTF(stderr,
 "ds_get_predefined_cursor: index %d is a mask cursor -- using its own image\n",
              index) ;
      --index ;
    }

  c = (Xv_cursor) xv_create(0, CURSOR,
                    CURSOR_SRC_CHAR,  index,
                    CURSOR_MASK_CHAR, index+1,   /* Assume mask is next one. */
                    0) ;
  return(c) ;
}


Xv_Cursor
get_accept_cursor(isfolder, copy, many)    /* Build an accept cursor. */
Boolean isfolder ;                         /* Folder cursor wanted? */
Boolean copy ;                             /* Supply copy cursor. */
Boolean many ;                             /* Multiple icons dragging? */
{
  Xv_Cursor cursor ;

  if (isfolder)
    {
           if (!copy && !many)
        cursor = get_folder_cursor(OLC_FOLDER_MOVE_DROP) ;
      else if (!copy && many)
        cursor = get_folder_cursor(OLC_FOLDERS_MOVE_DROP) ;
      else if (copy && !many)
        cursor = get_folder_cursor(OLC_FOLDER_COPY_DROP) ;
      else
        cursor = get_folder_cursor(OLC_FOLDERS_COPY_DROP) ;
    }
  else
    {
           if (!copy && !many)
        cursor = ds_get_predefined_cursor(OLC_DOC_MOVE_DROP) ;
      else if (!copy && many)
        cursor = ds_get_predefined_cursor(OLC_DOCS_MOVE_DROP) ;
      else if (copy && !many)
        cursor = ds_get_predefined_cursor(OLC_DOC_COPY_DROP) ;
      else
        cursor = ds_get_predefined_cursor(OLC_DOCS_COPY_DROP) ;
    }
  return(cursor) ;
}


Xv_Cursor
get_drag_cursor(isfolder, copy, many)    /* Build a drag cursor. */
Boolean isfolder ;                       /* Folder cursor wanted? */
Boolean copy ;                           /* Supply copy cursor. */
Boolean many ;                           /* Multiple icons dragging? */
{
  Xv_Cursor cursor ;

  if (isfolder)
    {
           if (!copy && !many)
        cursor = get_folder_cursor(OLC_FOLDER_MOVE_DRAG) ;
      else if (!copy && many)
        cursor = get_folder_cursor(OLC_FOLDERS_MOVE_DRAG) ;
      else if (copy && !many)
        cursor = get_folder_cursor(OLC_FOLDER_COPY_DRAG) ;
      else
        cursor = get_folder_cursor(OLC_FOLDERS_COPY_DRAG) ;
    }
  else
    {
           if (!copy && !many)
        cursor = ds_get_predefined_cursor(OLC_DOC_MOVE_DRAG) ;
      else if (!copy && many)
        cursor = ds_get_predefined_cursor(OLC_DOCS_MOVE_DRAG) ;
      else if (copy && !many)
        cursor = ds_get_predefined_cursor(OLC_DOC_COPY_DRAG) ;
      else
        cursor = ds_get_predefined_cursor(OLC_DOCS_COPY_DRAG) ;
    }
  return(cursor) ;
}


/*  Check to see if this is a double click. Returns TRUE if the time and
 *  distance between 2 events are close enough to constitute a double
 *  click. The following X resources are used to determine the thresholds
 *  for time and distances:
 *
 *    OpenWindows.MultiClickTimeout
 *    OpenWindows.DragThreshold
 *
 *  This code was taken from the following document and slightly modified:
 *
 *  "Detecting and handling mouse double click with XView"
 *  Jim Becker -- Feb 8, 1991
 *
 *  Note: It is assummed that this routine is being called with parameters
 *        generated from two SELECT down events.
 *
 *  Returns:  TRUE  - Double click
 *            FALSE - Not a double click
 */

int
fm_is_double_click(old_sec, old_usec, sec, usec, old_x, old_y, x, y)
long old_sec, old_usec, sec, usec ;
int old_x, old_y, x, y ;
{
  static int time_threshold ;
  static int dist_threshold ;
  static short first_time = TRUE ;
         short ret_value  = FALSE ;
  int delta_time ;
  int delta_x, delta_y ;

/* Don't bother to calculate if old values are null. */

  if (old_sec == 0 && old_usec == 0) return(ret_value) ;

/* First time this is called init the thresholds. */

  if (first_time)
    {

/* Get time threshold in milliseconds. */

      time_threshold = 100 *
                       defaults_get_integer("OpenWindows.MultiClickTimeout",
                                            "OpenWindows.MultiClickTimeout",
                                            4) ;
      dist_threshold = defaults_get_integer("OpenWindows.DragThreshold",
                                            "OpenWindows.DragThreshold", 4) ;

      first_time     = FALSE ;
    }

  delta_time  = (sec - old_sec) * 1000 ;
  delta_time += usec     / 1000 ;
  delta_time -= old_usec / 1000 ;

/* Is the time within bounds? */

  if (delta_time <= time_threshold)
    {

/* Check to see if the distance is ok. */

      delta_x = (old_x > x ? old_x - x : x - old_x) ;
      delta_y = (old_y > y ? old_y - y : y - old_y) ;

      if (delta_x <= dist_threshold && delta_y <= dist_threshold)
        ret_value = TRUE ;
    }
  return(ret_value) ;
}


int
notice(msg, yes, no)              /* Put up notice. */
char *msg ;
char *yes ;                       /* Yes prompt. */
char *no ;                        /* No prompt. */
{
  if (no)
    return(notice_prompt(X->fileX[Fm->curr_wno]->frame, NULL,
                         NOTICE_MESSAGE_STRINGS,        msg, 0,
                         NOTICE_BUTTON_YES,             yes,
                         NOTICE_BUTTON_NO,              no,
                         0) == NOTICE_YES) ;
  else
    return(notice_prompt(X->fileX[Fm->curr_wno]->frame, NULL,
                         NOTICE_MESSAGE_STRINGS,        msg, 0,
                         NOTICE_BUTTON_YES,             yes,
                         0) == NOTICE_YES) ;
}


void
fm_namestripe(wno)        /* Update the frame label. */
int wno ;
{
  extern char *defaults_get_string() ;
  char buffer[MAXPATHLEN], *display, hostname[MAXHOSTNAMELEN], *ptr, *tempstr ;

  if (defaults_exists("window.header", "Window.Header"))
    {
      tempstr = (char *) defaults_get_string("window.header",
                                             "Window.Header", "") ;
      if (tempstr == NULL || *tempstr == '\0') tempstr = " " ;
      STRCPY(buffer, tempstr) ;
    }
  else
    {
      if (wno == WASTE) STRCPY(buffer, Str[(int) M_WASTE_TITLE]) ;
      else              STRCPY(buffer, Str[(int) M_MAIN_TITLE]) ;
      STRCAT(buffer, (char *) ds_relname()) ;
      STRCAT(buffer, ": ") ;

      ptr = display = DisplayString(X->display) ;
      while (*ptr != '\0') ptr++ ;
      while (*ptr != ':')  ptr-- ;
      *ptr = '\0' ;
      GETHOSTNAME(hostname, MAXHOSTNAMELEN) ;
      if (strcmp(display, hostname) && strcmp(display, "localhost") &&
          strcmp(display, "unix")   && strcmp(display, ""))
        {
          STRCAT(buffer, hostname) ;
          STRCAT(buffer, ": ") ;
        }
      *ptr = ':' ;

      if (Fm->file[wno]->filter[0])
        {
          STRCAT(buffer, (char *) Fm->file[wno]->filter) ;
          STRCAT(buffer, ": ") ;
        }

      STRCAT(buffer, (char *) Fm->file[wno]->path) ;
    }
  XV_SET(X->fileX[wno]->frame, XV_LABEL, buffer, 0) ;
}


void
fm_name_icon(frame, fname)      /* Place label on icon. */
Frame frame ;
char *fname ;
{
  Icon edit_icon ;

  if (defaults_exists("icon.footer", "Icon.Footer")) return ;

  if (*fname == '/' && fname[1])
    fname = (char *) strrchr(fname, '/') + 1 ;

  edit_icon = window_get(frame, FRAME_ICON) ;

  XV_SET(edit_icon, ICON_TRANSPARENT_LABEL, fname, 0) ;

/* Window_set actually makes a copy of all the icon fields. */

  XV_SET(frame, FRAME_ICON, edit_icon, 0) ;
}


int
fm_strlen(str)        /* Return pixel width of string. */
register char *str ;
{
  Font_string_dims dims ;

/* Get and store widths of chars in font. */

  xv_get(X->file_font, FONT_STRING_DIMS, str, &dims) ;
  return(dims.width) ;
}


void
fm_make_panpix(width, height)       /* Create offscreen panning pixmap. */
int width, height ;
{
  int depth = (int) xv_get(X->base_frame, WIN_DEPTH) ;

  X->panpix = XCreatePixmap(X->display,
                            RootWindow(X->display, DefaultScreen(X->display)),
                            width, height, depth) ;
  Fm->panw = width ;
  Fm->panh = height ;
}


void
fm_pane_to_pixmap(wno, x, y)
int wno, x, y ;
{
  Drawable xid ;

  if (wno == WNO_TREE) xid = X->treeX.xid ;
  else                 xid = X->fileX[wno]->xid ;

  if (Fm->color)
    XCopyArea(X->display, xid, X->panpix, X->GCpangc,
              x, y, Fm->panw, Fm->panh, 0, 0) ;
  else
    XCopyPlane(X->display, xid, X->panpix, X->GCpangc,
               x, y, Fm->panw, Fm->panh, 0, 0, 1L) ;
  Fm->pansx = x ;
  Fm->pansy = y ;
}


void
fm_update_pan(wno)
int wno ;
{
  Drawable xid ;
  int dx, dy ;

  if (wno == WNO_TREE) xid = X->treeX.xid ;
  else                 xid = X->fileX[wno]->xid ;

  dx = abs(Fm->panx2 - Fm->panx1) ;
  dy = abs(Fm->pany2 - Fm->pany1) ;

  if (Fm->color)
    XCopyArea(X->display, X->panpix, xid, X->GCpangc, 0, 0, Fm->panw, Fm->panh,
              Fm->pansx + Fm->panx2, Fm->pansy + Fm->pany2) ;
  else
    XCopyPlane(X->display, X->panpix, xid, X->GCpangc, 0, 0, Fm->panw, Fm->panh,
               Fm->pansx + Fm->panx2, Fm->pansy + Fm->pany2, 1L) ;

  if (Fm->panx1 <= Fm->panx2)
    {
      XFillRectangle(X->display, xid, X->GCpangc,
                     Fm->pansx + Fm->panx1, Fm->pansy + Fm->pany1,
                     dx, Fm->panh) ;
      if (Fm->pany1 <= Fm->pany2)
        XFillRectangle(X->display, xid, X->GCpangc,
                       Fm->pansx + Fm->panx2, Fm->pansy + Fm->pany1,
                       Fm->panw - dx, dy) ;
      else
        XFillRectangle(X->display, xid, X->GCpangc,
                       Fm->pansx + Fm->panx2, Fm->pansy + Fm->pany2 + Fm->panh,
                       Fm->panw - dx, dy) ;
    }
  else
    {
      XFillRectangle(X->display, xid, X->GCpangc,
                     Fm->pansx + Fm->panx2 + Fm->panw, Fm->pansy + Fm->pany1,
                     dx, Fm->panh) ;
      if (Fm->pany1 <= Fm->pany2)
        XFillRectangle(X->display, xid, X->GCpangc,
                       Fm->pansx + Fm->panx1, Fm->pansy + Fm->pany1,
                       Fm->panw - dx, dy) ;
      else
        XFillRectangle(X->display, xid, X->GCpangc,
                       Fm->pansx + Fm->panx1, Fm->pansy + Fm->pany2 + Fm->panh,
                       Fm->panw - dx, dy) ;
    }
}


void
fm_init_colormap()     /* Build colormap on color monitor. */
{
  if (Fm->color)
    {
      struct xv_singlecolor *color ;

/* Use the main frames background & foreground colors. */

      if ((color = (struct xv_singlecolor *)
           xv_get(X->base_frame, FRAME_BACKGROUND_COLOR)) != NULL)
        {

/* Position 0 holds the background color */

          Fm->red[0]   = color->red ;
          Fm->green[0] = color->green ;
          Fm->blue[0]  = color->blue ;
        }
      else ERR_EXIT(Str[(int) M_NO_BACK]) ;

      if ((color = (struct xv_singlecolor *)
           xv_get(X->base_frame, FRAME_FOREGROUND_COLOR)) != NULL)
        {

/* Position 1 holds the forground color. */

          Fm->red[1]   = color->red ;
          Fm->green[1] = color->green ;
          Fm->blue[1]  = color->blue ;
        }
      else ERR_EXIT(Str[(int) M_NO_FORE]) ;
    }
}


void
fm_set_colormap(win)      /* Create/Update colormap. */
Xv_Window win ;
{
  if (!X->cms)
    {

/* Note this assumes the frame cms has not been gimmicked by the program. */

      X->cms = ds_cms_create(X->base_frame) ;

      if (!X->cms) ERR_EXIT(Str[(int) M_CREATE_CMAP]) ;

      XV_SET(X->cms, CMS_NAME, "filemgr_cms", 0) ;
    }
  DS_SET_COLORMAP(win, X->cms, (unsigned long) DS_NULL_CMS_INDEX,
                               (unsigned long) DS_NULL_CMS_INDEX) ;
  X->xcolors = (unsigned long *) xv_get(X->cms, CMS_INDEX_TABLE) ;

/* Reset text gc to draw in proper color. */

  X->GCval.foreground = Fm->Window_fg ;
  X->GCval.background = Fm->Window_bg ;
  X->GCmask = GCForeground | GCBackground ;
  XChangeGC(X->display, X->GCftext, X->GCmask, &X->GCval) ;
}


/*  Read in the contents of a 8bit deep XPM pixmap file, converting it to
 *  a drawable. Currently this is only setup fot XPM files that are 64x64
 *  pixels or less.
 */

/* Load XPM icons using the maintained X11 library. */
unsigned long
get_xpm_content(char *path, int *image_depth)
{
    XpmAttributes attrs = {0};
    Pixmap pix = None, mask = None;
    attrs.visual = (Visual *) xv_get(X->base_frame, XV_VISUAL);
    attrs.colormap = (Colormap) xv_get(
        (Cms) xv_get(X->base_frame, WIN_CMS), CMS_CMAP_ID);
    attrs.depth = (unsigned int) xv_get(X->base_frame, WIN_DEPTH);
    attrs.valuemask = XpmVisual | XpmColormap | XpmDepth | XpmSize;
    if (XpmReadFileToPixmap(X->display, DefaultRootWindow(X->display),
                            path, &pix, &mask, &attrs) != XpmSuccess)
        return None;
    if (mask != None) XFreePixmap(X->display, mask);
    if (attrs.width > 64 || attrs.height > 64) {
        XFreePixmap(X->display, pix);
        pix = None;
    }
    *image_depth = (int) attrs.depth;
    XpmFreeAttributes(&attrs);
    return pix;
}

/*  This process forks and starts processes from within filemgr. It should
 *  be the only process to do this (apart from the find popup which is
 *  handled specially.
 *
 *  The command to be run is passed in as a string, and this is run under
 *  /bin/sh -c "command", like the system(3) library call, to correctly
 *  handle any meta characters.
 *
 *  An additional parameter determines whether filemgr waits for the child
 *  process to terminate. For such commands, stdout and stderr are
 *  redirected to an error file, which in some cases is further processed
 *  by the calling routine.
 */

int
fm_run_str(s, wait_for_child)
char *s ;
int wait_for_child ;
{
  struct rlimit rlp ;
  char buf[256] ;
  const char *Null_file = "/dev/null" ;
  int j;
#ifdef SVR4
  int status ;
#else
  union wait status ;
#endif /*SVR4*/

/*  Map stdin to /dev/null to prevent filemgr hanging when waiting for input
 *  from the child.
 */

  if ((Fm->Null_fd = open(Null_file, O_RDONLY)) == -1)
    {
      SPRINTF(buf, Str[(int) M_NO_OPEN], Null_file) ;
      perror(buf) ;
      return(-1) ;
    }

/* Map stderr and stdout to error file so we can report errors . */

  if (wait_for_child)
    if ((Fm->Err_fd = open(Fm->err_file, O_RDWR | O_CREAT, 00666)) == -1)
      {
        SPRINTF(buf, Str[(int) M_NO_OPEN], Fm->err_file) ;
        perror(buf) ;
        return(-1) ;
      }

  if ((Fm->system_pid = vfork()) < 0)
    {
      SPRINTF(buf, Str[(int) M_NO_FORK], s) ;
      perror(buf) ;
      return(-1) ;
    }

  if (Fm->system_pid == 0)           /* Child. */
    {
      if (dup2(Fm->Null_fd, 0) == -1)
        FPRINTF(stderr, Str[(int) M_NO_DUP], s, strerror(errno)) ;

      if (wait_for_child)
        if (dup2(Fm->Err_fd, 1) == -1 || dup2(Fm->Err_fd, 2) == -1)
          FPRINTF(stderr, Str[(int) M_NO_DUP], s, strerror(errno)) ;

      getrlimit(RLIMIT_NOFILE, &rlp) ;

      for (j = rlp.rlim_cur; j > 2; j--) CLOSE(j) ;

      (void) execl("/bin/sh", "sh", "-c", s, (char *) 0) ;
      FPRINTF(stderr, Str[(int) M_NO_FIND], s) ;
      _exit(1) ;
    }
  else                                /* Parent. */
    {
      if (wait_for_child)
        {
          while ((j = wait(&status)) != Fm->system_pid)
            {

	  /* Break for anything but interrupted system calls. */

              if (j < 0 && errno != EINTR) break ;
            }

          if (j < 0)
            {
              SPRINTF(buf, Str[(int) M_NO_FORK], s) ;
              perror(buf) ;
            }

/*  Check status for return error code.  Just because the fork
 *  exited w/o a hitch does not mean the command worked.
 */
          if (WIFSTOPPED(status))
            {
              if (Fm->debug) FPRINTF(stderr, Str[(int) M_CHILD_STOP], s) ;
              j = -1 ;                        /* Signal failure. */
            }

          if (WIFSIGNALED(status))
            {
              if (Fm->debug) FPRINTF(stderr, Str[(int) M_CHILD_SIGNAL], s) ;
              j = -1 ;                        /* Signal failure. */
            }

          if (WIFEXITED(status) && (WEXITSTATUS(status) != 0))
            {
              if (Fm->debug) FPRINTF(stderr, Str[(int) M_CHILD_EXIT], s) ;
              j = -1 ;                        /* Signal failure. */
            }
          CLOSE(Fm->Err_fd) ;
          CLOSE(Fm->Null_fd) ;

          if (j == Fm->system_pid) j = 0 ;         /* All ok. */
          Fm->system_pid = 0 ;

/* Zero is sucessful, non-zero indicates error. */

          return(j) ;
        }
      else
        {
	  extern void fm_associate_value(void *, void *);

	  /* associate Null_fd with pid so it can be closed when the child dies */
	  fm_associate_value( (void *)(intptr_t)Fm->system_pid, (void *)(intptr_t)Fm->Null_fd );
          notify_set_wait3_func(X->base_frame, run_child, Fm->system_pid) ;
          return(0) ;
        }
    }
  return(j) ;
}


/*ARGSUSED*/
static Notify_value
run_child(frame, pid, status, rusage)
Notify_client frame ;
int pid ;
#ifdef SVR4
int *status ;
#else
union wait *status ;
#endif /*SVR4*/
void *rusage ;
{
  File_Pane *f ;
  int devno, wno ;

  if (WIFEXITED(*status))
    {
	int null_fd;
	extern void *fm_value_from_id( void *id );
	extern void fm_disassociate_value( void *val );

	/* the child dies, make sure the Null_fd gets closed */
	null_fd = (int)(intptr_t) fm_value_from_id( (void *)(intptr_t)pid );
	CLOSE(null_fd);
	fm_disassociate_value( (void *)(intptr_t)null_fd );

/*  Check to see if this was a format_floppy command that has just finished.
 *  If so, then decrement the busy count for every window watching that floppy
 *  device, and clear the entry in format_pid.
 */

      for (devno = 0; devno < MAXFLOPPY; devno++)
        if (Fm->format_pid[devno] == pid)
          {
            for (wno = 0; wno < Fm->num_win; wno++)
              {
                f = Fm->file[wno] ;
                if ((f->mtype == FM_FLOPPY || f->mtype == FM_DOS) &&
                     f->devno == devno)
                busy_cursor(FALSE, wno, &f->busy_count) ;
              }
            Fm->format_pid[devno] = 0 ;
            set_timer(Fm->interval) ;
	    if (Fm->show_floppy)
            	check_media(FM_FLOPPY, MAXFLOPPY, &Fm->floppy_mask, 1) ;
          }
      return(NOTIFY_DONE) ;
    }
  else return(NOTIFY_IGNORED) ;
}


/* Create a generic icon entry. */

void
create_generic_icon_entry(i, bits, mask_bits)
int i ;
unsigned short *bits ;
unsigned short *mask_bits ;
{
  X->Gen_icon_image[i] = (Image_Ptr) LINT_CAST(fm_malloc(sizeof(Image_Object))) ;
  if (!X->Gen_icon_image[i]) ERR_EXIT(Str[(int) M_CREATE_GEN_ICON]) ;

  X->Gen_icon_mask[i] = (Image_Ptr) LINT_CAST(fm_malloc(sizeof(Image_Object))) ;
  if (!X->Gen_icon_mask[i]) ERR_EXIT(Str[(int) M_CREATE_GEN_MASK]) ;

  X->Gen_icon_image[i]->image = xv_create(0,                 SERVER_IMAGE,
                                       XV_WIDTH,          GLYPH_WIDTH,
                                       XV_HEIGHT,         GLYPH_HEIGHT,
                                       SERVER_IMAGE_BITS, bits,
                                       0) ;

  X->Gen_icon_mask[i]->image = xv_create(0,                 SERVER_IMAGE,
                                      XV_WIDTH,          GLYPH_WIDTH,
                                      XV_HEIGHT,         GLYPH_HEIGHT,
                                      SERVER_IMAGE_BITS, mask_bits,
                                      0) ;

  X->Gen_icon_image[i]->xid = (int) xv_get(X->Gen_icon_image[i]->image, XV_XID) ;
  X->Gen_icon_mask[i]->xid  = (int) xv_get(X->Gen_icon_mask[i]->image,  XV_XID) ;

  X->Gen_icon_image[i]->name = NULL ;        /* unused */
  X->Gen_icon_mask[i]->name  = NULL ;        /* unused */
}


static Image_Ptr
get_generic_icon(type, mask)          /* Return generic icon. */
BYTE type ;
Boolean mask ;                        /* Get the mask */
{
  if (type == FT_UNCHECKED) type = FT_DOC ;

  if (!X->Gen_icon_image[0]) init_generic_icons() ;

  CEDB1("CE- using generic %sicon\n", (mask ? "mask " : "")) ;

  if (mask) return(X->Gen_icon_mask[type]) ;
  else      return(X->Gen_icon_image[type]) ;
}


Server_image
get_generic_icon_image(type, mask)      /* Return generic icon's image. */
BYTE type ;
Boolean mask ;                          /* Get the mask xid? */
{
  Image_Ptr ip = get_generic_icon(type, mask) ;
  return(ip->image) ;
}


Drawable
get_generic_icon_xid(type, mask)      /* Return generic icon's image. */
BYTE type ;
Boolean mask ;                        /* Get the mask xid? */
{
  Image_Ptr ip = get_generic_icon(type, mask) ;
  return(ip->xid) ;
}


/* Create a generic list icon entry. */

void
create_generic_list_icon_entry(i, bits, mask_bits)
int i ;
unsigned short *bits, *mask_bits ;
{
  X->Gen_list_image[i] = (Image_Ptr) LINT_CAST(fm_malloc(sizeof(Image_Object))) ;
  if (!X->Gen_list_image[i]) ERR_EXIT(Str[(int) M_CREATE_GEN_ICON]) ;

  X->Gen_list_mask[i] = (Image_Ptr) LINT_CAST(fm_malloc(sizeof(Image_Object))) ;
  if (!X->Gen_list_mask[i]) ERR_EXIT(Str[(int) M_CREATE_GEN_MASK]) ;

  X->Gen_list_image[i]->image = xv_create(0,                 SERVER_IMAGE,
                                       XV_WIDTH,          16,
                                       XV_HEIGHT,         16,
                                       SERVER_IMAGE_BITS, bits,
                                       0) ;

  X->Gen_list_mask[i]->image = xv_create(0,                 SERVER_IMAGE,
                                      XV_WIDTH,          16,
                                      XV_HEIGHT,         16,
                                      SERVER_IMAGE_BITS, mask_bits,
                                      0) ;

  X->Gen_list_image[i]->xid  = (int) xv_get(X->Gen_list_image[i]->image, XV_XID) ;
  X->Gen_list_mask[i]->xid   = (int) xv_get(X->Gen_list_mask[i]->image,  XV_XID) ;

  X->Gen_list_image[i]->name = NULL ;        /* Unused. */
  X->Gen_list_mask[i]->name  = NULL ;        /* Unused. */

  X->Gen_list_image[i]->hits = 0 ;
  X->Gen_list_mask[i]->hits  = 0 ;
}


static Image_Ptr
get_generic_list_icon(type, mask)     /* Return generic list icon. */
BYTE type ;
Boolean mask ;                        /* Get the mask */
{
  if (type == FT_UNCHECKED) ERR_EXIT(Str[(int) M_UNCHECK_LICON]) ;

  if (!X->Gen_list_image[0]) init_generic_list_icons() ;

  CEDB1("CE- using generic %sicon\n", (mask ? "mask " : "")) ;

  if (mask) return(X->Gen_list_mask[type]) ;
  else      return(X->Gen_list_image[type]) ;
}


Drawable
get_generic_list_icon_xid(type, mask)    /* Return generic icon's image. */
BYTE type ;
Boolean mask ;                           /* Get the mask xid? */
{
  Image_Ptr ip = get_generic_list_icon(type, mask) ;
  return(ip->xid) ;
}


/* Comparison function for RB tree (icon cache). */

Comparison
compare_str(key, data)
char *key ;
register caddr_t data ;
{
  int result = strcmp(key, (char *) ((Image_Ptr) data)->name) ;

       if (result == 0) return(equal) ;
  else if (result < 0)  return(less) ;
  else                  return(greater) ;
}


/* Data return function for RB tree (icon cache). */

caddr_t
get_icon_name(data)
caddr_t data ;
{
  return((caddr_t) ((Image_Ptr) data)->name) ;
}


static Image_Ptr
create_icon_entry(file)           /* Create an icon entry. */
char *file ;
{
  Image_Ptr node = NULL ;
  char errmsg[200], iconpath[MAXPATHLEN] ;

  node = (Image_Ptr) LINT_CAST(fm_malloc(sizeof(Image_Object))) ;
  if (!node) ERR_EXIT(Str[(int) M_NO_ALLOC_TICON]) ;

/* XXX: saving the unexpanded pathname.  May cause problems later */

  node->name = (WCHAR *) strdup(file) ;

/*  Expand any environment variables in the icon pathname */

  ds_expand_pathname(file, iconpath) ;

  node->image = (Server_image) icon_load_svrim(iconpath, errmsg) ;
  if (!node->image)
    {

/* Keep bad entry so next attempt to get it will be stopped. */

      FPRINTF(stderr, Str[(int) M_ERROR_MES], errmsg) ;
      node->xid = BAD_ICON_XID ;
    }
  else node->xid = (int) xv_get(node->image, XV_XID) ;

  node->hits = 0 ;        /* Save for performance comparisons, incr later. */
  return(node) ;
}


static Image_Ptr
get_icon(fpo, mask)                   /* Get the file's icon struct ptr. */
File_Pane_Object *fpo ;
Boolean mask ;                        /* Get the mask xid? */
{
  char *iconfile = NULL ;
  Image_Ptr ip = NULL ;

  CEDB2("\nCE- getting %sicon for %s\n", (mask ? "mask " : ""), fpo->name) ;

/* If file is a bad link, show the broken link icon. */

  if (fpo->type == FT_BAD_LINK)
    return(get_generic_icon(fpo->type, mask)) ;

/*  Try to get the icon's image from the cache first. If this fails, then
 *  get the generic icon for this file's type.
 */

  iconfile = get_tns_attr(fpo->tns_entry,
                          (mask ? Fm->ceo->type_icon_mask : Fm->ceo->type_icon)) ;

  if (!iconfile)
    {
      CE_ENTRY entry = NULL ;

/* XXX -- need to add FT_BADLINK, etc. to CE */

           if (fpo->type == FT_DIR) entry = Fm->ceo->generic_dir ;
      else if (fpo->type == FT_APP) entry = Fm->ceo->generic_app ;
      else if (fpo->type == FT_DOC) entry = Fm->ceo->generic_doc ;

      CEDB1("CE- no tns %siconfile found\n", (mask ? "mask " : "")) ;
      if (entry)
        iconfile = get_tns_attr(entry,
                     (mask ? Fm->ceo->type_icon_mask : Fm->ceo->type_icon)) ;
      if (!iconfile)
        CEDB1("CE- no generic tns %siconfile found\n", (mask ? "mask " : "")) ;
    }

  if (iconfile)
    {
      ip = (Image_Ptr) LINT_CAST(rb_lookup(IconTree, (caddr_t) iconfile)) ;
      if (!ip)
        {
          ip = create_icon_entry(iconfile) ;
          rb_insert(IconTree, (caddr_t) ip, (caddr_t) iconfile) ;
        }
      CEDB2("CE- using cache entry, icon=%s, hits=%d\n", ip->name, ++ip->hits) ;
    }

  if (!ip || ip->xid == BAD_ICON_XID)
    ip = get_generic_icon(fpo->type, mask) ;

  return(ip) ;
}


int
get_icon_xid(fpo, mask)            /* Get the file's icon image xid. */
File_Pane_Object *fpo ;
Boolean mask ;                     /* Get the mask xid? */
{
  Image_Ptr ip = get_icon(fpo, mask) ;
  return(ip->xid) ;
}


Server_image
get_icon_server_image(fpo, mask)      /* Get the file's icon image. */
File_Pane_Object *fpo ;
Boolean mask ;                        /* Get the mask xid? */
{
  Image_Ptr ip = get_icon(fpo, mask) ;
  return(ip->image) ;
}


static XPixel
convert_color_string(s)               /* Get the default file's fg color. */
char *s ;
{
  XPixel cms_index, x_index = DS_NULL_CMS_INDEX ;
  Xv_Singlecolor color ;
  int r, g, b ;

  if (sscanf(s, "%d %d %d", &r, &g, &b) == 3)
    {
      color.red   = (u_char) r ;
      color.green = (u_char) g ;
      color.blue  = (u_char) b ;
      cms_index   = (XPixel) ds_cms_index(X->cms, &color) ;
      x_index     = (XPixel) ds_x_index(cms_index) ;
    }
  return(x_index) ;
}


unsigned long
get_default_fg_index(type)     /* Get the default file's fg color index. */
int type ;
{
  XPixel fg = DS_NULL_CMS_INDEX ;

  if (Fm->color)
    {
      CE_ENTRY entry = NULL ;
      char *str = NULL ;

           if (type == FT_APP) entry = Fm->ceo->generic_app ;
      else if (type == FT_DOC) entry = Fm->ceo->generic_doc ;
      else                     entry = Fm->ceo->generic_dir ;

      str = get_tns_attr(entry, Fm->ceo->type_fgcolor) ;
      if (str)
        {
          fg = convert_color_string(str) ;
          CEDB1("CE- default foreground color = %s", str) ;
          CEDB1(", index = %d\n", fg) ;
        }
      else
        {
          CEDB("CE- no generic tns foreground color found\n") ;
          fg = (XPixel) xv_get(X->cms, CMS_FOREGROUND_PIXEL) ;
        }
    }

  if (fg == DS_NULL_CMS_INDEX)
    fg = (XPixel) xv_get((Cms) xv_get(X->base_frame, WIN_CMS),
                                      CMS_FOREGROUND_PIXEL) ;

  return(fg) ;
}


unsigned long
get_fg_index(fpo)               /* Get the file's fg color index. */
File_Pane_Object *fpo ;
{
  XPixel index = DS_NULL_CMS_INDEX ;
  char *str = NULL ;

/*  Get the icon's foreground color from the CE.  If this fails, then use
 *  the default colors.
 */

  if (Fm->color)
    {
      str = get_tns_attr(fpo->tns_entry, Fm->ceo->type_fgcolor) ;
      if (!str) CEDB("CE- no tns foreground color found\n") ;
      else
        {
          index = convert_color_string(str) ;
          CEDB1("CE- foreground color = %s", str) ;
          CEDB1(", index = %d\n", index) ;
        }
    }

  if (index == DS_NULL_CMS_INDEX)
    index = get_default_fg_index(fpo->type) ;

  return(index) ;
}


unsigned long
get_default_bg_index(type)     /* Get the default file's bg color index. */
int type ;
{
  XPixel bg = DS_NULL_CMS_INDEX ;

  if (Fm->color)
    {
      CE_ENTRY entry = NULL ;
      char *str = NULL ;

           if (type == FT_APP) entry = Fm->ceo->generic_app ;
      else if (type == FT_DOC) entry = Fm->ceo->generic_doc ;
      else                     entry = Fm->ceo->generic_dir ;

      str = get_tns_attr(entry, Fm->ceo->type_bgcolor) ;
      if (str)
        {
          bg = convert_color_string(str) ;
          CEDB1("CE- default background color = %s", str) ;
          CEDB1(", index = %d\n", bg) ;
        }
      else
        {
          CEDB("CE- no generic tns background color found\n") ;
          bg = (XPixel) xv_get(X->cms, CMS_BACKGROUND_PIXEL) ;
        }
    }

  if (bg == DS_NULL_CMS_INDEX)
    bg = (XPixel) xv_get((Cms) xv_get(X->base_frame, WIN_CMS),
                                      CMS_BACKGROUND_PIXEL) ;
  return(bg) ;
}


unsigned long
get_bg_index(fpo)              /* Get the file's bg color index. */
File_Pane_Object *fpo ;
{
  XPixel index = DS_NULL_CMS_INDEX ;
  char *str = NULL ;

/*  Get the icon's background color from the CE.  If this fails, then use
 *  the default colors.
 */

  if (Fm->color)
    {
      str = get_tns_attr(fpo->tns_entry, Fm->ceo->type_bgcolor) ;
      if (!str) CEDB("CE- no tns background color found\n") ;
      else
        {
          index = convert_color_string(str) ;
          CEDB1("CE- background color = %s", str) ;
          CEDB1(", index = %d\n", index) ;
        }
    }

  if (index == DS_NULL_CMS_INDEX)
    index = get_default_bg_index(fpo->type) ;

  return(index) ;
}


/*ARGSUSED*/
void
fm_create_folder_canvas(parent, wno)    /* Create folder canvas */
Frame parent ;
register int wno ;
{
  Canvas c ;
  Scrollbar hsb, vsb ;
  Xv_Window first_pw, fv ;

  c = X->fileX[wno]->canvas ;
  set_scrollbar_attr(wno, FM_H_SBAR, FM_SB_SPLIT, TRUE) ;
  set_scrollbar_attr(wno, FM_V_SBAR, FM_SB_SPLIT, TRUE) ;

  hsb = xv_get(c, OPENWIN_HORIZONTAL_SCROLLBAR,
               xv_get(c, OPENWIN_NTH_VIEW, 0)) ;
  vsb = xv_get(c, OPENWIN_VERTICAL_SCROLLBAR,
               xv_get(c, OPENWIN_NTH_VIEW, 0)) ;

  XV_SET(c,
         CANVAS_RETAINED,              FALSE,

/* Sets BitGravity = ForgetGravity. */

         CANVAS_FIXED_IMAGE,           FALSE,
         CANVAS_X_PAINT_WINDOW,        TRUE,
         CANVAS_AUTO_SHRINK,           FALSE,
         CANVAS_AUTO_CLEAR,            FALSE,
         OPENWIN_SPLIT,
           OPENWIN_SPLIT_INIT_PROC,    fm_folder_split,
           OPENWIN_SPLIT_DESTROY_PROC, fm_folder_join,
           0,
         XV_HELP_DATA,  "filemgr:File_Pane",
         XV_KEY_DATA,   Fm->wno_key,  wno,
         0) ;

  X->fileX[wno]->pw  = first_pw = (Xv_Window) canvas_paint_window(c) ;
  X->fileX[wno]->xid = xv_get(X->fileX[wno]->pw, XV_XID) ;
  Fm->file[wno]->pw_width  = get_pw_attr(wno, FM_WIN_WIDTH) ;
  Fm->file[wno]->pw_height = get_pw_attr(wno, FM_WIN_HEIGHT) ;

  XV_SET(first_pw,
         WIN_CONSUME_EVENTS,
           WIN_ASCII_EVENTS,
           WIN_META_EVENTS,
           LOC_WINENTER,
           LOC_WINEXIT,
           LOC_DRAG,
           ACTION_DRAG_MOVE,
           ACTION_DRAG_COPY,
           ACTION_DRAG_PREVIEW,
           SHIFT_CTRL,
           ACTION_PROPS,
           ACTION_COPY,
           ACTION_PASTE,
           ACTION_FIND_FORWARD,
           ACTION_FIND_BACKWARD,
           ACTION_CUT,
           ACTION_GO_LINE_BACKWARD,
           ACTION_GO_LINE_END,
           ACTION_GO_PAGE_FORWARD,
           ACTION_GO_PAGE_BACKWARD,
           0,
         WIN_IGNORE_EVENTS,
           LOC_MOVE,
           0,
         XV_KEY_DATA, Fm->wno_key,  wno,
         WIN_BIT_GRAVITY, ForgetGravity,
         0) ;

  if (X->def_cursor == 0)
    X->def_cursor = xv_get(first_pw, WIN_CURSOR, NULL) ;

  create_drop_site(wno, first_pw, FALSE) ;
  NOTIFY_INTERPOSE_EVENT_FUNC(first_pw, pw_folder_event, NOTIFY_IMMEDIATE) ;
  NOTIFY_INTERPOSE_EVENT_FUNC(c, folder_event, NOTIFY_SAFE) ;
  NOTIFY_INTERPOSE_EVENT_FUNC(c, folder_event, NOTIFY_IMMEDIATE) ;

  NOTIFY_INTERPOSE_EVENT_FUNC(xv_get(hsb, SCROLLBAR_NOTIFY_CLIENT),
                              scroll_event, NOTIFY_SAFE) ;
  NOTIFY_INTERPOSE_EVENT_FUNC(xv_get(vsb, SCROLLBAR_NOTIFY_CLIENT),
                              scroll_event, NOTIFY_SAFE) ;

/* X Drawing Stuff. */

#ifdef OW_I18N
  X->font = xv_get(first_pw, WIN_FONT) ;
  X->font_set = (XFontSet) xv_get(X->font, FONT_SET_ID) ;
#else
  X->font_set = 0 ;
#endif /*OW_I18N*/

  XSetFont(X->display, X->GCftext,
           xv_get(xv_get(first_pw, WIN_FONT), XV_XID)) ;

  if (Fm->color)
    fm_set_colormap(X->fileX[wno]->canvas) ;

  c = X->fileX[wno]->pathX.canvas ;
  fv = xv_get(c, OPENWIN_NTH_VIEW, 0) ;
  XV_SET(xv_get(c, OPENWIN_HORIZONTAL_SCROLLBAR, fv),
         SCROLL_LINE_HEIGHT, GLYPH_WIDTH,
         0) ;

  XV_SET(c,
         CANVAS_RETAINED,       FALSE,
         CANVAS_FIXED_IMAGE,    FALSE,
         CANVAS_X_PAINT_WINDOW, TRUE,
         CANVAS_AUTO_SHRINK,    FALSE,
         CANVAS_AUTO_CLEAR,     FALSE,
         XV_HELP_DATA,          "filemgr:Path_Pane",
         XV_KEY_DATA,           Fm->wno_key, wno,
         0) ;

  X->fileX[wno]->pathX.pw = first_pw = (Xv_Window) canvas_paint_window(c) ;
  X->fileX[wno]->pathX.xid =
    (Drawable) xv_get(X->fileX[wno]->pathX.pw, XV_XID) ;

  add_frame_menu_accel(wno, FM_FILE_MENU) ;
  add_frame_menu_accel(wno, FM_EDIT_MENU) ;

  adjust_path_height(wno) ;
  set_pw_events(wno, first_pw) ;
  create_drop_site(wno, first_pw, TRUE) ;
  NOTIFY_INTERPOSE_EVENT_FUNC(first_pw, pw_tree_event, NOTIFY_IMMEDIATE) ;
  NOTIFY_INTERPOSE_EVENT_FUNC(c, tree_event, NOTIFY_SAFE) ;
  NOTIFY_INTERPOSE_EVENT_FUNC(c, tree_event, NOTIFY_IMMEDIATE) ;
}


/*ARGSUSED*/
static void
fm_folder_join(win)
Xv_Window win ;
{
  int wno = (int) xv_get(win, XV_KEY_DATA, Fm->wno_key) ;

  /*
  **  This is a bit of a hack, but ... a bug was occurring where when the canvas
  **  had been split and the user selected "join views" from the *new* scrollbar
  **  filemgr would crash with an X error.  The problem was the canvas that was
  **  destroyed as a result of the join still received events - causing grief.
  **  The semi-hack fix is to clear the paint window handle when we receive the
  **  join event and check for that in folder_event.
  */
  X->fileX[wno]->pw = 0;
  fm_scrollbar_scroll_to(wno, FM_V_SBAR, 0) ;
  resize_window_drop_sites(wno, FALSE) ;
}


/*ARGSUSED*/
static void
fm_folder_split(orig, new, pos)
Xv_Window orig, new ;
int pos ;
{
  int wno ;
  Xv_Window npw, opw ;

  opw = (Xv_Window) xv_get(orig, CANVAS_VIEW_PAINT_WINDOW) ;
  wno = (int)       xv_get(opw, XV_KEY_DATA, Fm->wno_key) ;
  npw = (Xv_Window) xv_get(new, CANVAS_VIEW_PAINT_WINDOW) ;

  XV_SET(npw,
         WIN_CONSUME_PICK_EVENTS,
           WIN_ASCII_EVENTS,
           WIN_META_EVENTS,
           LOC_WINENTER,
           LOC_WINEXIT,
           LOC_DRAG,
           ACTION_DRAG_MOVE,
           ACTION_DRAG_COPY,
           ACTION_DRAG_PREVIEW,
           SHIFT_CTRL,
           ACTION_PROPS,
           ACTION_COPY,
           ACTION_PASTE,
           ACTION_FIND_FORWARD,
           ACTION_FIND_BACKWARD,
           ACTION_CUT,
           0,
         XV_KEY_DATA,     Fm->wno_key, wno,
         WIN_BIT_GRAVITY, ForgetGravity,
         0) ;

  create_drop_site(wno, npw, FALSE) ;      /* Create dnd drop site on pw. */

  NOTIFY_INTERPOSE_EVENT_FUNC(npw, pw_folder_event, NOTIFY_SAFE) ;
  NOTIFY_INTERPOSE_EVENT_FUNC(npw, pw_folder_event, NOTIFY_IMMEDIATE) ;
}


void
do_new_dnd_drop(ec)       /* Procedure to handle new dnd drops. */
Event_Context ec ;
{
  Selection_requestor sel ;
  Catch catch ;

  sel = create_seln_requestor(ec) ;

/*  This is a bogus hack, but it serves a purpose.  Let's check
 *  to see if any of the files in any of our active pitches have
 *  the destination of the drop as a substring.  If so, then there
 *  is a potential cycle, and we want to disallow the drop operation.
 */

  catch = (Catch) xv_get(sel, XV_KEY_DATA, Fm->Catch_key) ;

/*  To acknowledge the drop and to associate the rank of the source's
 *  selection to our requestor selection object, we call dnd_decode_drop().
 */

  if (dnd_decode_drop(sel, X->event) != XV_ERROR)
    {
      if (catch->destdir && compare_with_plist(catch->destdir))
        {
          fm_msg(TRUE, Str[(int) M_DND_SOURCE], catch->destdir) ;
          really_stop_dnd(sel, FALSE) ;
          return ;
        }

      if (catch->over_file && compare_with_plist(catch->over_file))
        {
          fm_msg(TRUE, Str[(int) M_DND_SOURCE], catch->over_file) ;
          really_stop_dnd(sel, FALSE) ;
          return ;
        }
      load_from_dnd(sel) ;
    }
  else FPRINTF(stderr, Str[(int) M_DND_ERROR], __FILE__) ;
}


/*  Create/initialize the event context describing the latest event
 *  occurring on the file pane.  This include information about
 *  button presses, x & y location, wno, etc.
 */

Event_Context
setup_event_context(canvas, event, ispath)
Canvas canvas ;
Event *event ;
Boolean ispath ;
{
  static Event_Context ec = NULL ;

/* Create the event context if needed. */

  if (!ec)
    {
      ec = (Event_Context) LINT_CAST(malloc(sizeof(Event_Context_Object))) ;
      MEMSET((char *) ec, 0, sizeof(Event_Context_Object)) ;
      ec->state = CLEAR ;
    }

  if (!event)
    {
#ifdef DEBUG_EVENT
      PRINTF("setup_event_context: NULL event handle passed\n") ;
#endif /*DEBUG_EVENT*/
      return(ec) ;
    }

  ec->id      = event_action(event) ;
  ec->meta    = event_meta_is_down(event) ;
  ec->control = event_ctrl_is_down(event) ;
  ec->down    = event_is_down(event) ;
  ec->wno     = (int) xv_get(canvas, XV_KEY_DATA, Fm->wno_key) ;
  ec->ispath  = ispath ;
  X->event    = event ;

  if (ec->wno != WNO_TREE) Fm->curr_wno = ec->wno ;

  if (ec->id == ACTION_SELECT || ec->id == ACTION_MENU ||
      ec->id == ACTION_ADJUST || ec->id == LOC_DRAG    ||
      ec->id == ACTION_DRAG_MOVE ||
      ec->id == ACTION_DRAG_COPY ||
      ec->id == ACTION_DRAG_PREVIEW )
    {
      ec->old_x = ec->x ;
      ec->old_y = ec->y ;
      ec->x     = event_x(event) ;
      ec->y     = event_y(event) ;
    }

/* Redefine the event id if it is an ascii or function key press. */

  if (event_is_ascii(event))
    {
      ec->ch = ec->id ;
      ec->id = MY_ASCII ;
    }

  if (event_is_key_left(event)          ||
      ec->id == ACTION_GO_LINE_BACKWARD ||
      ec->id == ACTION_GO_LINE_END      ||
      ec->id == ACTION_GO_PAGE_FORWARD  ||
      ec->id == ACTION_GO_PAGE_BACKWARD) ec->id = MY_FKEY ;

#ifdef DEBUG_EVENT
  PRINTF("event : x=%d, y=%d, old_x=%d, old_y=%d, start_x=%d, start_y=%d\n",
         ec->x, ec->y, ec->old_x, ec->old_y, ec->start_x, ec->start_y) ;
  PRINTF("        down=%d, meta=%d, control=%d, id=%d, event=%d\n\n",
         ec->down, ec->meta, ec->control, ec->id, event_action(event)) ;
#endif /*DEBUG_EVENT*/

       if (ec->id == LOC_DRAG)                 ec->fm_id = E_DRAG ;
  else if (ec->id == ACTION_SELECT)            ec->fm_id = E_SELECT ;
  else if (ec->id == ACTION_ADJUST)            ec->fm_id = E_ADJUST ;
  else if (ec->id == ACTION_MENU)              ec->fm_id = E_MENU ;
  else if (ec->id == WIN_RESIZE)               ec->fm_id = E_RESIZE ;
  else if (ec->id == MY_FKEY)                  ec->fm_id = E_FKEY ;
  else if (ec->id == MY_ASCII)                 ec->fm_id = E_ASCII ;
  else if (ec->id == ACTION_DRAG_COPY)         ec->fm_id = E_DRAG_COPY ;
  else if (ec->id == ACTION_DRAG_MOVE)         ec->fm_id = E_DRAG_MOVE ;
  else if (ec->id == ACTION_DRAG_PREVIEW)      ec->fm_id = E_DRAG_PREVIEW ;
  else if (ec->id == ACTION_DRAG_LOAD)         ec->fm_id = E_DRAG_LOAD ;
  else if (ec->id == LOC_WINENTER)             ec->fm_id = E_WINENTER ;
  else if (ec->id == LOC_WINEXIT)              ec->fm_id = E_WINEXIT ;
  else if (ec->id == SCROLLBAR_REQUEST)        ec->fm_id = E_SCROLL ;
  else                                         ec->fm_id = E_OTHER ;

  return(ec) ;
}


/*  User dropped an item over the root window -- start it up using its
 *  defined OPEN method.
 */

static void
open_item(ec, dnd_object)
Event_Context ec ;
Dnd dnd_object ;
{
  enum media_type mtype ;
  Drawable xid ;
  Pitch p = (Pitch) xv_get(dnd_object, XV_KEY_DATA, Fm->Pitch_key) ;
  int devno, i, info[MAXINFO], wno, x, y ;

  Window root, child ;
  int rx, ry, wx, wy ;
  unsigned int kbut ;

  if (!p->files)
    {
      fm_msg(TRUE, Str[(int) M_NOGET_DND]) ;
      return ;
    }

  /* make sure the ec->wno and curr_wno is pointing to the  original
     dragged window since the drag action across other path/folder panel
     will cause ec->wno to change */
  ec->wno = p->files->wno;
  if (p->files->wno != WNO_TREE) Fm->curr_wno = p->files->wno;

/* Get the current X/Y location of the pointer, and fudge it into the event. */

  if (ec->wno == WNO_TREE) xid = X->treeX.xid ;
  else                     xid = X->fileX[ec->wno]->xid ;
  XQueryPointer(X->display, xid, &root, &child, &rx, &ry, &wx, &wy, &kbut) ;
  ec->x = rx ;
  ec->y = ry ;

  Fm->stopped = FALSE ;
  for (i = 0; i < p->num_items; i++)
    {

/*  If we are dropping multiple objects onto the desktop, then we need to
 *  stagger them. If it looks like we're going off the screen, then don't
 *  increment, just dump one on top of the other.
 */

      wno = p->files[i].wno ;
      check_stop_key(wno) ;
      if (Fm->stopped) break ;
      x = ((ec->x + (i * FDR_OFF)) < Fm->screen_width)
          ? (ec->x + (i * FDR_OFF)) : ec->x ;
      y = ((ec->y + (i * FDR_OFF)) < Fm->screen_height)
          ? (ec->y + (i * FDR_OFF)) : ec->y ;

      if (p->files[i].type == FT_UNCHECKED)
        {

/* Tree folder  has been dropped -- open subframe. */

          info[0] = info[1] = info[2] = info[5] = info[6] = -1 ;
          info[3] = x ;
          info[4] = y ;
          devno   = 0 ;
          mtype   = FM_DISK ;
          if (ec->wno != WNO_TREE)
            {
              devno = Fm->file[wno]->devno ;
              mtype = Fm->file[wno]->mtype ;
            }
          FM_NEW_FOLDER_WINDOW("", p->files[i].name, NULL,
                               mtype, devno, TRUE, info) ;
        }
      else

/*  Open file in window[wno] at pos x, y, 1-copy only, using default
 *  open method, using subframes, no editor, no args.
 */
        open_object(Fm->file[wno]->object[p->files[i].index],
                    (char *) Fm->file[wno]->path,
                    wno, x, y, NULL, TRUE, TRUE, FALSE) ;
    }
}


/* Start up a dnd drag using the new selection/dnd service package. */

void
start_dnd(ec)
Event_Context ec ;
{
  Boolean clean_now = TRUE ;          /* Clean up dnd object immediately? */

  if (ec->wno != WNO_TREE && !ec->ispath) make_drag_cursor(ec) ;
  X->dnd_object = create_dnd_object(ec) ;

  if (!X->dnd_object) return ;

  switch (dnd_send_drop(X->dnd_object))
    {
      case XV_OK              : clean_now = FALSE ;
                                break ;
      case DND_ROOT           : open_item(ec, X->dnd_object) ;
                                break ;
      case DND_TIMEOUT        : if (Fm->Show_dnd_error)
                                  fm_msg(TRUE, Str[(int) M_DND_TIMEOUT]) ;
                                break ;
      case DND_ILLEGAL_TARGET : fm_msg(TRUE, Str[(int) M_DND_ILLEGAL]) ;
                                break ;
      case DND_SELECTION      : fm_msg(TRUE, Str[(int) M_DND_BADSEL]) ;
                                break ;
      case XV_ERROR           : fm_msg(TRUE, Str[(int) M_DND_FAIL]) ;
    }

  if (clean_now)
    destroy_dnd_object(X->dnd_object) ;
}


#ifdef DEBUG
static int saved_repaints      = 0 ;
static int last_saved_repaints = 0 ;
#endif /*DEBUG*/

XRectangle
compress_damage(display, xid, xrects)
Display *display ;
Xv_Window xid ;
Xv_xrectlist *xrects ;
{
  XEvent ev ;
  XRectangle *xr = xrects->rect_array ;
  XRectangle clip ;

  int i ;
  int minx = xr->x ;
  int miny = xr->y ;
  int maxx = minx + xr->width ;
  int maxy = miny + xr->height ;

/* Look through this expose event building the bbox. */

  for (i = 1, xr++; i < xrects->count; i++, xr++)
    {
      if (xr->x < minx) minx = xr->x ;
      if (xr->y < miny) miny = xr->y ;
      if ((int) (xr->x + xr->width) > maxx)  maxx = xr->x + xr->width ;
      if ((int) (xr->y + xr->height) > maxy) maxy = xr->y + xr->height ;
    }

  XSync(display, 0) ;

/* Look through pending expose events building the bbox. */

  while (XPending(display) && (XPeekEvent(display, &ev),
         (ev.type == Expose && ev.xany.window == xid)))
    {
      XNextEvent(display, &ev) ;
      if (ev.xexpose.x < minx) minx = ev.xexpose.x ;
      if (ev.xexpose.y < miny) miny = ev.xexpose.y ;
      if ((int) (ev.xexpose.x + ev.xexpose.width) > maxx)
        maxx = ev.xexpose.x + ev.xexpose.width ;
      if ((int) (ev.xexpose.y + ev.xexpose.height) > maxy)
        maxy = ev.xexpose.y + ev.xexpose.height ;

#ifdef DEBUG
      ++saved_repaints ;
#endif /*DEBUG*/
    }

#ifdef DEBUG
  if (saved_repaints - last_saved_repaints > 8)
    {
      FPRINTF(stderr,  Str[(int) M_REPAINTS],
              saved_repaints - last_saved_repaints, saved_repaints) ;
    }
  last_saved_repaints = saved_repaints ;
#endif /*DEBUG*/

/* Confine drawing to the extent of the damage. */

  clip.x = minx ;
  clip.y = miny ;
  clip.width = maxx - minx ;
  clip.height = maxy - miny ;
  return(clip) ;
}


/* Repaint Proc - displays files in a given folder. */

void
do_display_folder(canvas, pw, display, xid, xrects)
Canvas canvas ;                      /* Canvas handle. */
Xv_Window pw ;                       /* Pixwin/Paint window. */
Display *display ;                   /* Server connection. */
Window xid ;                         /* X11 window handle. */
Xv_xrectlist *xrects ;               /* Damage rectlist. */
{
  short dmode ;
  Rect *r ;                          /* Canvas dimensions. */
  static int wwidth = 0 ;
  int x, y, width, height ;
  int w, wno ;

  if (Fm->dont_paint) return ;

  wno = (int) xv_get(canvas, XV_KEY_DATA, Fm->wno_key) ;
  set_status_info(wno) ;

  if (Fm->file[wno]->can_paint == FALSE) return ;
  X->fileX[wno]->canvas = canvas ;
  X->fileX[wno]->pw     = pw ;
  if (xid != 0)
    X->fileX[wno]->xid  = xid ;
  else
    X->fileX[wno]->xid  = (Drawable) xv_get(pw, XV_XID) ;

/* XView only: If window width changes; repaint entire window. */

  w = (int) xv_get(canvas, WIN_WIDTH) ;
  if (w != wwidth)
    {
      xrects = NULL ;
      wwidth = w ;
    }

  if (Fm->panning)    /* Force a clear screen and repaint if we're panning. */
    {
      xrects = NULL ;
      Fm->panning = FALSE ;
    }

  if (xrects)
    {
      XRectangle xr ;

      xr = compress_damage(display, xid, xrects) ;
      x      = xr.x ;
      y      = xr.y ;
      width  = xr.width ;
      height = xr.height ;
    }
  else
    {
      Drawable fxid ;

/* If null, the area is the entire window. */

      r = (Rect *) xv_get(canvas, CANVAS_VIEWABLE_RECT, pw) ;
      x      = r->r_left ;
      y      = r->r_top ;
      width  = r->r_width ;
      height = r->r_height ;
      fxid = (Drawable) xv_get(pw, XV_XID) ;
      XClearArea(display, fxid, x, y, width, height, FALSE) ;
    }

  dmode = Fm->file[wno]->display_mode ;

  check_viewable_rect(wno, x, y, width, height) ;

       if (dmode == VIEW_BY_LIST && Fm->file[wno]->listopts)
    do_display_list(wno, x, y, width, height) ;
  else
    do_display_icon(wno, x, y, width, height) ;

  if (!Fm->file[wno]->num_objects) set_pos_attr(wno, FM_POS_POS, FALSE) ;
}


void
resize_X_arrays(f, n)   /* Resize dynamic arrays, resize main frame. */
int f ;                 /* True if first time. Malloc, else realloc. */
int n ;                 /* Initial size or increment. */
{
  int old ;            /* Old size of the array. */
  int wno ;

  if (f)
    {
      old = 0 ;
      Fm->maxwin = n ;
    }
  else
    {
      old = Fm->maxwin ;
      Fm->maxwin += n ;
    }

  X->fileX = (File_PaneX **) LINT_CAST(make_mem((char *) X->fileX,
              sizeof(File_PaneX **) * old, sizeof(File_PaneX **) * n, f)) ;

  for (wno = old; wno < Fm->maxwin; wno++)
    X->fileX[wno] = (File_PaneX *)
                     LINT_CAST(fm_malloc((unsigned) sizeof(File_PaneX))) ;
}


/* Rename panel interpose procedure. */

static Notify_value
rename_panel_interpose_proc(panel, event, arg, type)
Panel panel ;
Event *event ;
Notify_arg arg ;
Notify_event_type type ;
{
  switch (event_action(event))
    {
      case WIN_REPAINT :

/* XXX: kludge to get around XView's inability to do input focus. */

        XV_SET(panel, WIN_SET_FOCUS, 0) ;  /* Set input focus on the panel. */
        break ;

      default : break ;
    }
  return(notify_next_event_func(panel, (Notify_event) event, arg, type)) ;
}


/*  Rename a file by creating a text panel item and placing it on the
 *  canvas for the user to edit.
 */

void
fm_rename(wno, i)
int wno ;           /* Window pane number where file is located. */
int i ;             /* File index number for file. */
{
  int x, y, len ;
  static WCHAR file[128] ;

  STRNCPY((char *) file, (char *) Fm->file[wno]->object[i]->name, 127) ;

  if (!Fm->writable)
    {

/* Unable to rename in current folder. */

      fm_msg(TRUE, Str[(int) M_RENAMEPERM], file) ;
      return ;
    }

  Fm->Renamed_file = i ;           /* Save to stop editing later. */
  x = text_x_coord(Fm->Renamed_file, wno) ;
  y = text_y_coord(Fm->Renamed_file, wno) ;
  len = strlen((char *) file) ;

/* Make sure a minimum length is used. */

  if (len < 7) len = 7 ;

  if (X->fileX[wno]->Rename_panel == 0)
    {
      X->fileX[wno]->Rename_panel = (Panel)
                                    xv_create(X->fileX[wno]->frame, PANEL,
                                       XV_SHOW,       FALSE,
                                       WIN_HEIGHT,    Fm->font_sizeH + 4,
                                       WIN_COLUMNS,   len+1,
                                       XV_HELP_DATA,  "filemgr:Rename",
                                       0) ;
      if (Fm->color) fm_set_colormap(X->fileX[wno]->Rename_panel) ;
      NOTIFY_INTERPOSE_EVENT_FUNC(X->fileX[wno]->Rename_panel,
                                  rename_panel_interpose_proc, NOTIFY_SAFE) ;

      X->fileX[wno]->Rename_item = (Panel_item)
                            xv_create(X->fileX[wno]->Rename_panel, PANEL_TEXT,
                                      PANEL_NOTIFY_PROC,          stop_rename,
                                      PANEL_VALUE_DISPLAY_LENGTH, len,
                                      PANEL_VALUE_STORED_LENGTH,  255,
                                      XV_X,         1,
                                      XV_Y,         1,
                                      XV_HELP_DATA, "filemgr:Rename",
                                      0) ;
    }

  XV_SET(X->fileX[wno]->Rename_panel, WIN_X, x, WIN_Y, y, 0) ;
  XV_SET(X->fileX[wno]->Rename_item,
         PANEL_VALUE_DISPLAY_LENGTH, len,
         PANEL_VALUE,                file,
         0) ;

  window_fit_width(X->fileX[wno]->Rename_panel) ;

  XV_SET(X->fileX[wno]->Rename_item,
         PANEL_TEXT_SELECT_LINE,            /* Highlight entire line (V3+) */
         0) ;

  XV_SET(X->fileX[wno]->Rename_panel, XV_SHOW, TRUE, 0) ;

/* Put up "Renaming %s ..." message. */

  fm_msg(FALSE, Str[(int) M_RENAME], file) ;
  set_timer(0) ;                           /* Turn timer repaint check off. */
  Fm->Rename_wno = wno ;
  Fm->Renaming = TRUE ;
}


/*  Get font used, and initialise the tree pane server images. */

void
fm_font_and_sv_init()
{
  char s[2] ;                         /* For font character. */
  int i ;
  XFontStruct *font_info ;
  Xv_Font font ;

  if (defaults_exists("font.name", "Font.Name"))
    font = (Xv_Font ) xv_get(X->base_frame, XV_FONT) ;
  else

/* Setup tree font. */

#ifdef OW_I18N
  font = (Xv_Font) xv_find(X->base_frame, FONT,
                           FONT_FAMILY, FONT_FAMILY_SANS_SERIF,
                           FONT_STYLE,  FONT_STYLE_NORMAL,
                           0) ;
#else
  font = (Xv_Font ) xv_find(X->base_frame, FONT,
                            FONT_FAMILY, FONT_FAMILY_LUCIDA,
                            FONT_STYLE,  FONT_STYLE_NORMAL,
                            0) ;
#endif /*OW_I18N*/

  if (!font)
    {
      fm_msg(TRUE, Str[(int) M_LUCIDA]) ;
      font = (Xv_Font ) xv_get(X->base_frame, XV_FONT) ;
    }

  X->tree_font = font ;
  Fm->defchar_height = (int) xv_get(X->tree_font, FONT_DEFAULT_CHAR_HEIGHT) ;
  Fm->tree.height = GLYPH_HEIGHT + (2 * MARGIN) + Fm->defchar_height ;

#ifdef OW_I18N
  X->font_set = (XFontSet) xv_get(X->tree_font, FONT_SET_ID) ;
#else
  X->font_set = 0 ;
#endif /*OW_I18N*/

  X->fontstruct = (XFontStruct *) xv_get(X->tree_font, FONT_INFO) ;
#ifdef OW_I18N
  X->font_extents = XExtentsOfFontSet(X->font_set) ;
  Fm->ascent  = -(X->font_extents->max_logical_extent.y) ;
  Fm->descent = X->font_extents->max_logical_extent.height - Fm->ascent ;
#else
  Fm->ascent  = X->fontstruct->max_bounds.ascent ;
  Fm->descent = X->fontstruct->max_bounds.descent ;
#endif /*OW_I18N*/

/* Remember character sizes for tree font. */

  s[1] = '\0' ;

#ifndef OW_I18N
  font_info = (XFontStruct *) xv_get(font, FONT_INFO) ;
  for (i = ' '; i <= 255; i++)
    {
      *s = (char ) i ;
      if (font_info->per_char)
        Fm->Fontwidth[i-' '] = (int)
          font_info->per_char[i - font_info->min_char_or_byte2].width ;
      else
        Fm->Fontwidth[i-' '] = (int) font_info->min_bounds.width ;
    }
#endif /*OW_I18N*/
}


void
make_busy_cursor()
{
  X->busy_cursor = (Xv_Cursor) xv_create(XV_NULL,          CURSOR,
                                         CURSOR_SRC_CHAR,  OLC_BUSY_PTR,
                                         CURSOR_MASK_CHAR, OLC_BUSY_MASK_PTR,
                                         0) ;
}


void
make_panning_cursor()
{
  Server_image panning_sv ;

  panning_sv = xv_create(XV_NULL,            SERVER_IMAGE,
                         XV_WIDTH,           16,
                         XV_HEIGHT,          16,
                         SERVER_IMAGE_BITS,  panning_image,
                         SERVER_IMAGE_DEPTH, 1,
                         0) ;
  X->pan_cursor = xv_create(XV_NULL,      CURSOR,
                            CURSOR_IMAGE, panning_sv,
                            CURSOR_XHOT,  0,
                            CURSOR_YHOT,  12,
                            CURSOR_OP,    PIX_SRC ^ PIX_DST,
                            0) ;
}


void
fm_set_folder_colors()
{

/* XXX kludge - set Folder_fg & Folder_bg to default folder
 *     colors.  Should be cleaned up and removed entirely sometime.
 */

  Fm->Folder_fg = get_default_fg_index(FT_DIR) ;
  Fm->Folder_bg = get_default_bg_index(FT_DIR) ;

  if (Fm->color)
    {
      XPixel cms_index ;

      Fm->Panel_fg = (unsigned long) xv_get(X->cms, CMS_FOREGROUND_PIXEL) ;
      Fm->Panel_bg = (unsigned long) xv_get(X->cms, CMS_BACKGROUND_PIXEL) ;

      cms_index = (XPixel) xv_get(X->base_frame, WIN_FOREGROUND_COLOR) ;
      Fm->Window_fg = (unsigned long) ds_x_index(cms_index) ;

      cms_index = (XPixel) xv_get(X->base_frame, WIN_BACKGROUND_COLOR) ;
      Fm->Window_bg = (unsigned long) ds_x_index(cms_index) ;

/* Can't do earlier; XV_SET on colormap causes a repaint. */

/* Set foreground & background color for color system. */

      XSetBackground(X->display, X->GCimage, Fm->Folder_bg) ;
      XSetForeground(X->display, X->GCimage, Fm->Folder_fg) ;

/* Set text color to default folder's colors. */

      XSetBackground(X->display, X->GCttext, Fm->Window_bg) ;
      XSetForeground(X->display, X->GCttext, Fm->Panel_fg) ;

      XSetBackground(X->display, X->GCftext, Fm->Panel_bg) ;
      XSetForeground(X->display, X->GCftext, Fm->Panel_fg) ;

/* Set mask gc to draw in window fg/bg colors. */

      XSetForeground(X->display, X->GCmargc, Fm->Window_bg ^ Fm->Window_fg) ;
    }
}


void
fm_tree_font_and_color()
{
  if (Fm->color)
    {
      fm_init_colormap() ;
      fm_set_colormap(X->base_frame) ;
    }

/* Set text context's font. */

  XSetFont(X->display, X->GCttext, xv_get(X->tree_font, XV_XID)) ;
}


void
fm_create_tree_canvas()
{
  enum menu_type m ;
  int minwidth, minheight ;
  Icon icon ;
  Menu_item mi ;
  Rect *r ;
  static Server_image Tree_icon_sv = 0 ;
  static Server_image Tree_mask_sv = 0 ;
  Xv_Window first_pw ;

  static unsigned short T_image[] = {
#include "images/tree.icon"
  } ;
  static unsigned short T_mask[] = {
#include "images/tree_mask.icon"
  } ;

  fm_window_create(FM_TREE_FRAME, WNO_TREE) ;

  XV_SET(X->treeX.file_button,
         PANEL_ITEM_MENU, X->menus[(int) FM_FILE_MENU],
         0) ;
  XV_SET(X->treeX.view_button,
         PANEL_ITEM_MENU, X->menus[(int) FM_VIEW_MENU],
         0) ;
  XV_SET(X->treeX.goto_button,
         PANEL_ITEM_MENU, X->menus[(int) FM_GOTO_MENU],
         0) ;

  minwidth  = (int) xv_get(X->treeX.goto_button, XV_X) +
              (int) xv_get(X->treeX.goto_button, XV_WIDTH) + 100 ;
  minheight = (int) xv_get(X->treeX.canvas, XV_Y) + 100 ;

  XV_SET(X->treeX.frame, FRAME_MIN_SIZE, minwidth, minheight, 0) ;

  fm_menu_create(FM_TFILE_MENU) ;
  m = FM_TFILE_MENU ;
  set_menu_window_no(m, WNO_TREE) ;
  XV_SET(X->treeX.file_button, PANEL_ITEM_MENU, X->menus[(int) m], 0) ;
  add_menu_help(m, TFILE_BUT_OPEN,   "File_Open") ;
  add_menu_help(m, TFILE_BUT_OPENF,  "File_Open_Folder") ;
  add_menu_help(m, TFILE_BUT_FIND,   "File_Find") ;
  add_menu_help(m, TFILE_BUT_FILEI,  "File_File_Info") ;
  add_menu_help(m, TFILE_BUT_RCOPY,  "File_Remote_Copy") ;
  add_menu_help(m, TFILE_BUT_CCMDS,  "File_Custom_Commands") ;

  add_menu_accel(m, TFILE_BUT_OPEN, "coreset Open") ;
  add_menu_accel(m, TFILE_BUT_FIND, "coreset Find") ;
  add_frame_menu_accel(WNO_TREE, FM_TFILE_MENU) ;

  mi = (Menu_item) xv_get(X->menus[(int) m], MENU_NTH_ITEM, TFILE_BUT_CCMDS) ;
  XV_SET(mi, MENU_PULLRIGHT, X->menus[(int) FM_CC_MENU], 0) ;

  fm_menu_create(FM_TVIEW_MENU) ;
  m = FM_TVIEW_MENU ;
  XV_SET(X->treeX.view_button, PANEL_ITEM_MENU, X->menus[(int) m], 0) ;
  add_menu_help(m, TVIEW_DIR,   "Tree_Pane_Direction") ;
  add_menu_help(m, TVIEW_SHOW,  "Tree_Pane_Show_All_Subfolders") ;
  add_menu_help(m, TVIEW_HIDE,  "Tree_Pane_Hide_Subfolders") ;
  add_menu_help(m, TVIEW_START, "Tree_Pane_Start_Tree_Here") ;
  add_menu_help(m, TVIEW_ADD,   "Tree_Pane_Add_Trees_Parent") ;

  XV_SET(X->treeX.frame,
         FRAME_WM_COMMAND_ARGC_ARGV, 0, -1,
         XV_KEY_DATA,                Fm->wno_key, WNO_TREE,
         0) ;

  Fm->Hscroll_unit = GLYPH_WIDTH * 3 ;
  Fm->Vscroll_unit = GLYPH_HEIGHT + MARGIN +  Fm->defchar_height ;
  set_scrollbar_attr(WNO_TREE, FM_V_SBAR, FM_SB_LINE_HT, Fm->Vscroll_unit) ;
  set_scrollbar_attr(WNO_TREE, FM_V_SBAR, FM_SB_SPLIT, TRUE) ;
  set_scrollbar_attr(WNO_TREE, FM_H_SBAR, FM_SB_LINE_HT, Fm->Hscroll_unit) ;
  set_scrollbar_attr(WNO_TREE, FM_H_SBAR, FM_SB_SPLIT, TRUE) ;

  if (!Tree_icon_sv)
    Tree_icon_sv = (Server_image) xv_create(XV_NULL,            SERVER_IMAGE,
                                            SERVER_IMAGE_BITS,  T_image,
                                            SERVER_IMAGE_DEPTH, 1,
                                            XV_WIDTH,           64,
                                            XV_HEIGHT,          64,
                                            0) ;

  if (!Tree_mask_sv)
    Tree_mask_sv = (Server_image) xv_create(XV_NULL,            SERVER_IMAGE,
                                            SERVER_IMAGE_BITS,  T_mask,
                                            SERVER_IMAGE_DEPTH, 1,
                                            XV_WIDTH,           64,
                                            XV_HEIGHT,          64,
                                            0) ;

  icon = (Icon) xv_create(XV_NULL,                ICON,
                          ICON_IMAGE,             Tree_icon_sv,
                          ICON_MASK_IMAGE,        Tree_mask_sv,
                          ICON_TRANSPARENT_LABEL, Str[(int) M_VIEW],
                          XV_WIDTH,               64,
                          XV_HEIGHT,              64,
                          WIN_RETAINED,           TRUE,
                          ICON_TRANSPARENT,       TRUE,
                          0) ;
  XV_SET(X->treeX.frame, FRAME_ICON, icon, 0) ;

  XV_SET(X->treeX.canvas,
         CANVAS_RETAINED,              FALSE,
         CANVAS_FIXED_IMAGE,           FALSE,
         CANVAS_X_PAINT_WINDOW,        TRUE,
         CANVAS_AUTO_SHRINK,           FALSE,
         CANVAS_AUTO_CLEAR,            FALSE,
         OPENWIN_SPLIT,
           OPENWIN_SPLIT_INIT_PROC,    fm_tree_split,
           OPENWIN_SPLIT_DESTROY_PROC, fm_tree_join,
           0,
         XV_HELP_DATA, "filemgr:Tree_Pane",
         XV_KEY_DATA,  Fm->wno_key, WNO_TREE,
         WIN_BIT_GRAVITY, ForgetGravity,
         0) ;

  Fm->tree.r_width       = get_canvas_attr(WNO_TREE, FM_WIN_WIDTH) ;
  Fm->tree.r_height      = get_canvas_attr(WNO_TREE, FM_WIN_HEIGHT) ;
  X->treeX.pw = first_pw = (Xv_Window) canvas_paint_window(X->treeX.canvas) ;
  X->treeX.xid           = (Drawable) xv_get(X->treeX.pw, XV_XID) ;
  Fm->tree.pw_width      = get_pw_attr(WNO_TREE, FM_WIN_WIDTH) ;
  Fm->tree.pw_height     = get_pw_attr(WNO_TREE, FM_WIN_HEIGHT) ;

  r = (Rect *) xv_get(X->treeX.goto_button, PANEL_ITEM_RECT) ;
  scrunch_window(WNO_TREE, r->r_height) ;

  XV_SET(X->treeX.file_button, XV_HELP_DATA, "filemgr:File", 0) ;
  XV_SET(X->treeX.view_button, XV_HELP_DATA, "filemgr:View", 0) ;
  XV_SET(X->treeX.goto_button,
         XV_HELP_DATA, "filemgr:Goto",
         XV_KEY_DATA,  Fm->wno_key, WNO_TREE,
         0) ;

  set_pw_events(WNO_TREE, first_pw) ;
  if (Fm->iconfont && Fm->iconfont[0]) set_icon_font(X->treeX.frame) ;
  create_drop_site(WNO_TREE, first_pw, FALSE) ;
  set_window_dims(WNO_TREE, Fm->tree_info) ;
  NOTIFY_INTERPOSE_EVENT_FUNC(first_pw, pw_tree_event, NOTIFY_IMMEDIATE) ;

  NOTIFY_INTERPOSE_EVENT_FUNC(X->treeX.canvas, tree_event, NOTIFY_SAFE) ;
  NOTIFY_INTERPOSE_EVENT_FUNC(X->treeX.canvas, tree_event, NOTIFY_IMMEDIATE) ;

  XV_SET(X->treeX.frame,
         WIN_CONSUME_X_EVENT_MASK, VisibilityChangeMask, 0) ;
  NOTIFY_INTERPOSE_EVENT_FUNC(X->treeX.frame, Fm_tree_event, NOTIFY_SAFE) ;
  NOTIFY_INTERPOSE_EVENT_FUNC(X->treeX.frame, Fm_tree_event, NOTIFY_IMMEDIATE) ;

  notify_interpose_destroy_func(X->treeX.frame, destroy_tree) ;
  sync_server() ;
}


/*ARGSUSED*/
static Notify_value
destroy_tree(frame, status)
Frame frame ;
Destroy_status status ;
{
  int i;

  if (status == DESTROY_CLEANUP)
    {
      if (X->treeX.frame)
        {

/*  Remember the current state of the tree pane, so that if the user wants
 *  to show it again, it'll look the same.
 */
	  if (!is_frame_closed(WNO_TREE)) {  /* decrease the openwins count */
	     Fm->openwins-- ;
             if (!Fm->openwins) set_timer(0) ;
          }
	  for (i = 0; i < MAXPOPUPS; i++)
	    if (Fm->popup_wno[i] == WNO_TREE)
	      {
		Fm->popup_wno[i] = -9 ;
		if (X->items[(int) FM_FIO_FIO_FRAME] ==
		    X->items[Fm->popup_item[i]])
                  Fm->fprops_showing = FALSE ;
		XV_SET(X->items[Fm->popup_item[i]],
		       FRAME_CMD_PUSHPIN_IN, FALSE,
		       0);
		XV_SET(X->items[Fm->popup_item[i]], XV_SHOW, FALSE, 0) ;
		XV_SET(X->items[Fm->popup_item[i]],
		       FRAME_CMD_PUSHPIN_IN, TRUE,
		       0);
	      }
          get_win_X_info(WNO_TREE, Fm->tree_info) ;
          XV_DESTROY_SAFE(X->treeX.frame) ;
          Fm->treeview = FALSE ;
          Fm->tree.can_paint = FALSE ;
          X->treeX.frame = 0 ;
        }
      return(notify_next_destroy_func(frame, status)) ;
    }
  return(NOTIFY_DONE) ;
}


/*ARGSUSED*/
static void
fm_tree_join(win)
Xv_Window win ;
{
  resize_window_drop_sites(WNO_TREE, FALSE) ;
}


/*ARGSUSED*/
static void
fm_tree_split(orig, new, pos)
Xv_Window orig, new ;
int pos ;
{
  int wno ;
  Xv_Window npw, opw ;

  opw = (Xv_Window) xv_get(orig, CANVAS_VIEW_PAINT_WINDOW) ;
  wno = (int)       xv_get(opw,  XV_KEY_DATA, Fm->wno_key) ;
  npw = (Xv_Window) xv_get(new,  CANVAS_VIEW_PAINT_WINDOW) ;

  XV_SET(npw,
         WIN_CONSUME_PICK_EVENTS,
           WIN_META_EVENTS,
           LOC_WINEXIT,
           LOC_DRAG,
           ACTION_DRAG_MOVE,
           ACTION_DRAG_COPY,
           ACTION_DRAG_PREVIEW,
           SHIFT_CTRL,
           ACTION_PROPS,
           ACTION_COPY,
           ACTION_PASTE,
           ACTION_FIND_FORWARD,
           ACTION_FIND_BACKWARD,
           ACTION_CUT,
           0,
         XV_KEY_DATA,     Fm->wno_key, wno,
         WIN_BIT_GRAVITY, ForgetGravity,
         0) ;

  create_drop_site(wno, npw, FALSE) ;   /* Create dnd drop site on pw. */

  NOTIFY_INTERPOSE_EVENT_FUNC(npw, pw_tree_event, NOTIFY_SAFE) ;
  NOTIFY_INTERPOSE_EVENT_FUNC(npw, pw_tree_event, NOTIFY_IMMEDIATE) ;
}


/*ARGSUSED*/
void
do_tree_repaint(canvas, pw, display, xid, xrects)
Canvas canvas ;
Xv_Window pw ;
Display *display ;
Window xid ;
Xv_xrectlist *xrects ;
{
  if (Fm->dont_paint || Fm->tree.can_paint == FALSE) return ;

/*  Keep the visible canvas window dimensions to avoid repainting
 *  unnecessary bits.
 */

  Fm->tree.r_height = get_canvas_attr(WNO_TREE, FM_WIN_HEIGHT) ;
  Fm->tree.r_width  = get_canvas_attr(WNO_TREE, FM_WIN_WIDTH) ;

/* Convert scrolling units to pixels. */

  Fm->tree.r_top  = get_scrollbar_attr(WNO_TREE, FM_V_SBAR, FM_SB_VIEW_ST) *
                    Fm->Vscroll_unit ;
  Fm->tree.r_left = get_scrollbar_attr(WNO_TREE, FM_H_SBAR, FM_SB_VIEW_ST) *
                    Fm->Hscroll_unit ;

  Fm->tree.dx1 = Fm->tree.r_left ;
  Fm->tree.dy1 = Fm->tree.r_top ;
  Fm->tree.dx2 = Fm->tree.r_left + Fm->tree.r_width ;
  Fm->tree.dy2 = Fm->tree.r_top  + Fm->tree.r_height ;

/*  Recalculate tree positions when the tree view changes.  Always
 *  keep open folder visible when state changes from path to tree...
 */

  fm_drawtree(Fm->StateChanged) ;
  if (Fm->StateChanged)
    {
      Fm->StateChanged = FALSE ;
      if (Fm->panning) Fm->panning = FALSE ;
      else if (Fm->treeview && Fm->tree.current)
        fm_visiblefolder(Fm->tree.current);
    }
}


void
fm_drawtree(calc)      /* Draw path pane. */
Boolean calc ;         /* Recalculate tree positions. */
{
  Drawable xid ;
  Tree_Pane_Object *saved_sibling ;
  int height, width ;
  int toobig = FALSE ;
  int width_ok, height_ok;
  static int prev_width = 0;
  static int prev_height = 0;

  if (!Fm->treeview) return ;

  xid = X->treeX.xid ;

/* Lock and clear canvas... */

  if (calc)
    XClearArea(X->display, xid, Fm->tree.dx1, Fm->tree.dy1,
      Fm->tree.dx2, Fm->tree.dy2, FALSE) ;

  if (Fm->tree.head == NULL) return ;    /* Tree not setup yet. */
  if (calc)
    {
      calc_tree(Fm->tree.head) ;
      Fm->dont_paint = TRUE ;

      width  = Fm->tree_width  + (2 * MARGIN) ;
      if (width % Fm->Hscroll_unit)
        width = Fm->Hscroll_unit * ((width / Fm->Hscroll_unit) + 1) ;
      if (width > MAX_CANVAS_SIZE)
        {
          width = MAX_CANVAS_SIZE - 50 ;
          toobig = TRUE ;
        }

      height = Fm->tree_height + (2 * MARGIN) ;
      if (height % Fm->Vscroll_unit)
        height = Fm->Vscroll_unit * ((height / Fm->Vscroll_unit) + 1) ;
      if (height > MAX_CANVAS_SIZE)
        {
          height = MAX_CANVAS_SIZE - 50 ;
          toobig = TRUE ;
        }

      width_ok = ((int)xv_get(X->treeX.canvas, XV_WIDTH) == width);
      height_ok = ((int)xv_get(X->treeX.canvas, XV_HEIGHT) == height);
      if (prev_width != width)
        prev_width = width;
      else
        width_ok = 1;

      if (prev_height != height)
        prev_height = height;
      else
        height_ok = 1;

      if (!width_ok && !height_ok)
        XV_SET(X->treeX.canvas,                  /* This clears the canvas. */
             CANVAS_WIDTH,  width,
             CANVAS_HEIGHT, height,
             0) ;
      else if (width_ok && !height_ok)
        XV_SET(X->treeX.canvas,                  /* This clears the canvas. */
             CANVAS_HEIGHT, height,
             0) ;
      if (!width_ok && height_ok)
        XV_SET(X->treeX.canvas,                  /* This clears the canvas. */
             CANVAS_WIDTH,  width,
             0) ;
      Fm->dont_paint = FALSE ;
    }

/*  If we've changed the tree's root, we don't want to display any of it's
 *  possible sibling...
 */

  saved_sibling = Fm->tree.head->sibling ;
  Fm->tree.head->sibling = NULL ;
  begin_buffer_tree_segments() ;
  display_tree(Fm->tree.head) ;
  flush_buffer_tree_segments() ;
  Fm->tree.head->sibling = saved_sibling ;

  draw_folder(WNO_TREE, Fm->tree.selected, FT_DIR_OPEN, TRUE) ;
  set_tree_title(Fm->tree.head) ;
  if (toobig) write_footer(WNO_TREE, Str[(int) M_TRUNC_VIEW], TRUE) ;
}


void
draw_path(wno, do_scroll)            /* Draw folder path. */
int wno, do_scroll ;
{
  Boolean drawn_first ;              /* Set after drawing first icon. */
  char *name ;                       /* Folder name. */
  int levelno ;                      /* Level array index. */
  int width ;                        /* Width of canvas to display path. */
  int x ;                            /* X coordinate. */
  int y ;                            /* Y coordinate. */
  int gap, itype, len, off, slen, top ;
  Canvas c ;
  Drawable xid ;
  Rect *r ;
  Scrollbar hsb ;
  File_Pane *f = Fm->file[wno] ;
  Tree_Pane *t_p ;
  Tree_Pane_Object *f_p ;            /* Node pointers. */
  Tree_Pane_Object *level[MAXLEVELS] ;      /* Nodes in path */

  width = 0 ;
  c   = X->fileX[wno]->pathX.canvas ;
  xid = X->fileX[wno]->pathX.xid ;
  r   = (Rect *) xv_get(c, CANVAS_VIEWABLE_RECT, X->fileX[wno]->pathX.pw) ;
  XClearArea(X->display, xid, r->r_left,  r->r_top,
                              r->r_width, r->r_height, FALSE) ;
  top = r->r_top ;

/* Get full path name; back up tree */

  t_p = &f->path_pane ;
  f_p = t_p->current ;
  for (levelno = 0; f_p != t_p->head; f_p = f_p->parent, levelno++)
    {
      if (levelno > MAXLEVELS)
        {
          fm_msg(TRUE, Str[(int) M_LEVELS]) ;
          return ;
        }
      level[levelno] = f_p ;
    }
  level[levelno] = f_p ;

  Fm->Npath = levelno+1 ;

/* Determine the length of the longest part of the full pathname. */

  while (levelno > -1)
    {
      f_p = level[levelno] ;
      len = strwidth(f_p->name) ;
      if (len < GLYPH_WIDTH) len = GLYPH_WIDTH ;
      width += len ;
      levelno-- ;
    }

/* Adjust the width of the path pane to fully handle the pathname. */

  gap = GLYPH_WIDTH >> 2 ;
  XV_SET(c, CANVAS_WIDTH, width + (gap * (Fm->Npath+1)), 0) ;

/* Display path. */

  drawn_first = FALSE ;
  levelno = Fm->Npath - 1 ;
  y = MARGIN + top ;
  if (!Fm->color) XSetForeground(X->display, X->GCttext, Fm->Folder_fg) ;
  x   = gap ;
  while (levelno > -1)
    {
      f_p = level[levelno] ;
      if (drawn_first == FALSE && wno == WASTE) name = Str[(int) M_WASTE] ;
      else                                      name = (char *) f_p->name ;
      slen = len = strwidth((WCHAR *) name) ;
      if (len < GLYPH_WIDTH) len = GLYPH_WIDTH ;
      itype = (levelno) ? FT_DIR : FT_DIR_OPEN ;
      if (drawn_first == FALSE)
        {
               if (wno == WASTE)          itype = FT_WASTE ;
          else if (f->mtype == FM_CD)     itype = FT_CD ;
          else if (f->mtype == FM_FLOPPY) itype = FT_FLOPPY ;
          else if (f->mtype == FM_DOS)    itype = FT_DOS ;
        }
      if (len > GLYPH_WIDTH) off = (len - GLYPH_WIDTH) / 2 ;
      else                   off = (GLYPH_WIDTH - len) / 2 ;
      f_p->Xpos = x + off ;
      f_p->Ypos = y ;
      draw_folder(wno, f_p, itype, FALSE) ;
      drawn_first = TRUE ;

      XDRAWIMAGESTRING(X->display, xid, X->font_set, X->GCttext,
                       x + ((len - slen) / 2),
                       y + GLYPH_HEIGHT + Fm->ascent, name, strlen(name)) ;
      x += len + gap ;
      levelno-- ;
    }

/* Draw connecting lines between folders. */

  for (f_p = t_p->current; f_p != t_p->head; f_p = f_p->parent)
    XDrawLine(X->display, xid, X->GCline,
              f_p->Xpos,                       y + (GLYPH_HEIGHT >> 1),
              f_p->parent->Xpos + GLYPH_WIDTH, y + (GLYPH_HEIGHT >> 1)) ;

  t_p->lastcur = t_p->current ;

       if (wno == WASTE                      && t_p->head == t_p->selected)
    itype = FT_WASTE ;
  else if (f->mtype == FM_CD     && t_p->head == t_p->selected)
    itype = FT_CD ;
  else if (f->mtype == FM_FLOPPY && t_p->head == t_p->selected)
    itype = FT_FLOPPY ;
  else if (f->mtype == FM_DOS    && t_p->head == t_p->selected)
    itype = FT_DOS ;
  else
    itype = (t_p->current == t_p->selected) ? FT_DIR_OPEN : FT_DIR ;
  draw_folder(wno, t_p->selected, itype, TRUE) ;

  if (do_scroll == TRUE)
    {
      hsb = (Scrollbar) xv_get(c, OPENWIN_HORIZONTAL_SCROLLBAR,
                               xv_get(c, OPENWIN_NTH_VIEW, 0)) ;
      scrollbar_scroll_to(hsb, (int) xv_get(hsb, SCROLLBAR_OBJECT_LENGTH)) ;
    }
}


static int
strwidth(str)           /* Return pixel width of string. */
register WCHAR *str ;
{
  return(ds_get_strwidth(X->fontstruct, (char *) str)) ;
}


void
apply_custom_panel()
{
  Custom_Command *cc ;
  Menu m = X->menus[(int) FM_CC_MENU] ;
  Menu_item mi ;
  char *ptr ;
  int i, insert_pt, selected, total ;
  int nitems = get_item_int_attr(FM_PO_CMD_LIST, FM_ITEM_LROWS) ;

  if ((selected = get_list_first_selected(FM_PO_CMD_LIST)) != -1)
    {
      do_custom_list_deselect(selected) ;
      select_list_item(FM_PO_CMD_LIST, selected, FALSE) ;
      Fm->ignore_custom_desel = TRUE ;
    }

  for (i = (int) xv_get(m, MENU_NITEMS) - 2; i > 0; i--)
    {
      mi = xv_get(m, MENU_NTH_ITEM, i) ;
      XV_SET(m, MENU_REMOVE, i, 0) ;
      XV_DESTROY_SAFE(mi) ;
    }

  for (i = 0; i < nitems; i++)
    {
      cc = (Custom_Command *) xv_get(X->items[(int) FM_PO_CMD_LIST],
                                     PANEL_LIST_CLIENT_DATA, i) ;
      mi = xv_create(XV_NULL,                  MENUITEM,
                     MENU_NOTIFY_PROC,         fm_exec_your_cmd,
                     MENU_STRING,              strdup(cc->alias),
                     XV_KEY_DATA, ALIAS_KEY,   strdup(cc->alias),
                     XV_KEY_DATA, CMDLINE_KEY, strdup(cc->command),
                     XV_KEY_DATA, PTEXT1_KEY,  strdup(cc->prompt),
                     XV_KEY_DATA, PROMPT_KEY,  strdup(cc->is_prompt),
                     XV_KEY_DATA, OUTPUT_KEY,  strdup(cc->is_output),
                     0) ;
      insert_pt = (int) xv_get(m, MENU_NITEMS) - 2 ;
      XV_SET(m, MENU_INSERT, insert_pt, mi, 0) ;
    }
  total = (int) xv_get(m, MENU_NITEMS) ;
  mi    = (Menu_item) xv_get(m, MENU_NTH_ITEM, total) ;
  XV_SET(m, MENU_DEFAULT_ITEM, mi, 0) ;
  fm_msg(FALSE, Str[(int) M_MENU_UPDATE]) ;
  Fm->custom_changed = FALSE ;
}


Menu_item
create_display_popup(item, op, frame, make)
Menu_item item ;
Menu_generate op ;
Xv_Window frame ;
void (*make)() ;
{

  if (op != MENU_NOTIFY) return(item) ;

/*  If the popup already exists, just show it on the screen, otherwise
 *  create, position and show the popup.
 */

  if (frame == 0) make() ;
  else
    {
      Menu m;
      int wno;

      m = (Menu) xv_get(item, MENU_PARENT) ;
      wno = (int) xv_get(m, MENU_CLIENT_DATA);
      XV_SET(frame, XV_SHOW, TRUE, 0) ;

      if (X->items[(int) FM_PO_PO_FRAME] == frame)
         Fm->popup_wno[(int) FM_PO_POPUP] = Fm->curr_wno ;
      else if (X->items[(int) FM_CP_CPO_FRAME] == frame)
         Fm->popup_wno[(int) FM_CP_POPUP] = Fm->curr_wno ;
      else if (X->items[(int) FM_RCP_RCP_FRAME] == frame)
         {
           if (wno == WNO_TREE)
	      Fm->popup_wno[(int) FM_RCP_POPUP] = WNO_TREE ;
           else
	      Fm->popup_wno[(int) FM_RCP_POPUP] = Fm->curr_wno ;
         }
    }
  return(item) ;
}


Menu_item
do_dnd(item, op, dndop)
Menu_item item ;
Menu_generate op ;
void (*dndop)() ;
{
  Menu menu ;
  Event_Context ec ;
  Event *event ;
  int wno ;

  if (op != MENU_NOTIFY) return(item) ;
  menu  = (Menu)    xv_get(item, MENU_PARENT) ;
  event = (Event *) xv_get(menu, MENU_FIRST_EVENT) ;
  if (event && event_action(event) == ACTION_ACCELERATOR) wno = Fm->curr_wno ;
  else wno = (int) xv_get(menu, MENU_CLIENT_DATA) ;
  ec    = setup_event_context(X->fileX[wno]->frame, event, FALSE) ;
  ec->x = ec->y = -1 ;

  dndop(ec) ;
  return(item) ;
}


void
do_fi_basic()
{
  int h, y ;
  Rect *r ;

  r = (Rect *) xv_get(X->items[(int) FM_FIO_OPEN_METHOD], PANEL_ITEM_RECT) ;
  y = r->r_top ;
  set_item_int_attr(FM_FIO_APPLY_BUTTON, FM_ITEM_Y, y) ;
  set_item_int_attr(FM_FIO_RESET_BUTTON, FM_ITEM_Y, y) ;
  set_item_int_attr(FM_FIO_MORE_BUTTON,  FM_ITEM_Y, y) ;
  set_item_int_attr(FM_FIO_OPEN_METHOD,  FM_ITEM_SHOW, FALSE) ;
  set_item_int_attr(FM_FIO_PRINT_METHOD, FM_ITEM_SHOW, FALSE) ;
  set_item_int_attr(FM_FIO_MOUNT_POINT,  FM_ITEM_SHOW, FALSE) ;
  set_item_int_attr(FM_FIO_MOUNT_FROM,   FM_ITEM_SHOW, FALSE) ;
  set_item_int_attr(FM_FIO_FREE_SPACE,   FM_ITEM_SHOW, FALSE) ;
  set_item_int_attr(FM_FIO_APPLY_BUTTON, FM_ITEM_SHOW, TRUE) ;
  set_item_int_attr(FM_FIO_RESET_BUTTON, FM_ITEM_SHOW, TRUE) ;

  h = r->r_top + r->r_height + 15 ;
  set_item_int_attr(FM_FIO_FIO_FRAME, FM_ITEM_HEIGHT, h) ;

  XV_SET(X->items[(int) FM_FIO_MORE_BUTTON],
         PANEL_LABEL_STRING,  Str[(int) M_INFO_MORE],
         0) ;
  Fm->info_extended = FALSE ;
}


void
do_fi_extended()
{
  int h, y ;
  Rect *r ;

  r = (Rect *) xv_get(X->items[(int) FM_FIO_FREE_SPACE], PANEL_ITEM_RECT) ;
  y = r->r_top + r->r_height + 15 ;
  set_item_int_attr(FM_FIO_APPLY_BUTTON, FM_ITEM_Y, y) ;
  set_item_int_attr(FM_FIO_RESET_BUTTON, FM_ITEM_Y, y) ;
  set_item_int_attr(FM_FIO_MORE_BUTTON,  FM_ITEM_Y, y) ;
  set_item_int_attr(FM_FIO_OPEN_METHOD,  FM_ITEM_SHOW, TRUE) ;
  set_item_int_attr(FM_FIO_PRINT_METHOD, FM_ITEM_SHOW, TRUE) ;
  set_item_int_attr(FM_FIO_MOUNT_POINT,  FM_ITEM_SHOW, TRUE) ;
  set_item_int_attr(FM_FIO_MOUNT_FROM,   FM_ITEM_SHOW, TRUE) ;
  set_item_int_attr(FM_FIO_FREE_SPACE,   FM_ITEM_SHOW, TRUE) ;

  r = (Rect *) xv_get(X->items[(int) FM_FIO_APPLY_BUTTON], PANEL_ITEM_RECT) ;
  h = r->r_top + r->r_height + 15 ;
  set_item_int_attr(FM_FIO_FIO_FRAME, FM_ITEM_HEIGHT, h) ;

  XV_SET(X->items[(int) FM_FIO_MORE_BUTTON],
         PANEL_LABEL_STRING,  Str[(int) M_INFO_LESS],
         0) ;
  Fm->info_extended = TRUE ;
}


void
load_goto_list()  /* Load scrolling list with menu values in reverse order. */
{
  Panel_item pi = X->items[(int) FM_PO_GOTO_LIST] ;
  int i ;

  clear_list(FM_PO_GOTO_LIST) ;

/* Insert the "Home" entry in the first row. */

  XV_SET(pi,
         PANEL_LIST_INSERT,      0,
         PANEL_LIST_STRING,      0, Str[(int) M_HOME],
         PANEL_LIST_CLIENT_DATA, 0, Str[(int) M_HOME],
         0) ;

/* Insert entries from the users goto menu array. */

  for (i = 0; i < Fm->no_user_goto; i++)
    XV_SET(pi,
           PANEL_LIST_INSERT,      i+1,
           PANEL_LIST_STRING,      i+1, Fm->user_goto_label[i],
           PANEL_LIST_CLIENT_DATA, i+1, Fm->user_goto[i],
           0) ;
}


void
do_po_custom_add()
{
  Custom_Command *cc ;
  int nitems = get_item_int_attr(FM_PO_CMD_LIST, FM_ITEM_LROWS) ;
  char *ptr ;

  if (valid_new_cmd() == TRUE)
    {
      cc = (Custom_Command *)
           LINT_CAST(fm_malloc((unsigned) sizeof(Custom_Command))) ;

      if ((ptr = get_item_str_attr(FM_PO_CMD_CMDLINE, FM_ITEM_IVALUE)) != NULL)
        cc->command = strdup(ptr) ;

      if ((ptr = get_item_str_attr(FM_PO_CMD_MLABEL,  FM_ITEM_IVALUE)) != NULL)
        if (*ptr == '\0') cc->alias = strdup(cc->command) ;
	else              cc->alias = strdup(ptr) ;

      if ((ptr = get_item_str_attr(FM_PO_CMD_PTEXT1,  FM_ITEM_IVALUE)) != NULL)
        cc->prompt = strdup(ptr) ;

      cc->is_prompt =
        (get_item_int_attr(FM_PO_CMD_PROMPT, FM_ITEM_IVALUE) == TRUE)
        ? strdup("false") : strdup("true") ;
      cc->is_output =
        (get_item_int_attr(FM_PO_CMD_OUTPUT, FM_ITEM_IVALUE) == TRUE)
        ? strdup("false") : strdup("true") ;

      XV_SET(X->items[(int) FM_PO_CMD_LIST],
             PANEL_LIST_INSERT,      nitems,
             PANEL_LIST_STRING,      nitems, strdup(cc->alias),
             PANEL_LIST_CLIENT_DATA, nitems, cc,
             0) ;
      reset_custom_fields() ;
      Fm->custom_changed      = TRUE ;
      Fm->ignore_custom_desel = TRUE ;
    }
}


void
do_po_goto_add()     /* Handle user selecting "Add to menu" on goto props. */
{
  Panel_item pi ;
  int selected ;
  int nitems     = get_item_int_attr(FM_PO_GOTO_LIST,   FM_ITEM_LROWS) ;
  char *alias    = get_item_str_attr(FM_PO_GOTO_MLABEL, FM_ITEM_IVALUE) ;
  char *pathname = get_item_str_attr(FM_PO_PATHNAME,    FM_ITEM_IVALUE) ;

  if (valid_goto_name(pathname))
    {
      pi = X->items[(int) FM_PO_GOTO_LIST] ;

      if (alias == NULL || alias[0] == '\0') alias = pathname ;

      selected = get_list_first_selected(FM_PO_GOTO_LIST) ;
      if (selected != -1) select_list_item(FM_PO_GOTO_LIST, selected, FALSE) ;

      XV_SET(pi,
             PANEL_LIST_INSERT, nitems,
             PANEL_LIST_STRING, nitems, alias,
             PANEL_LIST_CLIENT_DATA, nitems, strdup(pathname),
             0) ;
      set_item_str_attr(FM_PO_PATHNAME,    FM_ITEM_IVALUE, "") ;
      set_item_str_attr(FM_PO_GOTO_MLABEL, FM_ITEM_IVALUE, "") ;
      Fm->goto_changed      = TRUE ;
      Fm->ignore_goto_desel = TRUE ;
    }
}


int
give_file_warning()
{
  int result ;

  result = notice_prompt(X->fileX[Fm->curr_wno]->frame, NULL,
                         NOTICE_MESSAGE_STRINGS,
                           Str[(int) M_FILE1],
                           0,
                         NOTICE_BUTTON, Str[(int) M_FILE_APPLY], FILE_CONTINUE,
                         NOTICE_BUTTON, Str[(int) M_FILE_ADD], FILE_ADDFILE,
                         NOTICE_BUTTON, Str[(int) M_CANCEL],   FILE_CANCEL,
                         0) ;
  return(result) ;
}


/*ARGSUSED*/
Notify_value
find_child(frame, pid, status, rusage)
Notify_client frame ;
int pid ;
#ifdef SVR4
int *status ;
#else
union wait *status ;
#endif /*SVR4*/
void *rusage ;
{
  if (WIFEXITED(*status))
    {
      Fm->Pid = 0 ;
      stop_button(0, NULL) ;
      return(NOTIFY_DONE) ;
    }
  else return(NOTIFY_IGNORED) ;
}


/*ARGSUSED*/
Notify_value
find_stdout(me, fd)      /* Stick find's output into scrolling list. */
Notify_client me ;
int fd ;
{
  do_find_stdout(fd) ;
  return(NOTIFY_DONE) ;
}


/*ARGSUSED*/
Notify_value
find_stderr(me, fd)      /* Stick find's output into footer. */
Notify_client me ;
int fd ;
{
  do_find_stderr(fd) ;
  return(NOTIFY_DONE) ;
}


void
do_stop_button()
{
  do_find_stderr(Fm->pstderr[0]) ;
  do_find_stdout(Fm->pstdout[0]) ;

  if (Fm->Pid > 0)
    KILL(Fm->Pid, SIGTERM) ;        /* Kill find in progress. */

  if (Fm->Nfound) {
    char message[128];
    fm_snprintf(message, sizeof(message), Str[(int) M_NFILESFOUND], Fm->Nfound);
    error(TRUE, message, NULL);
  }
  else
    error(TRUE, Str[(int) M_NOTHINGFOUND], (char *) 0) ;

  Fm->Pid = 0 ;
  CLOSE(Fm->pstdout[0]) ;
  CLOSE(Fm->pstderr[0]) ;
  CLOSE(Fm->pstdout[1]) ;
  CLOSE(Fm->pstderr[1]) ;
  notify_set_input_func((Notify_client) &Fm->Pid, NOTIFY_FUNC_NULL, Fm->pstdout[0]) ;
  notify_set_input_func((Notify_client) &Fm->Pid, NOTIFY_FUNC_NULL, Fm->pstderr[0]) ;

  set_item_int_attr(FM_FIND_FIND_BUTTON, FM_ITEM_INACTIVE, FALSE) ;
  set_item_int_attr(FM_FIND_STOP_BUTTON, FM_ITEM_INACTIVE, TRUE) ;
}


void
set_waste_icon()     /* Set the wastebasket icon to full or empty. */
{
  Icon icon ;
  Boolean full = (Boolean) (Fm->file[WASTE]->num_objects != 0) ;
  static Server_image full_image  = 0, full_mask  = 0 ;
  static Server_image empty_image = 0, empty_mask = 0 ;
  static unsigned short full_bits[] = {
#include "images/waste-full.icon"
  } ;
  static unsigned short full_mask_bits[] = {
#include "images/waste-full_mask.icon"
  } ;
  static unsigned short empty_bits[] = {
#include "images/waste-empty.icon"
  } ;
  static unsigned short empty_mask_bits[] = {
#include "images/waste-empty_mask.icon"
  } ;

  if (!X->Waste_icon)
    X->Waste_icon = (Icon) xv_create(X->fileX[WASTE]->frame, ICON,
                          ICON_TRANSPARENT_LABEL,  Str[(int) M_WASTE],
                          WIN_RETAINED,            TRUE,
                          0) ;

  if (!full_image)
    full_image = (Server_image) xv_create(0,          SERVER_IMAGE,
                                  SERVER_IMAGE_BITS,  full_bits,
                                  SERVER_IMAGE_DEPTH, 1,
                                  XV_WIDTH,           64,
                                  XV_HEIGHT,          64,
                                  0) ;

  if (!full_mask)
    full_mask = (Server_image) xv_create(0,          SERVER_IMAGE,
                                 SERVER_IMAGE_BITS,  full_mask_bits,
                                 SERVER_IMAGE_DEPTH, 1,
                                 XV_WIDTH,           64,
                                 XV_HEIGHT,          64,
                                 0) ;

  if (!empty_image)
    empty_image = (Server_image) xv_create(0,          SERVER_IMAGE,
                                   SERVER_IMAGE_BITS,  empty_bits,
                                   SERVER_IMAGE_DEPTH, 1,
                                   XV_WIDTH,           64,
                                   XV_HEIGHT,          64,
                                   0) ;

  if (!empty_mask)
    empty_mask = (Server_image) xv_create(0,          SERVER_IMAGE,
                                  SERVER_IMAGE_BITS,  empty_mask_bits,
                                  SERVER_IMAGE_DEPTH, 1,
                                  XV_WIDTH,           64,
                                  XV_HEIGHT,          64,
                                  0) ;

  XV_SET(X->Waste_icon,
         ICON_IMAGE,       full ? full_image : empty_image,
         ICON_MASK_IMAGE,  full ? full_mask  : empty_mask,
         XV_WIDTH,         64,
         XV_HEIGHT,        64,
         WIN_RETAINED,     TRUE,
         ICON_TRANSPARENT, TRUE,
         0) ;
  XV_SET(X->fileX[WASTE]->frame, FRAME_ICON, X->Waste_icon, 0) ;
  if (Fm->iconfont && Fm->iconfont[0]) set_icon_font(X->fileX[WASTE]->frame) ;
}


static Notify_value
wb_quit(frame, status)
Frame frame ;
Destroy_status status ;
{
  int reply;

/*  Since the WB is created with a NULL parent, it will not be destroyed
 *  automatically when the main frame is destroyed.  Do it explicitly.
 */

  if (status == DESTROY_CLEANUP)
      return(notify_next_destroy_func(frame, status)) ;

  if (status == DESTROY_CHECKING)
    {
      reply = quit_waste() ;
      if (reply == FM_CANCEL)
        notify_veto_destroy(frame);
      else
        return(notify_next_destroy_func(frame, status)) ;
    }

  return(NOTIFY_DONE) ;
}


int
quit_waste()
{
  int reply, i;

  if (is_frame(WASTE))
    {
      if (Fm->confirm_delete && Fm->file[WASTE]->num_objects)
        {
          reply = prompt_with_message(WASTE, Str[(int) M_DO_DELETE],
                                      Str[(int) M_YES], Str[(int) M_NO],
                                      Str[(int) M_CANCEL]) ;
               if (reply == FM_CANCEL) return reply;
          else if (reply == FM_YES) do_delete_wastebasket() ;
        }

      XV_DESTROY_SAFE(X->fileX[WASTE]->frame) ;
      if (Fm->props_created)
        set_item_int_attr(FM_PO_DELETE_I, FM_ITEM_IVALUE, !Fm->confirm_delete) ;
      set_tree_icon(path_to_node(WNO_TREE, Fm->file[WASTE]->path), FALSE) ;
      X->Waste_icon = 0 ;
      X->fileX[WASTE]->frame = 0 ;
      fm_drawtree(FALSE) ;               /* Redraw waste icon as closed. */

/* Turn off repaint timer if all windows closed. */

      if (!Fm->openwins) set_timer(0) ;
      for (i = 0; i < MAXPOPUPS; i++)
        if (Fm->popup_wno[i] == WASTE)
          {
	    Fm->popup_wno[i] = -9 ;
            XV_SET(X->items[Fm->popup_item[i]],
		   FRAME_CMD_PUSHPIN_IN, FALSE,
		   0) ;
            XV_SET(X->items[Fm->popup_item[i]], XV_SHOW, FALSE, 0) ;
            XV_SET(X->items[Fm->popup_item[i]],
		   FRAME_CMD_PUSHPIN_IN, TRUE,
		   0) ;
	  }
    }
    return reply;
}


void
make_new_frame(wno, mtype)
int wno ;
enum media_type mtype ;
{
  int minwidth, minheight ;
  Rect *r ;          /* Goto button and frame icon position/size. */

  fm_window_create(FM_MAIN_FRAME, wno) ;
  XV_SET(X->fileX[wno]->file_button,
         PANEL_ITEM_MENU, X->menus[(int) FM_FILE_MENU],
         XV_KEY_DATA,     Fm->wno_key, wno,
         0) ;
  XV_SET(X->fileX[wno]->view_button,
         PANEL_ITEM_MENU, X->menus[(int) FM_VIEW_MENU],
         XV_KEY_DATA,     Fm->wno_key, wno,
         0) ;
  XV_SET(X->fileX[wno]->edit_button,
         PANEL_ITEM_MENU, X->menus[(int) FM_EDIT_MENU],
         XV_KEY_DATA,     Fm->wno_key, wno,
         0) ;
  XV_SET(X->fileX[wno]->goto_button,
         PANEL_ITEM_MENU, X->menus[(int) FM_GOTO_MENU],
         XV_KEY_DATA,     Fm->wno_key, wno,
         0) ;
  XV_SET(X->fileX[wno]->eject_button,
         XV_SHOW,         (mtype != FM_DISK),
         0) ;

  minwidth  = (int) xv_get(X->fileX[wno]->Path_item, XV_X) + 100 ;
  minheight = (int) xv_get(X->fileX[wno]->canvas, XV_Y) + 100 ;
  XV_SET(X->fileX[wno]->frame, FRAME_MIN_SIZE, minwidth, minheight, 0) ;

  set_frame_icon(wno, mtype) ;

  XV_SET(X->fileX[wno]->frame,
         FRAME_NO_CONFIRM, TRUE,
         FRAME_INHERIT_COLORS, TRUE,
         WIN_CONSUME_PICK_EVENTS,
           ACTION_PROPS,
           ACTION_COPY,
           ACTION_PASTE,
           ACTION_FIND_FORWARD,
           ACTION_FIND_BACKWARD,
           ACTION_CUT,
           0,
	 FRAME_WM_COMMAND_ARGC_ARGV, 0, FM_DISK == mtype ?  0 : -1,
         XV_KEY_DATA, Fm->wno_key, wno,
         0) ;

  if (!Fm->running && Fm->no_start_win == TRUE)
    {
      XV_SET(X->fileX[wno]->frame, FRAME_CLOSED,
             (int) xv_get(X->base_frame, FRAME_CLOSED), 0) ;
      XV_SET(X->fileX[wno]->frame, XV_X,
             (int) xv_get(X->base_frame, XV_X), 0) ;
      XV_SET(X->fileX[wno]->frame, XV_Y,
             (int) xv_get(X->base_frame, XV_Y), 0) ;
      XV_SET(X->fileX[wno]->frame, XV_WIDTH,
             (int) xv_get(X->base_frame, XV_WIDTH), 0) ;
      XV_SET(X->fileX[wno]->frame, XV_HEIGHT,
             (int) xv_get(X->base_frame, XV_HEIGHT), 0) ;
    }

  XV_SET(X->fileX[wno]->panel,
         OPENWIN_SHOW_BORDERS,   FALSE,
         XV_HELP_DATA,           "filemgr:Control_Panel",
         WIN_CONSUME_PICK_EVENT,
           ACTION_PROPS,
           0,
         0) ;

  r = (Rect *) xv_get(X->fileX[wno]->goto_button, PANEL_ITEM_RECT) ;
  scrunch_window(wno, r->r_height) ;

  XV_SET(X->fileX[wno]->file_button, XV_HELP_DATA, "filemgr:File",         0) ;
  XV_SET(X->fileX[wno]->view_button, XV_HELP_DATA, "filemgr:View",         0) ;
  XV_SET(X->fileX[wno]->edit_button, XV_HELP_DATA, "filemgr:Edit",         0) ;
  XV_SET(X->fileX[wno]->goto_button, XV_HELP_DATA, "filemgr:Goto",         0) ;
  XV_SET(X->fileX[wno]->Path_item,   XV_HELP_DATA, "filemgr:Goto_Line",    0) ;
  XV_SET(X->fileX[wno]->leftmess,    XV_HELP_DATA, "filemgr:Left_Status",  0) ;
  XV_SET(X->fileX[wno]->rightmess,   XV_HELP_DATA, "filemgr:Right_Status", 0) ;
  XV_SET(X->fileX[wno]->eject_button, XV_HELP_DATA, "filemgr:Eject_Disk",  0) ;

  create_frame_drop_site(X->fileX[wno]->frame, wno) ;
  fm_init_path_tree(wno) ;
  fm_create_folder_canvas(X->fileX[wno]->frame, wno) ;
  check_args(wno) ;
  if (Fm->iconfont && Fm->iconfont[0]) set_icon_font(X->fileX[wno]->frame) ;

  XV_SET(X->fileX[wno]->frame,
         WIN_CONSUME_X_EVENT_MASK, VisibilityChangeMask, 0) ;
  NOTIFY_INTERPOSE_EVENT_FUNC(X->fileX[wno]->frame,
                              Fm_frame_event, NOTIFY_SAFE) ;
  NOTIFY_INTERPOSE_EVENT_FUNC(X->fileX[wno]->frame,
                              Fm_frame_event, NOTIFY_IMMEDIATE) ;

  notify_interpose_destroy_func(X->fileX[wno]->frame, destroy_window) ;
}


Notify_value
destroy_window(client, status)
Notify_client client ;
Destroy_status status ;
{
  int left ;                   /* Number of filemgr folder windows left. */
  int reply, wno ;
  Tree_Pane_Object *t_p ;

  if (status == DESTROY_CHECKING)
    {
      wno = (int) xv_get(client, XV_KEY_DATA, Fm->wno_key) ;
      left = Fm->num_win - 2 ;
      if (left <= 0)
        {
          reply = quit_prompt(wno, Str[(int) M_QUIT_MES],
                              Str[(int) M_QUIT_FM], Str[(int) M_CANCEL]) ;
          if (reply == FM_CANCEL) notify_veto_destroy(client) ;
          else
            {
              if (Fm->confirm_delete && Fm->file[WASTE]->num_objects)
                {
                  reply = prompt_with_message(wno, Str[(int) M_DO_DELETE],
                                          Str[(int) M_YES], Str[(int) M_NO],
                                          Str[(int) M_CANCEL]) ;
                       if (reply == FM_CANCEL) notify_veto_destroy(client) ;
                  else if (reply == FM_YES)    do_delete_wastebasket() ;
                }
            }
        }
    }
  else if (status == DESTROY_CLEANUP)  /* Terminate request; clean up self. */
    {
      int i;
      Fm->num_win-- ;
      left = Fm->num_win - 1 ;
      wno = (int) xv_get(client, XV_KEY_DATA, Fm->wno_key) ;
      if (!is_frame_closed(wno)) {  /* decrease the openwins count */
	 Fm->openwins-- ;
         if (!Fm->openwins) set_timer(0) ;
      }
      for (i = 0; i < MAXPOPUPS; i++)
        if (Fm->popup_wno[i] == wno)
          {
	    Fm->popup_wno[i] = -9 ;
	    if (X->items[(int) FM_FIO_FIO_FRAME] == X->items[Fm->popup_item[i]])
	      Fm->fprops_showing = FALSE ;
            XV_SET(X->items[Fm->popup_item[i]],
		   FRAME_CMD_PUSHPIN_IN, FALSE,
		   0) ;
            XV_SET(X->items[Fm->popup_item[i]], XV_SHOW, FALSE, 0) ;
            XV_SET(X->items[Fm->popup_item[i]],
		   FRAME_CMD_PUSHPIN_IN, TRUE,
		   0) ;
	  }
      fm_pathdeselect(wno) ;
      save_directory_info(wno) ;
      if (left <= 0)
        {
          UNLINK(Fm->err_file) ;     /* Erase the temporary error file. */
          yield_shelf() ;
          save_window_positions() ;
          UNLINK(Fm->pipe_name) ;
          exit(0) ;
        }
      else
        {
          XV_DESTROY_SAFE(X->fileX[wno]->frame) ;
          t_p = path_to_node(WNO_TREE, Fm->file[wno]->path) ;
          set_tree_icon(t_p, FALSE) ;
          if (Fm->treeview) draw_folder(WNO_TREE, t_p, FT_DIR, FALSE) ;
          reset_file_pane(wno) ;
          Fm->file[wno]->mtype = FM_DISK ;
          Fm->file[wno]->can_paint = FALSE ;
	  X->fileX[wno]->Rename_panel = 0;
          X->fileX[wno]->frame = 0 ;
          X->Shelf_owner = 0;
          X->Shelf_item = 0;
	  if (X->dirDB != NULL) {
		XrmDestroyDatabase(X->dirDB) ;
          	X->dirDB = NULL;
	  }
        }
      return(notify_next_destroy_func(client, status)) ;
    }
  else if (status == DESTROY_SAVE_YOURSELF)
    {

/*  If the user has done a "Save Workspace", then save the current state of
 *  the wastebasket and the tree pane, plus all the existing filemgr windows
 *  to the ~/.desksetdefaults file.
 */

      save_window_positions() ;
    }
  return(NOTIFY_DONE) ;
}


int
fm_create_wastebasket()
{
  enum menu_type m = FM_WEDIT_MENU ;
  int minwidth, minheight ;
  Rect *r ;

  if (is_frame(WASTE)) return(SUCCESS) ;   /* Assume already created. */

  if (Fm->file[WASTE]->path == NULL)
    Fm->file[WASTE]->path = (WCHAR *) fm_malloc((unsigned) (MAXPATHLEN+1)) ;

  STRCPY((char *) Fm->file[WASTE]->path, (char *) Fm->home) ;
  if (Fm->home[strlen((char *) Fm->home)-1] != '/')
    STRCAT((char *) Fm->file[WASTE]->path, "/") ;
  STRCAT((char *) Fm->file[WASTE]->path, ".wastebasket") ;
  if (access((char *) Fm->file[WASTE]->path, F_OK) == -1)
    if (mkdir((char *) Fm->file[WASTE]->path, 0777) == -1)
      {
        fm_msg(TRUE, Str[(int) M_NO_CREATE_WASTE],
               Fm->file[WASTE]->path, strerror(errno)) ;
        return(FAILURE) ;
      }

  fm_window_create(FM_WASTE_FRAME, WASTE) ;

  set_window_params(WASTE) ;
  Fm->curr_wno = WASTE ;

  fm_menu_create(FM_WEDIT_MENU) ;
  XV_SET(X->fileX[WASTE]->edit_button, PANEL_ITEM_MENU, X->menus[(int) m], 0) ;
  add_menu_help(m, WASTE_EDIT_SEL_ALL,  "Waste_Select_All") ;
  add_menu_help(m, WASTE_EDIT_CUT,      "Edit_Cut") ;
  add_menu_help(m, WASTE_EDIT_COPY,     "Edit_Copy") ;
  add_menu_help(m, WASTE_EDIT_PASTE,    "Edit_Paste") ;
  add_menu_help(m, WASTE_EDIT_UNDELETE, "Waste_Undelete") ;
  add_menu_help(m, WASTE_EDIT_DESTROY,  "Waste_Destroy") ;
  add_menu_help(m, WASTE_EDIT_PROPS,    "Edit_Properties") ;

  add_menu_accel(m, WASTE_EDIT_SEL_ALL, "coreset SelectAll") ;
  add_menu_accel(m, WASTE_EDIT_CUT,     "coreset Cut") ;
  add_menu_accel(m, WASTE_EDIT_COPY,    "coreset Copy") ;
  add_menu_accel(m, WASTE_EDIT_PASTE,   "coreset Paste") ;
  add_menu_accel(m, WASTE_EDIT_PROPS,   "coreset Props") ;
  add_frame_menu_accel(WASTE, FM_WEDIT_MENU) ;

  minwidth  = 2 * ((int) xv_get(X->fileX[WASTE]->edit_button, XV_X) +
                 (int) xv_get(X->fileX[WASTE]->edit_button, XV_WIDTH)) ;
  minheight = (int) xv_get(X->fileX[WASTE]->canvas, XV_Y) + 100 ;
  XV_SET(X->fileX[WASTE]->frame, FRAME_MIN_SIZE, minwidth, minheight, 0) ;

  XV_SET(X->fileX[WASTE]->view_button,
         PANEL_ITEM_MENU, X->menus[(int) FM_VIEW_MENU],
         0) ;

  fm_init_path_tree(WASTE) ;    /* Create tree for waste win. path pane. */
  XV_SET(X->fileX[WASTE]->frame,
         FRAME_CLOSED,               TRUE,
         FRAME_WM_COMMAND_ARGC_ARGV, 0, -1,
         XV_KEY_DATA,                Fm->wno_key, WASTE,
         0) ;

  r = (Rect *) xv_get(X->fileX[WASTE]->edit_button, PANEL_ITEM_RECT) ;
  scrunch_window(WASTE, r->r_height) ;

  XV_SET(X->fileX[WASTE]->edit_button, XV_HELP_DATA, "filemgr:Waste_Edit", 0) ;
  XV_SET(X->fileX[WASTE]->view_button, XV_HELP_DATA, "filemgr:View",       0) ;
  XV_SET(X->fileX[WASTE]->leftmess,   XV_HELP_DATA, "filemgr:Left_Status",  0) ;
  XV_SET(X->fileX[WASTE]->rightmess,  XV_HELP_DATA, "filemgr:Right_Status", 0) ;

  if (Fm->iconfont && Fm->iconfont[0]) set_icon_font(X->fileX[WASTE]->frame) ;
  create_frame_drop_site(X->fileX[WASTE]->frame, WASTE) ;
  set_window_dims(WASTE, Fm->waste_info) ;

  XV_SET(X->fileX[WASTE]->frame,
         WIN_CONSUME_X_EVENT_MASK, VisibilityChangeMask, 0) ;
  NOTIFY_INTERPOSE_EVENT_FUNC(X->fileX[WASTE]->frame, Fm_frame_event,
                              NOTIFY_SAFE) ;
  NOTIFY_INTERPOSE_EVENT_FUNC(X->fileX[WASTE]->frame, Fm_frame_event,
                              NOTIFY_IMMEDIATE) ;

  notify_interpose_destroy_func(X->fileX[WASTE]->frame, wb_quit) ;

  fm_create_folder_canvas(X->fileX[WASTE]->frame, WASTE) ;

/*  Build the cache with wastebasket contents.  Note this calls
 *  set_waste_icon(), which does an XV_SHOW = TRUE on the WB frame.
 *  Because of this, make sure it happens after the xv_set of X & Y
 *  icon coordinates above.  This fixes bug 1065270 where the WB
 *  would not paint correctly at startup.
 */

  fm_display_folder((BYTE) FM_BUILD_FOLDER, WASTE) ;
  adjust_tree(WASTE) ;

  sync_server() ;

/*  Note, no need to turn on the itimer with set_timer() since the
 *  event ACTION_OPEN will do so later on.
 */

  return(SUCCESS) ;
}

/* I18N related routine   (SHOULD BE IN LIBDESKSET)
 * Parameter:  A pointer to a multibyte string or a pointer to NULL.
 * Return Value:  Returns a pointer to the next multibyte character.
 * Usage:  If the actual argument is non NULL then a pointer to the first multi-
 *         byte character is returned.  If the actual argument is NULL then
 *         a pointer to the next multibyte character is returned.  The parameter
 *         scheme is very much like strtok();
 * CAUTION:  If the calling function uses the return value then it should
 *           make a copy.  The return value is a static buffer that may
 *           be overwritten or freed on subsequent calls to this routine.
 */
extern char *
fm_mbchar(str)
char *str;
{
     static char *string;
     static char *string_head;
     static char *buf;
     int num_byte = 0;

     if ( str != NULL ) {
          if ( string != NULL ) {
               free(string_head);
               string_head = NULL;
               string = NULL;
          }
          string = strdup(str);
          string_head = string;
     }
     if ( buf != NULL ) {
          free(buf);
          buf = NULL;
     }
     if ( string == NULL ) {
          free(string_head);
          string_head = NULL;
     } else {
          num_byte = mblen(string, MB_LEN_MAX);
          buf = (char *)malloc(num_byte+1);
          strncpy(buf, string, num_byte);
          buf[num_byte] = '\0';
          string += num_byte;
     }

     return buf;
}

/* I18N related routine
 * Parameter:  A pointer to a multibyte string.
 * Return Value:  The number of characters in the multibyte string.
 */
extern int
fm_mbstrlen(s)
char *s;
{
	int num_byte = 0, num_char = 0;

	while (*s) {
		if ( (num_byte = mblen(s, MB_LEN_MAX)) <= 0 )
			break;
		num_char++;
		s += num_byte;
	}
	return num_char;
}

/*  Code taken from Calendar Manager.  Should be in libdeskset. */
/*	given an area of a certain length (in pixels), compute
	how many bytes of the string in that variable length
	font may be displayed in that area			*/

extern int
fm_nchars(area, str)
	int area; char *str;
{
	Font_string_dims dims;
	char *buf;
	int i, l, w=0, n=0;
	int first = 1;

	xv_get(X->file_font, FONT_STRING_DIMS, str, &dims);
	if (dims.width <= area) {
		return(strlen(str));
	}
	l = fm_mbstrlen(str);
	for (i=0; i<l; i++) {
		if ( first ) {
			buf = fm_mbchar(str);
			first = 0;
		} else {
			buf = fm_mbchar((char *)NULL);
		}
		xv_get(X->file_font, FONT_STRING_DIMS, buf, &dims);
		w+=dims.width;
		if (w <= area)
			n += mblen(buf, MB_LEN_MAX);
		else break;
	}
	return(n);
}
