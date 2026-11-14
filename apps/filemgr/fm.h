
/*  @(#)fm.h	1.160 04/18/97
 *
 *  Copyright (c) 1987-1992 Sun Microsystems, Inc.
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

#include <sys/param.h>
#include <sys/stat.h>
#include <netdb.h>
#include "support/ce.h"

/* Data Structures. */

typedef struct CE_Struct {
  CE_NAMESPACE file_ns ;         /* Handle to file namespace. */
  CE_ATTRIBUTE file_type ;       /* Handle to index to tns id (TYPE_NAME). */

  CE_NAMESPACE type_ns ;         /* Handle to type namespace. */
  CE_ATTRIBUTE type_name ;       /* Handle to tns id (TYPE_NAME). */
  CE_ATTRIBUTE type_open ;       /* Handle to open method. */
  CE_ATTRIBUTE type_print ;      /* Handle to print method. */
  CE_ATTRIBUTE type_icon ;       /* Handle to icon full pathname. */
  CE_ATTRIBUTE type_icon_mask ;  /* Handle to icon' mask full pathname. */
  CE_ATTRIBUTE type_fgcolor ;    /* Handle to icon's foreground color. */
  CE_ATTRIBUTE type_bgcolor ;    /* Handle to icon's background color. */
  CE_ATTRIBUTE type_template ;   /* Handle to filename template. */
  CE_ATTRIBUTE type_drop ;       /* Handle to drop method. */

  CE_ENTRY generic_dir ;     /* Handle to generic folder tns entry. */
  CE_ENTRY generic_doc ;     /* Handle to generic document tns entry. */
  CE_ENTRY generic_app ;     /* Handle to generic application tns entry. */

  int running ;              /* Handle to classing engine running? */
} CE_Object ;

typedef struct CE_Struct *CEO ;

typedef struct {
  char  *data ;
  int alloc_size ;
  int bytes_used ;
} CHAR_BUF ;

struct env_list {            /* Used to generate custom command FILE env. */
  int size ;
  char *buffer ;
  struct env_list *next ;
} ;

typedef struct Tree_line
{
  int dx, dy ;
  struct Tree_line *link ;
} Tree_polyline ;

struct Tree_polygon
{
  struct {
    Tree_polyline *head ;
    Tree_polyline *tail ;
  } lower, upper ;
} ;

typedef struct Tree_node         /* Tree Object Structure. */
{
  unsigned char *name ;          /* Folder name. */
  struct Tree_node *parent ;     /* Who owns it. */
  struct Tree_node *child ;      /* Who it owns. */
  struct Tree_node *sibling ;    /* Peers. */
  time_t mtime ;                 /* Modified time (when explored). */
  int width ;
  int Xpos, Ypos ;
  int Xoff, Yoff ;
  struct Tree_polygon contour ;
  BYTE status ;                  /* Status; pruned, symlink, etc. */
} Tree_Pane_Object ;


typedef struct                   /* Tree Pane structure. */
{
  int border, height ;           /* Fixed tree icon dimensions. */
  int busy_count ;               /* An aid for setting the frame un/busy. */
  int can_paint ;                /* Set TRUE after Visibility event. */
  int painted ;                  /* Set TRUE after first repaint. */
  int r_left ;                   /* Tree pane rect (x). */
  int r_top ;                    /* Tree pane rect (y). */
  int r_width ;                  /* Tree pane rect (width). */
  int r_height ;                 /* Tree pane rect (height). */
  int dx1, dy1, dx2, dy2 ;       /* Current repaint damage area. */
  int pw_width ;                 /* Width of the tree pane paint window. */
  int pw_height ;                /* Height of the tree pane paint window. */
  Tree_Pane_Object *root ;       /* Real root of tree '/'. */
  Tree_Pane_Object *head ;       /* Current root of tree. */
  Tree_Pane_Object *sibling ;    /* Current root's sibling. */
  Tree_Pane_Object *current ;    /* Current tree node. */
  Tree_Pane_Object *lastcur ;    /* Last current tree node. */
  Tree_Pane_Object *selected ;   /* Selected tree node. */
  Tree_Pane_Object *lastsel ;    /* Last selected tree node. */
} Tree_Pane ;

typedef struct mmap_handle {
  caddr_t addr ;
  int len ;
} mmap_handle_t ;

typedef struct                   /* Cache object structure. */
{
  mode_t  st_mode ;              /* File mode. */
  nlink_t st_nlink ;             /* Number of links. */
  uid_t   st_uid ;               /* User ID of the file's owner. */
  gid_t   st_gid ;               /* Group ID of the file's group. */
  off_t   st_size ;              /* File size in bytes. */
  time_t  mtime ;                /* Time of last data modification */
  time_t  ctime ;                /* Time of last file status change */
  int     flen ;                 /* Length of the filename. */
  int     foff ;                 /* Offset to start of filename. */
  int     tlen ;                 /* Length of the CE TYPE_NAME. */
  int     toff ;                 /* Offset to start of CE TYPE_NAME. */
} Cache_Object ;

typedef struct Delete_Struct {
  WCHAR *path ;                      /* Old path. */
  WCHAR *file ;                      /* Filename. */
  struct Delete_Struct *next ;
} Delete_Object ;

typedef struct Delete_Struct *Delete_item ;


typedef struct                   /* Folder Object Structure. */
{
  unsigned char *name ;          /* File name. */
  int flen ;                     /* Length of filename (in characters). */
  char *type_name ;              /* CE TYPE_NAME (for caching). */
  int tlen ;                     /* Length of the CE TYPE_NAME. */
  BYTE image_depth ;             /* Depth of the contents image. */
  short cx ;                     /* X pos. of icon in content mode. */
  short cy ;                     /* Y pos. of icon in content mode. */
  short ix ;                     /* X pos. of icon in icon mode. */
  short iy ;                     /* Y pos. of icon in icon mode. */
  short lx ;                     /* X pos. of icon in list mode (-options). */
  short ly ;                     /* Y pos. of icon in list mode (-options). */
  short ox ;                     /* X pos. of icon in list mode (+options). */
  short oy ;                     /* Y pos. of icon in list mode (+options). */
  short width ;                  /* Width of string in pixels. */
  BYTE type ;                    /* Document, folder, etc. */
  BYTE selected ;                /* Selected? */
  Boolean redisplay ;            /* Used to work out what to redisplay. */
  SBYTE color ;                  /* Icon color. */
  u_short mode ;                 /* Permissions. */
  short nlink ;                  /* Number of links. */
  int dirpos ;                   /* Direct block position (for caching). */
  int uid ;                      /* Owner id. */
  int gid ;                      /* Group id. */
  long size ;                    /* File size. */
  time_t mtime ;                 /* Modify time. */
  time_t ctime ;                 /* Last changed time. */

/* Binding information. */

  CE_ENTRY tns_entry ;           /* Index to CE type namespace. */
  unsigned long icon_fg_index ;  /* Icon foreground color. */
  unsigned long icon_bg_index ;  /* Icon foreground color. */
  unsigned long image ;          /* Contents image. */
} File_Pane_Object ;


