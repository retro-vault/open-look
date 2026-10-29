
/*  @(#)xdefs.h	1.53 02/09/93
 *
 *  Copyright (c) 1987-1991 Sun Microsystems, Inc.
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

#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/cms.h>
#include <xview/panel.h>
#include <xview/font.h>
#include <xview/canvas.h>
#include <xview/cursor.h>
#include <xview/scrollbar.h>
#include <xview/xv_xrect.h>
#include <xview/svrimage.h>
#include <xview/dragdrop.h>
#include <X11/Xresource.h>

/* Use to compile special filemgr with Internationalization features. */

#ifdef OW_I18N
#include <locale.h>
#include <widec.h>
#include <wctype.h>
#include <stdlib.h>
#include <xview/xv_i18n.h>
#endif /*OW_I18N*/

/* Data Structures. */

/*  Structure to hold cache'd icon images in RB tree.  Many thanks
 *  to Nannette Simpson for her RB tree routines which are used here.
 */

typedef struct Image_Struct {
  WCHAR *name ;                    /* Filename of icon. */
  Server_image image ;             /* Image (or mask image). */
  int xid ;                        /* Xid of image. */
  int hits ;                       /* Number of times image was accessed. */
} Image_Object, *Image_Ptr ;

/* Structure for maintaining the dnd atoms we are interested in. */

typedef struct Atoms_Struct {
  Atom text ;           /* Native format of data. */
  Atom incr ;
  Atom targets ;        /* List of targets supported by sender. */
  Atom length ;
  Atom delete ;
  Atom null ;
  Atom file ;           /* Name of file. */
  Atom name ;           /* Name of application supplying selection data. */
  Atom host ;           /* Name of host FILE_NAME is on. */
  Atom location ;       /* X,Y location of the object relative to the 0th item. */
  Atom cursor_loc ;     /* X,Y location of the cursor relative to the 0th item. */
  Atom atm_file ;       /* The file name transport. */
  Atom done ;           /* ??? */
  Atom label ;          /* Last component of pathname. */
  Atom atm ;            /* List of atoms showing atom's sender supports. */
  Atom available ;      /* Different types sender is willing to source. */
  Atom enum_count ;     /* Number of items in current selection. */
  Atom enum_item ;      /* Current item, range = [0..enum_count-1]. */
  Atom execute ;        /* True if dnd object is "executable". */
  Atom link_copy ;      /* True if dnd object is "executable". */
  Atom current  ;       /* Current item we are looking at. */
} Atoms_Object ;

typedef struct Atoms_Struct *Atoms ;

typedef struct                   /* Tree Pane XView/Xlib structure. */
{
  Frame frame ;                  /* Frame. */
  Canvas canvas ;                /* Canvas. */
  Panel panel ;                  /* Command panel. */
  Xv_Window pw ;                 /* Paintwindow. */
  Drawable xid ;                 /* Xlib drawing handle. */
  Panel_item file_button ;       /* Control panel File button. */
  Panel_item view_button ;       /* Control panel View button. */
  Panel_item goto_button ;       /* Control panel Goto button. */
} Tree_PaneX ;


typedef struct                   /* File Pane XView/Xlib structure. */
{
  Tree_PaneX pathX ;             /* Path pane for this folder. */
  Frame frame ;                  /* Frame. */
  Canvas canvas ;                /* Canvas. */
  Panel panel ;                  /* Command panel. */
  Panel status_panel ;           /* Status line panel. */
  Xv_Window pw ;                 /* Paintwindow. */
  Drawable xid ;                 /* Xlib drawing handle. */
  Panel Rename_panel ;           /* Used for renaming files. */
  Panel_item Path_item ;         /* Goto line item. */
  Panel_item status_glyph ;      /* Possible lock or read-only glyph. */
  Panel_item leftmess ;          /* Left status panel message item. */
  Panel_item rightmess ;         /* Right status panel message item. */
  Panel_item Rename_item ;
  Panel_item file_button ;       /* Control panel File button. */
  Panel_item view_button ;       /* Control panel View button. */
  Panel_item edit_button ;       /* Control panel Edit button. */
  Panel_item goto_button ;       /* Control panel Goto button. */
  Panel_item eject_button ;      /* Status panel Eject Disk button. */
} File_PaneX ;