typedef struct                   /* File Pane Positional Object Structure. */
{
  int canvas_height ;            /* Canvas height in this mode. */
  int canvas_width ;             /* Canvas width in this mode. */
  int incx ;                     /* X increment (column width). */
  int incy ;                     /* Y increment (row height). */
  int max_filespercol ;          /* Maximum number of files per column. */
  int max_filesperline ;         /* Maximum number of files per line. */
  int startx ;                   /* X starting location. */
  int starty ;                   /* Y starting location. */
  int pos ;                      /* Positions set for this mode? */
  int moved ;                    /* Set if user has moved an icon. */
  int hsbar_pos ;                /* Horizontal scrollbar position. */
  int vsbar_pos ;                /* Vertical scrollbar position. */
  WCHAR *grid ;                  /* Icon grid (for checking for free points). */
} File_Pane_Pos ;


typedef struct                   /* File Pane Structure (for array). */
{
  File_Pane_Object **object ;    /* Array of folder objects. */
  Tree_Pane path_pane ;          /* Path pane for this folder. */
  int painted ;                  /* Set TRUE after first repaint. */
  int r_left ;                   /* Folder pane rect (x). */
  int r_top ;                    /* Folder pane rect (y). */
  int r_width ;                  /* Folder pane rect (width). */
  int r_height ;                 /* Folder pane rect (height). */
  int pw_width ;                 /* Width of folder pane paint window. */
  int pw_height ;                /* Height of folder pane paint window. */
  File_Pane_Pos c ;              /* Info. for content mode. */
  File_Pane_Pos i ;              /* Info. for icon mode. */
  File_Pane_Pos l ;              /* Info. for list mode (-options). */
  File_Pane_Pos o ;              /* Info. for list mode (+options). */
  int busy_count ;               /* An aid for setting the frame un/busy. */
  int can_paint ;                /* Set TRUE after Visibility event. */
  int devno ;                    /* Device number for CD's and floppies. */
  int max_objects ;              /* Number of allocated object pointers. */
  int num_objects ;              /* Number of objects in folder. */
  int prev_num_objects;		 /* Number of objects in folder before update */
  int widest_name ;              /* Widest file name. */
  enum media_type mtype ;        /* Folder media type. */
  WCHAR *path ;                  /* Current path. */
  Boolean have_deleted ;         /* Set for floppies with deleted files. */
  Boolean Message ;              /* Message in the left footer? */

/* Cache variables. */

  mmap_handle_t *mmap_ptr ;      /* Pointer to possible mmap'ed cache. */
  Cache_Object *cptr ;           /* Cache Cache_Object pointer. */
  Boolean has_cache ;            /* Valid .cache file for this directory? */
  int ctext ;                    /* Offset in cache for text. */

/* View property options. */

  int num_file_chars ;          /* Number of filename chars on the screen. */
  int dispdir ;                 /* Display direction. */

  short display_mode ;          /* View by [icon|list|content]. */
  short sortby ;                /* Sort by [name|type|size|date]. */
  Boolean see_dot ;             /* see dot (invisible) files? */
  int listopts ;                /* List options. */
  int content_types ;           /* Types of files to view by content. */

  WCHAR filter[81] ;            /* Folder filter */
} File_Pane ;


typedef struct                   /* Custom command structure. */
{
  char *alias ;                  /* Custom command menu alias (if any). */
  char *command ;                /* UNIX command. */
  char *prompt ;                 /* Prompt popup text string. */
  char *is_prompt ;              /* Set "true" if there's a prompt window. */
  char *is_output ;              /* Set "true" if there's an output window. */
} Custom_Command ;


typedef struct fmObject
{
  Tree_Pane tree ;              /* Tree pane object. */

  File_Pane **file ;            /* Array of File pane objects. */
  Tree_Pane_Object **tpo ;      /* Array of Tree_Pane_Objects (for sorting). */

  short font_width[96] ;        /* Current font character widths. */
  int font_sizeW ;              /* Width of standard character. */
  int font_sizeH ;              /* Height of standard character. */

  int folder_color ;
  u_char red[MAXCOLORMAPSIZE] ;
  u_char green[MAXCOLORMAPSIZE] ;
  u_char blue[MAXCOLORMAPSIZE] ;

/* The pertinant colors to use in filemgr are the panel fore and background
 * colors, and the window foreground and background colors.
 */

  unsigned long Folder_fg ;     /* Foreground color index. */
  unsigned long Folder_bg ;     /* Background color index. */
  unsigned long Panel_fg ;
  unsigned long Panel_bg ;
  unsigned long Window_fg ;
  unsigned long Window_bg ;

/* Variables associated with panning. */

  int BRx ;                     /* Bottom-right limits for panning pane. */
  int BRy ;
  int panning ;                 /* Set TRUE when we have to scroll pane. */
  int pansx ;                   /* Current offset of vissible pane portion. */
  int pansy ;
  int TLx ;                     /* Top-left limits for panning pane. */
  int TLy ;
  int panx1 ;                   /* Previous position of the mouse. */
  int pany1 ;
  int panx2 ;                   /* Current position of the mouse. */
  int pany2 ;
  int panw ;                    /* Width of visible pane portion. */
  int panh ;                    /* Height of visible pane portion. */

/* Variable associated with icon display in path, tree and folder panes. */

  int Npath ;                   /* Number of path icons. */
  short Fontwidth[256] ;        /* Tree font character widths. */
  int StateChanged ;            /* Switch between tree/path. */
  int ascent ;                  /* To factor ascent of strings. */
  int descent ;                 /* To factor descent of strings. */
  int Renamed_file ;            /* Index in Fm->file[]. */

/*  XXX kludge - using this global to stop start_dnd() from printing a
 *  generic dnd error message.  Can't think of a better way to do it for now.
 */

  Boolean Show_dnd_error ;

/* Variables associated with the various popup windows. */

  FILE *fpout ;
  FILE *fperr ;
  char fout_fname[MAXPATHLEN] ;
  char *fout_ptr ;
  char ferr_fname[MAXPATHLEN] ;
  char *ferr_ptr ;
  char *Found_chosen ;
  int fout_eol ;
  int ferr_eol ;
  int Nfound ;
  int Pid ;
  int pstdout[2] ;     /* For pipes to find process. */
  int pstderr[2] ;
  u_char Type ;

  int Hscroll_unit ;            /* Horizontal scroll unit in the tree pane. */
  int Vscroll_unit ;            /* Vertical scroll unit in the tree pane. */

/* Variables associated with drag and drop. */

  int Catch_key  ;              /* Key to attach dnd receive context to. */
  int Pitch_key  ;              /* Key to attach selection owner to. */
  int Drop_site_key  ;          /* Key to attach drop sites with. */

/* Variables associated with custom commands and the file info. popup. */

  char *basename ;              /* Current file's base name only. */
  int no_ccmds ;                /* No. of custom cmds. in .desksetdefaults. */
  int nseln ;                   /* Number of files currently selected. */
  struct stat f_status ;        /* 1st file permissions status. */
  struct stat status ;          /* Current file status. */

/* Variables associated with CD and floppy support. */

  int cd_mask ;                 /* Mask of mounted CD devices. */

  int floppy_mask ;             /* Mask of mounted floppy devices. */

/* Misc. */

  time_t eCEmtime ;      /* Mtime for /etc/cetables/cetables. */
  time_t oCEmtime ;      /* Mtime for $OPENWINHOME/lib/cetables/cetables. */
  time_t uCEmtime ;      /* Mtime for ~/.cetables/cetables. */

  CEO ceo ;
  char *cebuf ;          /* Buffer for first n bytes of each file for CE. */

  time_t *mtime ;               /* Array of last modified times. */
  char *appname ;               /* Application name to use for resources. */
  char *client_data ;           /* Custom cmd or goto item client data. */
  char *cmntpt[MAXCD] ;         /* CD mount points. */
  char *crawpath[MAXCD] ;       /* CD raw device paths. */
  char *dirpath ;               /* Initial directory game on cmdline? */
  char *display_name ;          /* Possible X11 display information. */
  char err_file[32] ;
  char *f_owner ;               /* 1st owner name. */
  char *f_group ;               /* 1st group name. */
  char *fmntpt[MAXFLOPPY] ;     /* Floppy mount points. */
  char *frawpath[MAXFLOPPY] ;   /* Floppy raw device paths. */
  char *fullname ;              /* Current file's full pathname. */
  char *iconfont ;              /* User supplied icon font name (if any). */
  char *iconlabel ;             /* User supplied icon label (if any). */
  char *list_label ;            /* Custom cmd or goto list label. */
  char *newdoc ;                /* Initial new document name. */
  char *Progname ;              /* Program name. */
  char *pipe_name ;             /* Name of the vold named pipe. */
  char *shellname ;             /* Name of the shell "tool" to exec. */
  char **user_goto ;            /* Array of user goto pathnames. */
  char **user_goto_label ;      /* Array of user goto aliases. */
  char *acolor_res ;
  char *dcolor_res ;
  char *fcolor_res ;

  int ce_bufsize ;     /* Number of chars to read from each file for CE. */
  int custom_changed ;          /* Set if custom props changed in some way. */
  int maxwin ;         /* Max. no. of frames: main + waste + (n x popups). */
  int num_win ;        /* Number of filemgr folder window (plus waste). */

  int goto_changed ;            /* Set if goto props changed in some way. */
  int goto_chosen ;             /* Selected row in goto list (if any). */
  int ignore_goto_desel ;       /* Ignore goto deselection if set TRUE. */
  int ignore_custom_desel ;     /* Ignore custom deselection if set TRUE. */
  int max_user_goto ;           /* No. of user goto menu entries allocated. */
  int no_user_goto ;            /* Number of user goto menu entries. */

  int cur_interval ;            /* Current wakeup interval (in seconds). */
  int cur_prop_sheet ;          /* Current property sheet being displayed. */
  int defchar_height ;          /* Default height of a font character. */
  int drag_minx ;               /* Minimum X drag cursor value. */
  int drag_miny ;               /* Minimum Y drag cursor value. */
  int drag_maxx ;               /* Maximum X drag cursor value. */
  int drag_maxy ;               /* Maximum X drag cursor value. */
  int format_pid[MAXFLOPPY] ;   /* Set when formatting floppies. */
  int Err_fd ;                  /* Error files fd. */
  int gclimit ;                 /* Fmgc garbage collect limit (in bytes). */
  int goto_key ;                /* Key for goto menu items. */
  int goto_path_key ;           /* Key for goto path in menu items. */
  int interval ;                /* Interval between checking file sysytems. */
  int isgoto_select ;           /* Set for goto button click SELECT. */
  int marquis_started ;         /* Set when starting marquis selection. */
  int menu_defs[MAXMENUS] ;     /* Default values for filemgr menus. */
  int no_saved_wins ;           /* Number of window infos. in database. */
  int Null_fd ;                 /* Open on /dev/null (for dup()'ing stdin). */
  int openwins ;                /* Number of open windows. */
  int pipe_fd ;                 /* Named pipe file descriptor. */
  int popup_wno[MAXPOPUPS] ;    /* Window number for open popup's. */
  int popup_item[MAXPOPUPS] ;   /* Popup window item number in X->items. */
  int props_created ;           /* Set TRUE when props panel created. */
  int Renaming ;
  int resize_dispmode ;         /* Mode to display after RESIZE event. */
  int screen_height ;           /* Height of the screen in pixels. */
  int screen_width ;            /* Width of the screen in pixels. */
  int tree_gap ;                /* Gap between levels in the tree pane. */
  int tree_height ;             /* Height of the tree pane in pixels. */
  int tree_width ;              /* Width of the tree pane in pixels. */
  int tree_Xmax ;               /* Maximum calculated X coordinate in tree. */
  int tree_Xmin ;               /* Minimum calculated X coordinate in tree. */
  int tree_Ymax ;               /* Maximum calculated Y coordinate in tree. */
  int tree_Ymin ;               /* Minimum calculated Y coordinate in tree. */
  int unobscured ;              /* Number of windows unobscured. */
  int wno_key ;                 /* XV_KEY_DATA key to attach wno to */

  int adv_pw ;                  /* Width of props panel in advanced mode. */
  int cur_pw ;                  /* Width of props panel in current mode. */
  int cus_pw ;                  /* Width of props panel in custom mode. */
  int gen_pw ;                  /* Width of props panel in general mode. */
  int goto_pw ;                 /* Width of props panel in goto mode. */
  int new_pw ;                  /* Width of props panel in new mode. */

  int curr_wno ;                /* Index to current window in Fm->file[]. */
  int move_wno ;                /* Window potential move operation started. */
  int Rename_wno ;              /* Window number rename is taking place. */
  int YP_wno ;

  Delete_item DelList ;

/* Tree/Path pane variables. */

  Boolean Found_child ;         /* Did we find it? */
  Tree_Pane_Object *Child ;     /* Descendant node. */
  Tree_Pane_Object *Chosen ;    /* Chosen folder (mouse()). */
  Tree_Pane_Object *sort_tp ;   /* For sorting Tree_Pane_Objects. */

  Tree_polyline **tlines ;      /* Dynamic array of Tree_polylines. */
  int maxtpo ;                  /* Maximum number of Tree_Pane_Object's. */
  int maxtreelines ;            /* Maximum no. of Tree_polylines alloc'ed. */
  int ntpo ;                    /* Number of Tree_Pane_Objects used. */
  int ntreelines ;              /* Number of Tree_polylines used. */

/* Global Icon property options. */

  int newwin ;                  /* Open new window for dir. double click? */
  Boolean confirm_delete ;      /* Confirm deletions. */
  int num_file_chars ;          /* Number of filename chars on the screen. */
  char *newdir ;                /* Initial new folder name. */
  int dispdir ;                 /* Display direction. */

/* Global View property options. */

  short display_mode ;          /* View by [icon|list|content]. */
  short sortby ;                /* Sort by [name|type|size|date]. */
  Boolean see_dot ;             /* see dot (invisible) files? */
  int listopts ;                /* List options. */
  int content_types ;           /* Types of files to view by content. */

/* Global Goto property options. */

  int maxgoto ;                 /* Maximum number of Goto menu entries. */

/* Global Advanced property options. */

  WCHAR print_script[80] ;      /* Print script to use. */
  WCHAR filter[81] ;            /* Folder filter */
  short editor ;                /* Which document editor to use. */
  char *other_editor ;          /* Other editor to use. */

  char *fm_res[MAXRESOURCES] ;  /* Filemgr X resources. */

/*  Wastebasket and tree pane information. Filemgr tries to extract this
 *  information from the users ~/.desksetdefaults file. It is used to position
 *  and size the open window and the icon for the wastebasket and the tree
 *  pane, and to determine if they are open or closed initially.
 *
 *  [0] - set if wastebasket/tree is closed to an icon.
 *  [1] - x position of the wastebasket/tree icon.
 *  [2] - y position of the wastebasket/tree icon.
 *  [3] - x position of the open wastebasket/tree window.
 *  [4] - y position of the open wastebasket/tree window.
 *  [5] - width of the open wastebasket/tree window.
 *  [6] - height of the open wastebasket/tree window.
 */

  int waste_info[MAXINFO] ;
  int tree_info[MAXINFO] ;

  int system_pid ;             /* fm_system() procees id; 0 is no process. */
  WCHAR hostname[MAXHOSTNAMELEN] ;        /* Name of our host. */

  WCHAR *home ;                /* Home folder environment variable. */
  unsigned long black ;        /* This screens foreground pixel. */
  unsigned long white ;        /* This screens background pixel. */

  WCHAR **history ;
  int maxhistory ;             /* Maximum no. of history entries allocated. */
  int nhistory ;               /* Number of history goto menu entries. */

/* State Variables: either TRUE or FALSE. */

  Boolean DND_BY_BITS ;         /* Dnd by bits only? for debugging. */
  Boolean add_old ;             /* If true, include old custom commands. */
  Boolean autosort ;            /* Auto sort when files added/removed? */
  Boolean debug ;               /* Debugging mode? */
  Boolean casesen ;             /* Sort by name case sensitive? */
  Boolean cd_content ;          /* Set false if just name match for CD's. */
  Boolean check_all ;           /* Check dir & file modification times? */
  Boolean delete_fdr_prompt ;   /* Prompt on folder deletion? */
  Boolean destroy_fdr_prompt ;  /* Prompt on folder destruction? */
  Boolean error_state;		/* If true, error message stay in footer */
  Boolean floppy_content ;      /* Set false if just name match for floppies */
  Boolean follow_links ;        /* Follow symbolic links? */
  Boolean info_extended ;       /* Set if extended file info. showing. */
  Boolean load_icons ;          /* Load icon positional information? */
  Boolean load_state ;          /* Load directory state information? */
  Boolean multi_byte ;          /* Set true if running in a multibyte locale. */
  Boolean no_start_win ;        /* Set TRUE if no save window positions. */
  Boolean pos_or_size ;         /* Position or size given on cmdline? */
  Boolean redraw_tree ;         /* Set if tree view needs redrawing. */
  Boolean show_cd ;             /* If set false, don't show CD windows. */
  Boolean show_floppy ;             /* If set false, don't show CD windows. */
  Boolean show_quit_prompt ;    /* Prompt for "really quit?" action? */
  Boolean stopped ;             /* Has the Stop key been pressed? */
  Boolean treeview ;            /* Showing tree (vs. path) view? */
  Boolean use_cache ;           /* Read/write directory cache information? */
  Boolean use_ce ;              /* Use classing engine? */
  Boolean running ;             /* Filemgr in window main loop? */
  Boolean dont_paint ;          /* Delay executing full repaint proc? */
  Boolean fixed_font ;          /* Fixed width fonts? */
  Boolean fprops_showing ;      /* File properties popup displayed? */
  Boolean color ;               /* Color monitor? */
  Boolean sdir_given ;          /* Set if sort direction set on cmdline. */
  Boolean started_dnd ;         /* Did we start the dnd operation? */
  Boolean tree_vertical ;       /* Tree orientation vertical? */
  Boolean writable ;            /* Write in current directory? */
  Boolean link ;                /* Link rather than copy files? */
  Boolean left_scrollbar ;      /* Scrollbar on left? */
  Boolean volmgt ;		/* vold up and running ? */
} Fm_Object ;