/* Context structure for describing the current drag/drop SEND action. */

typedef struct Pitch_File_Struct
{
  char *name ;                        /* Full pathname of file. */
  char *data_label ;                  /* Filename only. */
  int   size ;                   /* Size of file. */
  int   wno ;                    /* Index into Fm.file[wno] array. */
  int   index ;                  /* Index into Fm.file[wno].object array. */
  BYTE  type ;                   /* Same type as in File Pane Objects. */
  int   x_loc;                   /* object x location relative to the 0th item */
  int   y_loc;                   /* object y location relative to the 0th item */
  Boolean location_good;         /* usable location stored in this structure */
  Atom  type_name_atom ;         /* SUN_TYPE_<tns type name> atom. */
} Pitch_File_Object ;


typedef struct Dnd_Send_Struct
{
  char hostname[MAXHOSTNAMELEN] ;  /* Hostname of sender. */
  Selection sel ;
  int item ;                       /* index to filename we are working on. */
  int num_items ;                  /* number of items. */
  int x_loc;                       /* cursor x location relative to the 0th item */
  int y_loc;                       /* cursor y location relative to the 0th item */
  Pitch_File_Object *files ;       /* ptr to array of file structs. */
  char xfer_buf[XFER_BUFSIZ] ;     /* buffer used to transfer data. */
  FILE *fp ;                       /* used to read & transfer file bits. */
  Boolean bitsending ;             /* are we transferring bits? */
  Boolean link_copy ;              /* Are we linking in case of a copy? */
  Boolean cut_to_clipboard ;       /* Are files in this pitch to be deleted
                                      when the selection is lost? */
} Dnd_Send_Object ;

typedef struct Dnd_Send_Struct *Pitch ;

typedef struct Pitch_List_Struct {
  Pitch pitch ;   /* Pointer to the associated pitch structure. */
  Atom rank ;     /* Atom representing selection rank for that transaction. */
  struct Pitch_List_Struct *right ;      /* Right pointer */
  struct Pitch_List_Struct *left ;       /* Left pointer */
} Pitch_List_Object ;

/* Context structure for recording display list after dnd action. */

typedef struct Dnd_Display_Struct
{
  char *dir_name ;
  char *justify ;
  struct Dnd_Display_Struct *next ;
} Dnd_Display_List ;


/* Context structure for describing the current drag/drop RECEIVE action. */

typedef struct Dnd_Receive_Struct
{
  Atom *target_list ;
  int  num_targets ;
  Atom *transport_list ;
  int  num_transports ;
  char *data_label ;
  int  num_items ;
  int  cursor_x ;
  int  cursor_y ;
  Boolean cursor_loc_good;
  Boolean is_paste ;             /* Set true if Paste operation. */
  Boolean over_folder ;          /* Set true for dnd over folder pane icon. */
  char *src_hostname ;           /* Name of sender's host. */
  char *src_filename ;           /* Full pathname of file being sent. */
  int  object_x ;
  int  object_y ;
  Boolean object_loc_good;
  Atom chosen_target ;
  Atom chosen_transport ;
  int  transfer_state ;
  int  processing_stream ;
  int  state_mask ;
  int  stop_hit ;
  int  items_caught ;
  int  dest_wno ;               /* Window number the drop occured in. */
  int  x ;                      /* X position of drop. */
  int  y ;                      /* Y position of drop. */
  char tmpfile[MAXPATHLEN] ;    /* Temporary file to store rec'd bits in. */
  FILE *fp ;                    /* Tmpfile pipe/file pointer. */
  char destdir[MAXPATHLEN] ;    /* Destination dir to send dropped files. */

/* These next 2 fields only set if we are dropping onto an object which
 * accepts dnd drops.
 */

  char *over_file ;             /* Destination file to send dropped files. */
  char *drop_method ;           /* Drop method to use on destination file. */
  enum Stop_State_Type stop_state ;  /* KLUDGE to interrupt the dnd catch. */
  Boolean copying ;             /* Are we copying or moving the catch. */
  Boolean executable ;          /* True if file is executable. */

/* Redisplay list for after dnd has finished. */

  struct Dnd_Display_Struct *display_list ;
} Dnd_Receive_Object ;

typedef struct Dnd_Receive_Struct *Catch ;

typedef struct Xobject            /* XView/Xlib graphics object. */
{
  Tree_PaneX treeX ;              /* Tree pane XView/Xlib object. */
  File_PaneX **fileX ;            /* File pane XView/Xlib objects. */
  Display *display ;              /* Display id of frame. */
  Event *event ;                  /* Current event. */
  Frame base_frame ;              /* The hidden "main" window. */
  Icon Waste_icon ;
  Menu menus[MAXMENUS] ;          /* Various popup menus used by filemgr. */
  Selection_requestor sel ;       /* Used to stop dnd operations. */
  Server_image Fm_list_image[3] ; /* Used by the make_popup. */
  Visual *visual ;
  XrmDatabase desksetDB ;         /* Deskset resources database. */
  XrmDatabase dirDB ;             /* Directory resources database. */
  XrmDatabase old_ccmdDB ;        /* Old custom commands database. */
  XrmDatabase rDB ;               /* Combined resources database. */
  Xv_Server server ;
  Xv_font file_font ;             /* Current file pane font. */
  Xv_font tree_font ;             /* Current tree pane font. */
  Xv_opaque items[MAXITEMS] ;     /* Various XView items used by filemgr. */

  XSegment cached_segments[SEGCACHESIZE] ;   /* Cache of tree line segments. */
  int num_cachesegs ;                        /* Number of cached segments. */

  Dnd dnd_object ;

  Image_Ptr Gen_icon_image[FT_MAX_TYPES] ;   /* Generic icon images. */
  Image_Ptr Gen_icon_mask [FT_MAX_TYPES] ;   /* Generic mask images. */
  Image_Ptr Gen_list_image[FT_MAX_TYPES] ;   /* Generic list icon images. */
  Image_Ptr Gen_list_mask [FT_MAX_TYPES] ;   /* Generic list mask images. */

  Selection_owner Shelf_owner ;
  Selection_item  Shelf_item ;

  Xv_Cursor busy_cursor ;
  Xv_Cursor drag_cursor ;
  Xv_Cursor folder_drag_move_cursor ;
  Xv_Cursor folders_drag_move_cursor ;
  Xv_Cursor folder_drag_copy_cursor ;
  Xv_Cursor folders_drag_copy_cursor ;
  Xv_Cursor folder_accept_move_cursor ;
  Xv_Cursor folders_accept_move_cursor ;
  Xv_Cursor folder_accept_copy_cursor ;
  Xv_Cursor folders_accept_copy_cursor ;

  Panel_item toggle[NUM_TOGGLES] ;
  Xv_Singlecolor colors[MAXCOLORMAPSIZE] ;
  unsigned long *xcolors ;
  Cms cms ;

  GC GCimage ;           /* GC for image drawing (rop ops). */
  GC GCline ;            /* GC for line drawing (3d shading). */
  GC GCttext ;           /* GC for Tree text and stencil drawing. */
  GC GCftext ;           /* GC for File text and stencil drawing. */
  GC GCpangc ;           /* GC for panning in tree and folder pane[s]. */
  GC GCmargc ;           /* GC for marquis selection in the folder pane[s]. */
  GC GCcursor ;          /* GC for drawing images on the cursor. */
  GC GCselect ;          /* GC for icon [de]selection. */
  GC igc ;

  XGCValues GCval ;      /* GC values for drawing. */
  XGCMask   GCmask ;     /* Function mask for GC values. */

/* Variable associated with icon display in path, tree and folder panes. */

#ifdef OW_I18N
  Font font ;                       /* Used for i18n to get the font_set. */
  XFontSet font_set ;
  XFontSetExtents *font_extents ;
#else
  int font_set ;                    /* Dummy to avoid compilation errors. */
#endif /*OW_I18N*/

  XFontStruct *fontstruct ;

/* Variables associated with drag and drop. */

  struct Pitch_List_Struct *Pitch_list ;
  Atoms A ;

/* Variables associated with custom commands and the file info. popup. */

  Frame Cmd_run_frame ;
  Menu_item Cmd_exec_mi ;
  Panel_item Cmd_run_message ;
  Panel_item Cmd_run_args ;

/* Variables associated with panning. */

  Pixmap panpix ;               /* Offscreen copy of the panned pane. */
  Xv_Cursor def_cursor ;        /* Default cursor for the panned pane. */
  Xv_Cursor pan_cursor ;        /* Cursor displayed whilst panning. */
} XObject ;