typedef struct fmObject *FmVars ;

typedef struct
{
  int wno ;               /* Window index event occurred in. */
  int id ;                /* Event id, usually equals event_action(event). */
  int ch ;                /* ASCII value for keyboard events. */
  int x , y ;             /* X & y coord of event. */
  int old_x, old_y ;      /* X & y coord of last event. */
  int start_x, start_y ;  /* X & y coord of last down, button press. */
  long sec ;              /* Time in secs button click occurred. */
  long old_sec ;          /* Time in secs last button click occurred. */
  long usec ;             /* Time in usecs button click occurred. */
  long old_usec ;         /* Time in usecs last button click occurred. */

/* The event context is used for both file and tree panes.  The
 * appropiate pair of fields are set respectively.
 */

  int item ;                          /* Index number of item selected. */
  int old_item ;                      /* Index number of last item selected. */
  Tree_Pane_Object *tree_item ;       /* Selected tree item. */
  Tree_Pane_Object *old_tree_item ;   /* Last selected tree item. */

  Boolean down ;                      /* Button is down? */
  Boolean control ;                   /* Control key is down? */
  Boolean ispath ;                    /* Path pane event? */
  Boolean meta ;                      /* Meta key is down? */
  Boolean over_item ;                 /* Over an item in the pane? */
  Boolean over_name ;                 /* Over an item's filename in pane? */
  enum fm_event_type fm_id ;          /* Abstracted event type. */
  enum state_type state ;             /* State of tool when event arrived */
} Event_Context_Object, *Event_Context ;


Boolean exec_drop_method         P((char *, char *, char *, char *, int)) ;
Boolean find_pattern             P((char *, char *)) ;
Boolean is_dnd_target            P((File_Pane_Object *)) ;
Boolean is_frame                 P((int)) ;
Boolean is_frame_busy            P((int)) ;
Boolean is_frame_closed          P((int)) ;
Boolean is_image                 P((int, int)) ;
Boolean is_input_region          P((enum item_type)) ;
Boolean is_item                  P((enum item_type)) ;
Boolean is_menu                  P((enum menu_type)) ;
Boolean is_panpix                P(()) ;
Boolean is_popup_showing         P((enum item_type)) ;
Boolean is_resize_event          P(()) ;
Boolean is_visible               P((int)) ;
Boolean po_valid_values          P((int)) ;
Boolean valid_filename           P((char *)) ;
Boolean valid_new_command        P(()) ;
Boolean valid_print_cmd          P(()) ;

caddr_t get_icon_name            P((caddr_t)) ;

CE_ENTRY get_tns_entry           P((File_Pane_Object *, char *, int)) ;
CE_ENTRY get_tns_entry_by_bits   P((char *, int)) ;

char *connect_with_ce            P(()) ;
char *expand_filename            P((char *, char *)) ;
char *expand_keywords            P((char *, char *, char *, char *)) ;
char *fio_add_commas             P((off_t)) ;
char *fio_get_group              P((int)) ;
char *fio_get_name               P((int)) ;
char *fm_getcwd                  P((char *, int)) ;
char *fm_get_single_cmd          P((char *, int *, int *)) ;
char *fm_get_resource            P((enum db_type, enum res_type)) ;
char *fm_malloc                  P((unsigned int)) ;
char *fm_realpath                P((char *, char *)) ;
char *get_ccmd_str_resource      P((int, char *)) ;
char *get_default_drop_method    P((int)) ;
char *get_default_open_method    P((File_Pane_Object *, int)) ;
char *get_default_print_method   P(()) ;
char *get_default_type_name      P((int)) ;
char *get_drop_method            P((File_Pane_Object *)) ;
char *get_file_template          P((File_Pane_Object *, char *)) ;
char *get_goto_resource          P((enum goto_type, int)) ;
char *get_item_str_attr          P((enum item_type, enum item_attr_type)) ;
char *get_list_client_data       P((enum item_type, int)) ;
char *get_list_str               P((enum item_type, int)) ;
char *get_old_ccmd_str_resource  P((int, char *)) ;
char *get_open_method            P((File_Pane_Object *)) ;
char *get_path_item_value        P((int)) ;
char *get_pos_resource           P((char *, enum res_type, int *)) ;
char *get_print_method           P((File_Pane_Object *)) ;
char *get_rename_str             P((int)) ;
char *get_resource               P((enum db_type, char *)) ;
char *get_tns_attr               P((CE_ENTRY, CE_ATTRIBUTE)) ;
char *get_type_name              P((File_Pane_Object *)) ;
char *get_win_info               P((int, char *)) ;
char *make_list_time             P((time_t *)) ;
char *make_mem                   P((char *, int, int, int)) ;
char *make_time                  P((time_t *)) ;
char *quote_file_sel             P((int)) ;
char *set_bool                   P((int)) ;