typedef struct Xobject *FMXlib ;

Boolean move_by_filename         P((char *, char *, Catch)) ;

Dnd create_dnd_object            P((Event_Context)) ;

Drawable get_generic_icon_xid      P((int, int)) ;
Drawable get_generic_list_icon_xid P((int, int)) ;

Event_Context setup_event_context  P((Canvas, Event *, int)) ;

int fm_cmd_list_notify          P((Panel_item, char *, Xv_opaque,
                                   Panel_list_op, Event *, int)) ;
int goto_list_notify            P((Panel_item, char *, Xv_opaque,
                                   Panel_list_op, Event *, int)) ;
int list_notify                 P((Panel_item, char *, Xv_opaque,
                                   Panel_list_op, Event *, int)) ;

Menu_item change_tree_dir       P((Menu_item, Menu_generate)) ;
Menu_item check_floppy          P((Menu_item, Menu_generate)) ;
Menu_item cleanup_icons         P((Menu_item, Menu_generate)) ;
Menu_item create_document       P((Menu_item, Menu_generate)) ;
Menu_item create_folder         P((Menu_item, Menu_generate)) ;
Menu_item create_display_popup  P((Menu_item, Menu_generate,
                                   Xv_Window, void (*)())) ;
Menu_item create_shell          P((Menu_item, Menu_generate)) ;
Menu_item custom_print_popup    P((Menu_item, Menu_generate)) ;
Menu_item default_print_proc    P((Menu_item, Menu_generate)) ;
Menu_item delete                P((Menu_item, Menu_generate)) ;
Menu_item delete_wastebasket    P((Menu_item, Menu_generate)) ;
Menu_item do_dnd                P((Menu_item, Menu_generate,
                                   void (*)(Event_Context))) ;
Menu_item fi_basic              P((Menu_item, Menu_generate)) ;
Menu_item fi_extended           P((Menu_item, Menu_generate)) ;
Menu_item file_info_popup       P((Menu_item, Menu_generate)) ;
Menu_item fm_addparent_button   P((Menu_item, Menu_generate)) ;
Menu_item fm_copy_like_dnd      P((Menu_item, Menu_generate)) ;
Menu_item fm_cut_like_dnd       P((Menu_item, Menu_generate)) ;
Menu_item fm_delete_object      P((Menu_item, Menu_generate)) ;
Menu_item fm_expand_all_nodes   P((Menu_item, Menu_generate)) ;
Menu_item fm_find_object        P((Menu_item, Menu_generate)) ;
Menu_item fm_hide_node          P((Menu_item, Menu_generate)) ;
Menu_item fm_link_like_dnd      P((Menu_item, Menu_generate)) ;
Menu_item fm_props              P((Menu_item, Menu_generate)) ;
Menu_item fm_remote_transfer    P((Menu_item, Menu_generate)) ;
Menu_item fm_select_all         P((Menu_item, Menu_generate)) ;
Menu_item fm_set_root           P((Menu_item, Menu_generate)) ;
Menu_item fm_show_tree          P((Menu_item, Menu_generate)) ;
Menu_item format_disk           P((Menu_item, Menu_generate)) ;
Menu_item goto_menu_proc        P((Menu_item, Menu_generate)) ;
Menu_item make_comments         P((Menu_item, Menu_generate)) ;
Menu_item om_open_file          P((Menu_item, Menu_generate)) ;
Menu_item open_in_editor        P((Menu_item, Menu_generate)) ;
Menu_item open_tree_folder      P((Menu_item, Menu_generate)) ;
Menu_item paste_like_dnd_from_menu P((Menu_item, Menu_generate)) ;
Menu_item pp_tp_open_dir        P((Menu_item, Menu_generate)) ;
Menu_item really_quit           P((Menu_item, Menu_generate)) ;
Menu_item rename_make           P((Menu_item, Menu_generate)) ;
Menu_item undelete              P((Menu_item, Menu_generate)) ;
Menu_item up                    P((Menu_item, Menu_generate)) ;
Menu_item vm_icon_name          P((Menu_item, Menu_generate)) ;
Menu_item vm_icon_type          P((Menu_item, Menu_generate)) ;
Menu_item vm_list_date          P((Menu_item, Menu_generate)) ;
Menu_item vm_list_name          P((Menu_item, Menu_generate)) ;
Menu_item vm_list_size          P((Menu_item, Menu_generate)) ;
Menu_item vm_list_type          P((Menu_item, Menu_generate)) ;
Menu_item vm_pos_icon           P((Menu_item, Menu_generate)) ;
Menu_item vm_pos_list           P((Menu_item, Menu_generate)) ;

Notify_value destroy_window     P((Notify_client, Destroy_status)) ;
Notify_value Fm_frame_event     P((Frame, Event *,
                                   Notify_arg, Notify_event_type)) ;
Notify_value Fm_tree_event      P((Frame, Event *,
                                   Notify_arg, Notify_event_type)) ;
Notify_value find_interpose     P((Frame, Event *,
                                   Notify_arg, Notify_event_type)) ;
Notify_value find_stdout        P((Notify_client, int)) ;
Notify_value find_stderr        P((Notify_client, int)) ;
Notify_value folder_event       P((Canvas, Event *,
                                   Notify_arg, Notify_event_type)) ;
Notify_value path_event         P((Xv_window, Event *,
                                   Notify_arg, Notify_event_type)) ;
Notify_value pw_folder_event    P((Xv_Window, Event *,
                                   Notify_arg, Notify_event_type)) ;
Notify_value pw_tree_event      P((Xv_window, Event *,
                                   Notify_arg, Notify_event_type)) ;
Notify_value scroll_event       P((Canvas, Event *,
                                   Notify_arg, Notify_event_type)) ;
Notify_value tree_event         P((Xv_window, Event *,
                                   Notify_arg, Notify_event_type)) ;

Panel_setting goto_line_proc    P((Panel_item, Event *)) ;

Selection_requestor create_seln_requestor P((Event_Context)) ;

Server_image fm_server_image              P((int)) ;
Server_image get_generic_icon_image       P((int, int)) ;
Server_image get_icon_server_image        P((File_Pane_Object *, int)) ;

void activate_pattern               P((Panel_item, int, Event *)) ;
void cancel_comments                P((Panel_item, Event *)) ;
void cancel_rename                  P((Panel_item, Event *)) ;
void cpo_print_proc                 P((Panel_item, Event *)) ;
void create_drop_site               P((int, Xv_Window, int)) ;
void create_frame_drop_site         P((Frame, int)) ;
void destroy_dnd_object             P((Dnd)) ;
void display_folder                 P((Canvas, Xv_Window, Display *,
                                       Window, Xv_xrectlist *)) ;