Delete_item get_delete_path      P((WCHAR *)) ;

enum menu_type get_file_pane_menu   P((int)) ;
enum menu_type get_path_pane_menu   P((int)) ;
enum menu_type get_tree_pane_menu   P(()) ;
enum menu_type get_waste_pane_menu  P(()) ;

Event_Context check_if_over_item      P((Event_Context)) ;
Event_Context tree_check_if_over_item P((Event_Context)) ;

File_Pane_Object *get_first_selection P((int)) ;
File_Pane_Object **make_file_obj      P((int, char *, int, int)) ;

int add_children                P((register Tree_Pane_Object *,
                                   char *, Boolean *)) ;
int alloc_more_files            P((register int)) ;
int b_match                     P((char *, char *)) ;
int calc_days                   P((char *)) ;
int check_advanced_panel        P(()) ;
int check_cur_folder_panel      P(()) ;
int check_custom_panel          P(()) ;
int check_general_panel         P(()) ;
int check_goto_panel            P(()) ;
int check_media                 P((enum media_type, int, int *, int)) ;
int check_new_folder_panel      P(()) ;
int check_prop_values           P((int)) ;
int check_ps_args               P((int, char **, int [])) ;
int compare_dirpos              P((File_Pane_Object **, File_Pane_Object **)) ;
int compare_name                P((File_Pane_Object **,
                                   File_Pane_Object **)) ;
int compare_size                P((register File_Pane_Object **,
                                   register File_Pane_Object **)) ;
int compare_time                P((register File_Pane_Object **,
                                   register File_Pane_Object **)) ;
int compare_tree_node           P((Tree_Pane_Object **, Tree_Pane_Object **)) ;
int compare_type                P((register File_Pane_Object **,
                                   register File_Pane_Object **)) ;
int compare_with_plist          P((char *)) ;
int confirm_dir_delete          P((int, char *, int)) ;
int do_copy                     P((char *, char *)) ;
int do_cpo_print_proc           P(()) ;
int do_fastcopy                 P((char *, char *, struct stat *)) ;
int do_move                     P((char *, char *)) ;
int entry_found                 P((int, char *)) ;
int escape_arg                  P((char *, char *)) ;
int exists_prompt               P((int, char *)) ;
int expand_all                  P((register Tree_Pane_Object *)) ;
int fast_write                  P((char *, char *, int)) ;
int find_closest_grid_point     P((int, File_Pane_Object *, int *, int *)) ;
int fio_apply_change            P((char *)) ;
int fio_compare_and_update_panel P((File_Pane_Object *, int, int)) ;
int fio_prefix                  P((register char *, register char *)) ;
int fio_toggle_to_int           P(()) ;
int fm_create_wastebasket       P(()) ;
int fm_descendant               P((Tree_Pane_Object *, Tree_Pane_Object *)) ;
int fm_get_app_type             P((char *)) ;
int fm_getgid                   P((char *)) ;
int fm_getpath                  P((register Tree_Pane_Object *, WCHAR *)) ;
int fm_getuid                   P((char *)) ;
int fm_is_double_click          P((long, long, long, long,
                                   int, int, int, int)) ;
int fm_match_files              P((int, char *, int)) ;
int fm_new_folder_window        P((char *, char *, char *,
                                   enum media_type, int, int, int [])) ;
int fm_openfile                 P((char *, char *, int, int)) ;
int fm_over_item                P((int, int, int, int)) ;
int fm_over_listopt_item        P((int, int, int, int)) ;
int fm_regex_in_string          P((register char *)) ;
int fm_run_argv                 P((char **, int)) ;
int fm_run_str                  P((char *, int)) ;
int fm_set_canvas_height        P((int, int, int)) ;
int fm_set_canvas_width         P((int, int, int)) ;
int fm_stat                     P((char *, struct stat *)) ;
int fm_string_to_argv           P((char *, char *[])) ;
int fm_strlen                   P((register char *)) ;
int fm_writable                 P((char *)) ;
int get_bool_resource           P((enum db_type, enum res_type, int *)) ;
int get_bool_win_info           P((int, char *, int *)) ;
int get_canvas_attr             P((int, enum fm_can_type)) ;
int get_days                    P((register int, int, register int)) ;
int get_event_id                P(()) ;
int get_file_contents           P((FILE *, char *, int)) ;
int get_file_obj_x              P((int, File_Pane_Object *)) ;
int get_file_obj_y              P((int, File_Pane_Object *)) ;
int get_icon_xid                P((File_Pane_Object *, int)) ;
int get_int_resource            P((enum db_type, enum res_type, int *)) ;
int get_int_win_info            P((int, char *, int *)) ;
int get_item_int_attr           P((enum item_type, enum item_attr_type)) ;
int get_label_width             P((enum item_type)) ;
int get_list_first_selected     P((enum item_type)) ;
int get_menu_default            P((enum menu_type)) ;
int get_panel_item_value_width  P((enum item_type, char *)) ;
int get_pos_attr                P((int, enum pos_type)) ;
int get_pw_attr                 P((int, enum fm_can_type)) ;
int get_scrollbar_attr          P((int, enum fm_sb_type,
                                        enum fm_sb_attr_type)) ;
int get_str_resource            P((enum db_type, enum res_type, char *)) ;
int get_str_win_info            P((int, char *, char *)) ;
int get_selections              P((int, char *, int)) ;
int getunum                     P((char *)) ;
int give_file_warning           P(()) ;
int isempty                     P((WCHAR *)) ;
int is_recursive                P((int, char *, char *)) ;
int is_window_showing           P((char *, int)) ;
int item_prompt                 P((enum item_type, char *)) ;
int list_width                  P((int)) ;
int main                        P((int, char **)) ;
int make_duplicate              P((int, File_Pane_Object **, int, int)) ;
int make_key                    P(()) ;
int make_pos_dir                P((char *, int)) ;
int notice                      P((char *, char *, char *)) ;
int number_selected_items       P((int)) ;
int open_above_mount_point      P((int, char *)) ;
int open_files                  P((int, int, int, char *, int, int)) ;
int open_object                 P((File_Pane_Object *, char *, int, int,
                                   int, char *, int, int, int)) ;
int prompt_with_message         P((int, char *, char *, char *, char *)) ;
int quit_prompt                 P((int, char *, char *, char *)) ;
int read_encoded                P((register FILE *, register int,
                                   register unsigned char *, register int)) ;