void do_fm_cc_goto_menu_proc        P((Menu_item, int)) ;
void do_display_folder              P((Canvas, Xv_Window, Display *,
                                       Window, Xv_xrectlist *)) ;
void do_fio_activate_panel_item     P((Panel_item, Event *)) ;
void do_fio_reset_proc              P((Panel_item, Event *)) ;
void do_tree_repaint                P((Canvas, Xv_Window, Display *,
                                        Window, Xv_xrectlist *)) ;
void edit_button                    P((Panel_item, Event *)) ;
void eject_disk_button_proc         P((Panel_item, Event *)) ;
void file_button                    P((Panel_item, Event *)) ;
void find_button                    P((Panel_item, Event *)) ;
void fio_activate_panel_item        P((Panel_item, Event *)) ;
void fio_apply_proc                 P((Panel_item, Event *)) ;
void fio_done_proc                  P((Frame)) ;
void fio_folder_by_content          P((Panel_item, unsigned int, Event *)) ;
void fio_reset_proc                 P((Panel_item, Event *)) ;
void fm_cmd_apply                   P((Panel_item, Event *)) ;
void fm_cmd_reset                   P((Panel_item, Event *)) ;
void fm_create_folder_canvas        P((Frame, register int)) ;
void fm_name_icon                   P((Frame, char *)) ;
void fm_set_colormap                P((Xv_Window)) ;
void goto_button                    P((Panel_item, Event *)) ;
void load_from_dnd                  P((Selection_requestor)) ;
void menu_pressed                   P((Menu, Menu_item)) ;
void more_button                    P((Panel_item, Event *)) ;
void open_button                    P((Panel_item, Event *)) ;
void path_repaint                   P((Canvas, Xv_Window, Display *,
                                        Window, Xv_xrectlist *)) ;
void po_apply_proc                  P((Panel_item, Event *)) ;
void po_cur_display_item_proc       P((Panel_item, int, Event *)) ;
void po_cur_list_item_proc          P((Panel_item, int, Event *)) ;
void po_custom_add                  P((Panel_item, Event *)) ;
void po_new_display_item_proc       P((Panel_item, int, Event *)) ;
void po_new_list_item_proc          P((Panel_item, int, Event *)) ;
void po_goto_add                    P((Panel_item, Event *)) ;
void po_reset_proc                  P((Panel_item, Event *)) ;
void po_show_other_editor           P((Panel_item, int, Event *)) ;
void proceed_button                 P((Panel_item, Event *)) ;
void rcp_cancel_proc                P((Panel_item, Event *)) ;
void really_stop_dnd                P((Selection_requestor, int)) ;
void rename_disk                    P((Panel_item, Event *)) ;
void send_comments                  P((Panel_item, Event *)) ;
void set_icon_font                  P((Frame)) ;
void set_pw_events                  P((int, Xv_Window)) ;
void stop_button                    P((Panel_item, Event *)) ;
void tree_file_button               P((Panel_item, Event *)) ;
void tree_repaint                   P((Canvas, Xv_Window, Display *,
                                        Window, Xv_xrectlist *)) ;
void tree_view_button               P((Panel_item, Event *)) ;
void view_button                    P((Panel_item, Event *)) ;
void waste_empty_button             P((Panel_item, Event *)) ;
void wedit_button                   P((Panel_item, Event *)) ;
void wview_button                   P((Panel_item, Event *)) ;

XRectangle compress_damage          P((Display *, Xv_Window, Xv_xrectlist *)) ;

Xv_Cursor get_drag_cursor           P((int, int, int)) ;
Xv_Cursor get_accept_cursor         P((int, int, int)) ;
Xv_Cursor get_folder_cursor         P((int)) ;