int regex_match                 P((char *, char *)) ;
int regex_match_after_star      P((char *, char *)) ;
int sb_width                    P((int)) ;
int set_canvas_dims             P((int, int)) ;
int set_fp_image                P((char *, int, int)) ;
int shelf_held                  P(()) ;
int text_x_coord                P((int, int)) ;
int text_y_coord                P((int, int)) ;
int today_in_days               P(()) ;
int tree_join                   P((Tree_Pane_Object *)) ;
int tree_merge                  P((struct Tree_polygon *,
                                   struct Tree_polygon *)) ;
int tree_offset                 P((int, int, int, int, int ,int)) ;
int valid_advanced_panel        P(()) ;
int valid_custom_command        P(()) ;
int valid_goto_name             P((char *)) ;

mmap_handle_t *fast_read           P((char *)) ;

Tree_Pane_Object *make_tree_node   P((WCHAR *)) ;
Tree_Pane_Object *mouse            P((int, int, Tree_Pane_Object *)) ;
Tree_Pane_Object *path_chosen      P((int, int, int)) ;
Tree_Pane_Object *path_to_node     P((int, WCHAR *)) ;

Tree_polyline *tree_bridge         P((Tree_polyline *, int, int,
                                      Tree_polyline *, int, int)) ;
Tree_polyline *tree_line           P((int, int, Tree_polyline *)) ;

unsigned char get_hex              P((int)) ;

unsigned long compress_image       P((unsigned char *, int, int, int, int)) ;
unsigned long get_bg_index         P((File_Pane_Object *)) ;
unsigned long get_bitmap_content   P((char *)) ;
unsigned long get_default_bg_index P((int)) ;
unsigned long get_default_fg_index P((int)) ;
unsigned long get_fg_index         P((File_Pane_Object *)) ;
unsigned long get_icon_content     P((char *)) ;
unsigned long get_raster_content   P((char *)) ;
unsigned long get_xpm_content      P((char *, int *)) ;
unsigned long make_compressed      P((int, int, int, unsigned char *)) ;

void add_com_text               P((char *)) ;
void add_frame_menu_accel       P((int, enum menu_type)) ;
void add_menu_accel             P((enum menu_type, int, char *)) ;
void add_menu_help              P((enum menu_type, int, char *)) ;
void addparent_button           P(()) ;
void add_parent                 P((Tree_Pane_Object *)) ;
void add_tree_entry             P((Tree_Pane_Object *, WCHAR *)) ;
void adjust_entry               P((char *, char *)) ;
void adjust_path_height         P((int)) ;
void adjust_pos_info            P((int)) ;
void adjust_prop_sheet_size     P((int)) ;
void adjust_tree                P((int)) ;
void animate_icon               P((int, int, int, int)) ;
void apply_advanced_panel       P((int *, int *)) ;
void apply_cur_folder_panel     P((int *, int *)) ;
void apply_custom_panel         P(()) ;
void apply_general_panel        P((int *)) ;
void apply_goto_panel           P(()) ;
void apply_new_folder_panel     P(()) ;
void apply_prop_values          P((int)) ;
void begin_buffer_tree_segments P(()) ;
void build_folder               P((time_t *, register int *, register int)) ;
void build_cmd_panel_list       P((enum item_type)) ;
void busy_cursor                P((int, int, int *)) ;
void calc_tree                  P((Tree_Pane_Object *)) ;
void change_custom_list_item    P((int)) ;
void change_goto_list_item      P((int, char *, char *)) ;
void change_pos_view            P((int)) ;
void change_view_style          P((int)) ;
void check_all_files            P((int)) ;
void check_args                 P(()) ;
void check_del_dir              P((char *)) ;
void check_stop_key             P((int)) ;
void check_viewable_rect        P((int, int, int, int, int)) ;
void clear_area                 P((int, int, int, int, int)) ;
void clear_list                 P((enum item_type)) ;
void clear_position_info        P((int)) ;
void compare_child              P((Tree_Pane_Object *,
                                   Tree_Pane_Object *, Boolean *)) ;
void condense_folder            P((int)) ;
void copy_like_dnd              P((Event_Context)) ;
void copy_link_dnd              P((Event_Context, int)) ;
void cpo_make                   P(()) ;
void create_fm_dir              P((char *)) ;
void create_fm_dirs             P(()) ;
void create_generic_icon_entry  P((int, unsigned short *, unsigned short *)) ;
void create_generic_list_icon_entry P((int, unsigned short *,
                                            unsigned short *)) ;
void cut_like_dnd               P((Event_Context)) ;
void delete_icon                P((int, int, int, int)) ;
void delete_selected            P((int, int)) ;
void delete_text                P((int, int, int, char *)) ;
void descendant                 P((register Tree_Pane_Object *)) ;
void display_all_folders        P((int, char *)) ;
void display_each_folder        P((int)) ;
void display_tree               P((Tree_Pane_Object *)) ;
void do_check_floppy            P(()) ;
void do_cleanup_icons           P(()) ;
void do_custom_list_deselect    P((int)) ;
void do_custom_list_select      P((Custom_Command *)) ;
void do_delete_wastebasket      P(()) ;
void do_dismiss_popup           P((enum item_type)) ;
void do_display_icon            P((int, int, int, int, int)) ;
void do_display_list            P((int, int, int, int, int)) ;
void do_drag_copy_move          P((Event_Context)) ;
void do_duplicate_file          P((int)) ;
void do_eject_disk              P(()) ;
void do_fi_basic                P(()) ;
void do_fi_extended             P(()) ;
void do_file_info_popup         P((int)) ;
void do_find_stderr             P((int)) ;
void do_find_stdout             P((int)) ;
void do_fio_apply_proc          P(()) ;
void do_find_button             P(()) ;
void do_fio_folder_by_content   P(()) ;
void do_folder_event            P((Event_Context)) ;
void do_format_disk             P((char *, char *, char *)) ;
void do_goto_list_deselect      P((int)) ;
void do_goto_list_select        P((int, char *, char *)) ;
void do_goto_menu               P((int, char *)) ;
void do_list_notify             P((char *, int)) ;
void do_marquis_selection       P((Event_Context)) ;
void do_new_dnd_drop            P((Event_Context)) ;
void do_panning                 P((Event_Context)) ;
void do_po_custom_add           P(()) ;
void do_po_goto_add             P(()) ;
void do_proceed_button          P(()) ;
void do_rename_disk             P(()) ;
void do_rename_make             P(()) ;
void do_show_tree               P((int, int)) ;
void do_stop_button             P(()) ;
void do_undelete                P(()) ;
void draw_folder                P((int, Tree_Pane_Object *, int, int)) ;
void draw_folder_icon           P((int, int, File_Pane_Object **, int)) ;
void draw_folder_text           P((int, int, File_Pane_Object **, int)) ;
void draw_folder_icon_and_text  P((int, File_Pane_Object **,
                                   int, int, int, int)) ;
void draw_icon                  P((int, int, int, int, File_Pane_Object *)) ;
void draw_ith                   P((int, int)) ;
void draw_list_icon             P((int, int, int, File_Pane_Object *)) ;
void draw_list_ith              P((int, int)) ;
void draw_path                  P((int, int)) ;
void draw_tree_line             P((int, int, int, int)) ;
void draw_tree_text             P((Tree_Pane_Object *)) ;
void env_insert                 P((struct env_list *, struct env_list **)) ;
void error                      P((int, char *, char *)) ;
void expand_all_nodes           P(()) ;
void fast_close                 P((mmap_handle_t * ptr)) ;
void file_content               P((int, int, int, int, int, int)) ;
void file_pane_menu             P((int)) ;
void file_text                  P((int, int, int, char *, int)) ;
void find_do_connections        P(()) ;
void find_free_grid_point       P((int, File_Pane_Object *)) ;
void find_make                  P(()) ;
void find_object                P(()) ;
void finish_marquis_selection   P((Event_Context)) ;
void finish_panning             P((Event_Context)) ;
void fio_clear_item             P((enum item_type, int)) ;
void fio_clear_panel            P(()) ;
void fio_clear_toggle           P((int, int)) ;
void fio_file_free              P(()) ;
void fio_make                   P(()) ;
void fio_set_panel_x_values     P(()) ;
void fio_update_panel           P((int)) ;
void flush_buffer_tree_segments P(()) ;
void flush_delete_list          P(()) ;
void fm_add_help                P((enum item_type, char *)) ;
void fm_adjust_sb               P((int, enum fm_sb_type, int)) ;
void fm_all_filedeselect        P(()) ;
void fm_all_pathdeselect        P((int)) ;
void fm_busy_cursor             P((int, int)) ;
void fm_canvas_page_scroll      P((int, enum action_type)) ;
void fm_center_disk_buttons     P((enum item_type, enum item_type,
                                   enum item_type)) ;
void fm_center_info_buttons     P((enum item_type, enum item_type,
                                   enum item_type)) ;
void fm_center_prop_buttons     P((enum item_type, enum item_type,
                                   enum item_type, enum item_type)) ;
void fm_center_rcp_buttons      P((enum item_type, enum item_type,
                                   enum item_type)) ;
void fm_center_rename_buttons   P((enum item_type, enum item_type,
                                   enum item_type)) ;
void fm_check_keys              P(()) ;
void fm_clrmsg                  P(()) ;
void fm_create_menus            P(()) ;
void fm_create_tree_canvas      P(()) ;
void fm_destroy_tree            P((register Tree_Pane_Object *)) ;
void fm_destroy_window          P((int)) ;
void fm_display_folder          P((int, int)) ;
void fm_drawtree                P((int)) ;
void fm_draw_rect               P((int, int, int, int, int)) ;
void fm_file_scroll_height      P((int)) ;
void fm_file_scroll_width       P((int)) ;
void fm_filedeselect            P((int)) ;
void fm_font_and_sv_init        P(()) ;
void fm_free_argv               P((char *[])) ;
void fm_include_old             P((char *)) ;
void fm_increment_copy_number   P((char *, char *, char *, char *)) ;
void fm_init_colormap           P(()) ;
void fm_init_font               P(()) ;
void fm_init_path_tree          P((int)) ;
void fm_make_panpix             P((int, int)) ;
void fm_menu_create             P((enum menu_type)) ;
void fm_msg                     P((int, char *, ...)) ;
void fm_namestripe              P((int)) ;
void fm_open_folder             P((int)) ;
void fm_open_path               P((int, int)) ;
void fm_pane_to_pixmap          P((int, int, int)) ;
void fm_pathdeselect            P((int)) ;
void fm_popup_create            P((enum popup_type)) ;
void fm_position_popup          P((int, enum item_type)) ;
void fm_process_cmdfile         P(()) ;
void fm_put_resource            P((enum db_type, enum res_type, char *)) ;
void fm_rename                  P((int, int)) ;
void fm_resize_text_item        P((enum item_type, enum item_type)) ;
void fm_scroll_to               P((int, int)) ;
void fm_scrollbar_scroll_to     P((int, enum fm_sb_type, int)) ;
void fm_set_folder_colors       P(()) ;
void fm_set_pan_cursor          P((int, int)) ;
void fm_showerr                 P((char *)) ;
void fm_showopen                P(()) ;
void fm_sort_children           P((Tree_Pane_Object *)) ;
void fm_treedeselect            P(()) ;
void fm_tree_font_and_color     P(()) ;
void fm_update_pan              P((int)) ;
void fm_visiblefolder           P((register Tree_Pane_Object *)) ;
void fm_window_create           P((enum window_type, int)) ;
void free_fp_image              P((File_Pane_Object **)) ;
void free_panpix_image          P(()) ;
void garbage_collect            P(()) ;
void get_options                P((int, char *[])) ;
void get_win_X_info             P((int, int [])) ;
void goto_object                P(()) ;
int handle_floppy_rename       P((char *, int, int)) ;
void handle_media_removal       P((enum media_type, int)) ;
void handle_move_copy           P((Event_Context)) ;
void handle_resize              P((int)) ;
void hide_node                  P(()) ;
void incremental_selection      P((Event_Context, int, int)) ;
void init_advanced_panel        P(()) ;
void init_atoms                 P(()) ;
void init_ce                    P((int)) ;
void init_cur_folder_panel      P(()) ;
void init_custom_panel          P(()) ;
void init_filemgr               P(()) ;
void init_gcs                   P(()) ;
void init_general_panel         P(()) ;
void init_generic_icons         P(()) ;
void init_generic_list_icons    P(()) ;
void init_goto_panel            P(()) ;
void init_new_folder_panel      P(()) ;
void init_text                  P(()) ;
void init_toggles               P(()) ;
void init_ui_study              P(()) ;
void itimer_system_check        P(()) ;
int fm_verify_volmgt		P(()) ;
void keep_panel_up              P((enum item_type)) ;
void link_comment_items         P(()) ;
void link_cp_items              P(()) ;
void link_find_items            P(()) ;
void link_fio_items             P(()) ;
void link_like_dnd              P((Event_Context)) ;
void link_po_items              P(()) ;
void link_rcp_items             P(()) ;
void link_rename_items          P(()) ;
void load_cache                 P((int)) ;
void load_deskset_defs          P(()) ;
void load_dir_defs              P((char *)) ;
void load_directory_info        P((int, int, int [])) ;
void load_and_draw_folder       P((int)) ;
void load_goto_list             P(()) ;
void load_icon_positions        P((int)) ;
void load_old_ccmd_defs         P((char *)) ;
void load_positional_info       P((int, char *)) ;
void load_resources             P(()) ;
void make_busy_cursor           P(()) ;
void make_drag_cursor           P((Event_Context)) ;
void make_new_frame             P((int, enum media_type)) ;
void make_panning_cursor        P(()) ;
void make_pipe                  P(()) ;
void mitem_inactive             P((enum menu_type, int, int)) ;
void mitem_label                P((enum menu_type, int, WCHAR *)) ;
void mitem_set                  P((enum menu_type, int, int)) ;
void mouse_recursive            P((register int, register int,
                                   register Tree_Pane_Object *)) ;
void my_stat                    P((char *, File_Pane_Object **, int, int)) ;
void new_file                   P((int, char *)) ;
void open_dir                   P((File_Pane_Object *, int)) ;
void open_goto_file             P((int, char *)) ;
void over_path_tree_folder      P((int, Tree_Pane_Object *, int)) ;
void paste_like_dnd             P((Event_Context)) ;
void path_tree_event            P((Event_Context)) ;
void path_tree_select_item      P((Event_Context)) ;
void po_make                    P(()) ;
void pos_eitem_messages         P(()) ;
void print_files                P((char *, int, int)) ;
void put_resource               P((enum db_type, char *, char *)) ;
void put_pos_resource           P((char *, enum res_type, char *)) ;
void put_win_info               P((int, char *, char *)) ;
int  quit_waste                 P(()) ;
void rcp_make                   P(()) ;
void rcp_prompt                 P((char *, char *)) ;
void read_menu_defaults         P(()) ;
void read_resources             P(()) ;
void read_str                   P((char **, char *)) ;
void read_user_goto_menu        P(()) ;
void read_window_info           P((int, int [])) ;
void read_window_positions      P((int)) ;
void rebuild_folder             P((int)) ;
void redisplay_folder           P((int, int, int, int, int)) ;
void remove_delete_path         P((WCHAR *)) ;
void remove_tree_entry          P((Tree_Pane_Object *, WCHAR *)) ;
void reposition_status          P((int)) ;
void reset_advanced_panel       P(()) ;
void reset_cur_folder_panel     P(()) ;
void reset_custom_fields        P(()) ;
void reset_custom_panel         P(()) ;
void reset_file_pane            P((int)) ;
void reset_goto_panel           P(()) ;
void reset_new_folder_panel     P(()) ;
void reset_prop_values          P((int)) ;
void resize_arrays              P((int, int)) ;
void resize_history             P((int, int)) ;
void resize_path_text_item      P((int)) ;
void resize_prop_sheet          P((enum item_type, enum item_type,
                                   enum item_type, int)) ;
void resize_window_drop_sites   P((int, int)) ;
void resize_X_arrays            P((int, int)) ;
void reverse_ith                P((int, int)) ;
void rn_hidden_case          P((char *, char *, char *)) ;
void rn_no_template		P((char *, char *, char *, char *)) ;
void rn_other_cases          P((char *, char *, char *, char *)) ;
void save_cmdline               P((int, char **)) ;
void save_delete_path           P((WCHAR *, WCHAR *)) ;
void save_dir_defs              P((char *)) ;
void save_directory_info        P((int)) ;
void save_resources             P(()) ;
void save_window_info           P((int)) ;
void save_window_positions      P(()) ;
void scroll_to_file_and_select  P((char *, char *)) ;
void scrunch_window             P((int, int)) ;
void select_item                P((Event_Context, enum select_type)) ;
void select_list_item           P((enum item_type, int, int)) ;
void set_busy_cursor            P((int, int)) ;
void set_canvas_attr            P((int, enum fm_can_type, int)) ;
void set_ccmd_str_resource      P((int, char *, char *)) ;
void set_color                  P((char *, int)) ;
void set_copy_event             P((Event_Context)) ;
void set_cur_folder_panel       P(()) ;
void set_custom_menu            P((enum menu_type)) ;
void set_default                P((int)) ;
void set_default_item           P((enum item_type, enum item_type)) ;
void set_dnd_selection          P((Event_Context)) ;
void set_edit_button_menu       P(()) ;
void set_event_state            P((Event_Context, enum state_type)) ;
void set_event_time             P((Event_Context)) ;
void set_file_button_menu       P(()) ;
void set_file_obj_x             P((int, File_Pane_Object *, int)) ;
void set_file_obj_y             P((int, File_Pane_Object *, int)) ;
void set_fp_color               P((File_Pane_Object *, enum fm_fp_type,
                                   enum fm_color_type)) ;
void set_frame_attr             P((int, enum fm_frame_type, int)) ;
void set_frame_icon             P((int, enum media_type)) ;
void set_goto_button_menu       P(()) ;
void set_goto_resource          P((enum goto_type, int, char *)) ;
void set_grid_points            P((int)) ;
void set_home                   P(()) ;
void set_icon_pos               P((int, File_Pane_Object *)) ;
void set_item_int_attr          P((enum item_type, enum item_attr_type, int)) ;
void set_item_str_attr          P((enum item_type, enum item_attr_type,
                                   char *)) ;
void set_layout_item            P((int)) ;
void set_list_item              P((int)) ;
void set_menu_default           P((enum menu_type, int)) ;
void set_menu_items             P((enum menu_type, void (*)())) ;
void set_menu_window_no         P((enum menu_type, int)) ;
void set_path_item_value        P((int, char *)) ;
void set_print_method           P(()) ;
void set_pos_attr               P((int, enum pos_type, int)) ;
void set_root                   P(()) ;
void set_scrollbar_attr         P((int, enum fm_sb_type, enum fm_sb_attr_type,
                                   int)) ;
void set_status_glyph           P((int, enum media_type)) ;
void set_status_info            P((int)) ;
void set_timer                  P((int)) ;
void set_toggle_xvals           P((int, int)) ;
void set_toggles                P((struct stat, int)) ;
void set_tree_icon              P((Tree_Pane_Object *, int)) ;
void set_tree_file_button_menu  P(()) ;
void set_tree_title             P((Tree_Pane_Object *)) ;
void set_tree_view_button_menu  P(()) ;
void set_view_button_menu       P(()) ;
void set_waste_edit_menu        P(()) ;
void set_waste_icon             P(()) ;
void set_waste_pane_menu        P(()) ;
void set_window_dims            P((int, int [])) ;
void set_window_params          P((int)) ;
void sort_tree                  P((Tree_Pane_Object *)) ;
void start_dnd                  P((Event_Context)) ;
void start_popup                P(()) ;
void start_tool                 P(()) ;
void stop_move_operation        P(()) ;
void stop_rename                P(()) ;
void sync_server                P(()) ;
void time_check                 P(()) ;
void toggle_hidden_files        P((int)) ;
void toggle_item                P((Event_Context, enum select_type)) ;
void tree_attach_parent         P((Tree_Pane_Object *, int)) ;
void tree_calculate_dims        P((Tree_Pane_Object *)) ;
void tree_layout                P((Tree_Pane_Object *)) ;
void tree_layout_leaf           P((Tree_Pane_Object *)) ;
void tree_path_menu             P((int)) ;
void unique_filename            P((char *)) ;
void update_history             P((int)) ;
void usage                      P((char *)) ;
void version                    P((char *)) ;
void write_cache                P((int)) ;
void write_cmdline              P(()) ;
void write_custom_commands      P(()) ;
void write_dir_defs             P((int)) ;
void write_footer               P((int, char *, int)) ;
void write_icon_positions       P((int)) ;
void write_item_footer          P((enum item_type, char *, int)) ;
void write_menu_defaults        P(()) ;
void write_menu_def_val         P((enum menu_type, enum res_type)) ;
void write_positional_info      P((int, char *)) ;
void write_questions            P(()) ;
void write_resources            P(()) ;
void write_user_goto_menu       P(()) ;
void yield_shelf                P(()) ;

WCHAR *fmtmode                  P((WCHAR *, int)) ;

/* Classing engine prototypes. */

CE_ATTRIBUTE ce_get_attribute_id    P((CE_NAMESPACE, char *)) ;
CE_NAMESPACE ce_get_namespace_id    P((char *)) ;
char *ce_get_attribute              P((CE_NAMESPACE, CE_ENTRY, CE_ATTRIBUTE)) ;
int ce_begin                        P((void *)) ;

extern char *fm_mbchar() ;
extern int fm_mbstrlen() ;
extern int fm_nchars() ;

/* Prototypes missing from the original DeskSet headers. */
int fm_match(char *text, char *pattern);
struct Dnd_Receive_Struct;
Boolean move_by_filename(char *file, char *dest, struct Dnd_Receive_Struct *catcher);
Boolean valid_new_cmd(void);
void dout(char *format, ...);
void err_exit(char *file, int line, char *format, ...);

int is_window_showing_with_path(char *pathname);
