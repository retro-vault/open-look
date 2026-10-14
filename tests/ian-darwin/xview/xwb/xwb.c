#include <xview/ttysw.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
/***************************************************\
********                                     ********
********  Schlampisoft X-windows Workbench   ********
********                                     ********
********      (c)1995 by Michael Sohmen      ********
********                                     ********
\***************************************************/
#include <dirent.h>

#include <xview/frame.h>
#include <xview/panel.h>
#include <xview/xview.h>
#include <xview/canvas.h>
#include <xview/scrollbar.h>
#include <xview/notice.h>
#include <xview/textsw.h>
#include <xview/tty.h>
#include <xview/cms.h>
#include <xview/xv_xrect.h>
#include <xview/seln.h>

/* used for menu_dir function */
#include <sys/stat.h>
#include <dirent.h>
#include <dirent.h>
/*#include <X11/Xos.h>*/
#ifndef MAXPATHLEN
#include <sys/param.h>
#endif /* MAXPATHLEN */

#define  MENU_KEY         200
#define  NUM_COLORS       16
#define  WINDOW_WIDTH  /*650*/ 900
#define  WINDOW_HEIGHT /*500*/ 700

Frame       frame, addframe, helpframe ;
Panel       panel, addpanel;
Canvas      canvas ;
/*Scrollbar scrollbar ;*/
Menu        filemenu, dir_menu, toolmenu, optmenu, execmenu, helpmenu ;
Frame       myframe ;
Frame       quitframe ;
Panel       mypanel ;
Panel       quitpanel ;
Canvas      mycanvas ;
char        filename[512] ;
char        thisfile[256] ;
Textsw      textsw, helptext;
unsigned long textsw_buf_size;
Tty         tty ;
/*Scrollbar  ttyscrollbar ;*/
Xv_cmsdata    cms_data ;
unsigned char red[NUM_COLORS], green[NUM_COLORS], blue[NUM_COLORS] ;
unsigned long  *colors ;
GC  gc ;
Server_image  chips[NUM_COLORS], icon_image ;
Canvas piccanvas ;
Xv_Server server ;
Textsw_index point ;
char copytext[2048] ;
char *findstring ;
char *replacestring ;
Display *display ;
XID piccanvas_xid ;
XFontStruct *font ;
XGCValues gc_val ;
static char stipple_bits[] = {0xAA, 0xAA, 0x55, 0x55} ;
Textsw_index cursor ;
int compnum=0 ;
void select_conf();
Panel_item opt_text ;
Panel_item helpframe ;
char compilername_buf[16][256] ;
char compiler_buf    [16][4096] ;
char linker_buf      [16][256] ;
char attributes_buf  [16][256] ;
char source_buf      [16][256] ;
char exec_buf        [16][256] ;
char usew_buf        [16][256] ;
char tatt_buf        [16][256] ;
char printer_buf     [16][256] ;
char printformat_buf [16][1024] ;
char printformat_odd_buf  [16][1024] ;
char printformat_even_buf [16][1024] ;
char help_buf        [4][16000] ;
char path_buf [16000] ;
struct dirent *path_pointer[1000] ;
char           xpath[1000];
unsigned long int path_counter ;
void text_subwindow   () ;
void menu_bar         () ;
void message_window   () ;
void short_buttons    () ;
void dummy            () ;
void File_notify_proc () ;
void File_event_proc  () ;

void New_text_frame   () ;
void Actual_text_frame() ;
void Open             () ;
Menu gen_path_menu    () ;
Menu_item add_path_to_menu () ;
int path_type;
Menu_item path_item ;
Menu_item path_menu ;
void Save             () ;
void Save1            () ;
void Save2            () ;
void Save_as          () ;
void Copyright          () ;
void Saved_notify     () ;
void Print            () ;
void Printformat      () ;
void Printformatoddsides  () ;
void Printformatevensides () ;
void Quit             () ;
void Quit1            () ;
void Quit2            () ;
void Close            () ;
void Linenumber       () ;
void Linenumber1      () ;
void Linenumber2      () ;
void Undo             () ;
void Redo             () ;
void Find             () ;
void Find1            () ;
void Find2            () ;
void Findnext         () ;
void Findnext1         () ;
void Findnext2         () ;
int  search_string    () ;
void Replace          () ;
void set_replace_string  () ;
void set_replace_string2 () ;
void replace_string   () ;
void Copy_string      () ;
void Copy_string1      () ;
void Copy_string2      () ;
void Insert_string    () ;
void Insert_string1    () ;
void Insert_string2    () ;
/*void Remove_string    () ;*/
void Shortwin         () ;
void configurate      () ;
void compname_opt     () ;
void comp_opt         () ;
void link_opt         () ;
void attr_opt         () ;
void srce_opt         () ;
void exec_opt         () ;
void usew_opt         () ;
void tatt_opt         () ;
void prnt_opt         () ;
void form_opt         () ;
void form_odd_opt     () ;
void form_even_opt    () ;
int  filebutton  () ;
int  toolbutton  () ;
int  optbutton   () ;
int  execbutton  () ; 
int  helpbutton  () ;          
void Compile      () ;
void Compile1      () ;
void Compile2      () ;
void RunX         () ;
void RunX1         () ;
void RunX2         () ;
void Run          () ;
void Run1          () ;
void Run2          () ;
void Break        () ;
void Break1        () ;
void Break2        () ;
void Helpabout    () ;
void Helpquick    () ;
void Helpdetail   () ;
void Helpinternals() ;

void File_proc    () ;
void piccanvas_repaint () ;
void initcolor    () ;
void initchips    () ;
void choosecolor  () ; 
void cancel_quit  () ;
void ende         () ;
Notify_value notify_resize () ;
int argc_glob ;
char *argv_glob[] ;

int main(int argc, char * argv[])
{

  int   argc_glob = argc ;
  char *argv_glob =*argv ;

  strncpy (thisfile, argv[0], 256) ;

  configurate();
  /* Initialize  base frame */
  xv_init (XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);
  frame = (Frame)xv_create ((int)0, FRAME, 
       FRAME_LABEL,     "Schlampisoft Xwindows Workbench",
       XV_WIDTH,        WINDOW_WIDTH,
       XV_HEIGHT,       WINDOW_HEIGHT,
       0) ;

  /* Initialize Menu-Choosecolor */
  initcolor(&cms_data, red, green, blue);
  /*initchips(panel, chips) ;*/
  
  /* Initialize menu */
  filemenu = (Menu) xv_create((int)0 , MENU,             
       MENU_ITEM,  
         MENU_STRING,         "New",
         MENU_PULLRIGHT,
         xv_create(panel , MENU,
           MENU_ITEM,
             MENU_STRING,        "another frame", 
             MENU_NOTIFY_PROC,    New_text_frame,
             0,
           MENU_ITEM,
             MENU_STRING,        "clear window", 
             MENU_NOTIFY_PROC,    Actual_text_frame,
             0,
           0),
         0,
       MENU_ITEM,
         MENU_STRING,       "Open",
         MENU_GEN_PULLRIGHT, gen_path_menu,
         MENU_CLIENT_DATA,   0,
         0,   
       MENU_ITEM,
         MENU_STRING,        "Save",
         MENU_NOTIFY_PROC,    Save, 
         0,
       MENU_ITEM,
         MENU_STRING,        "Save as", 
         MENU_GEN_PULLRIGHT, gen_path_menu,
         MENU_CLIENT_DATA,    1,
         0,       
       MENU_ITEM,
         MENU_STRING,      "print",
         MENU_NOTIFY_PROC,  Print,
         0,
       MENU_ITEM,
         MENU_STRING,      "formatted print",
         MENU_PULLRIGHT,
         xv_create(panel, MENU,
           MENU_ITEM,
             MENU_STRING,       "print all",
             MENU_NOTIFY_PROC,  Printformat,
             0,
           MENU_ITEM,
             MENU_STRING,       "print odd sides",
             MENU_NOTIFY_PROC,  Printformatoddsides,
             0,
           MENU_ITEM,
             MENU_STRING,       "print even sides",
             MENU_NOTIFY_PROC,  Printformatevensides,
             0,
           0),
         0,
       MENU_ITEM,
         MENU_STRING,      "author",
         MENU_NOTIFY_PROC, Copyright,
         0,
       MENU_ITEM,
         MENU_STRING,      "QUIT",
         MENU_NOTIFY_PROC,  Quit,            
         0,
       0);
  toolmenu = (Menu) xv_create((int)0, MENU,
       MENU_ITEM,
         MENU_STRING,      "Line number",
         MENU_NOTIFY_PROC,  Linenumber,
         0,
 /*      MENU_ITEM,
         MENU_STRING,      "Undo",
         MENU_NOTIFY_PROC,  Undo,
         0,
       MENU_ITEM,
         MENU_STRING,      "Redo",
         MENU_NOTIFY_PROC,  Redo,
         0,*/         
       MENU_ITEM,  
         MENU_STRING,         "find",
         MENU_PULLRIGHT,
         xv_create(panel , MENU,
           MENU_ITEM,
             MENU_STRING,        "search forward", 
             MENU_CLIENT_DATA, 0,
            MENU_NOTIFY_PROC,    Find,
             0,
           MENU_ITEM,
             MENU_STRING,        "search backward", 
             MENU_CLIENT_DATA, 1,
            MENU_NOTIFY_PROC,    Find,
             0,
           0),
         0,
       MENU_ITEM,  
         MENU_STRING,         "find next",
         MENU_PULLRIGHT,
         xv_create(panel , MENU,
           MENU_ITEM,
             MENU_STRING,        "search forward", 
             MENU_CLIENT_DATA, 0,
            MENU_NOTIFY_PROC,    Findnext,
             0,
           MENU_ITEM,
             MENU_STRING,        "search backward", 
             MENU_CLIENT_DATA, 1,
            MENU_NOTIFY_PROC,    Findnext,
             0,
           0),
         0,
       MENU_ITEM,
         MENU_STRING,      "Replace",
         MENU_NOTIFY_PROC,  Replace,
         0,
       MENU_ITEM,
         MENU_STRING,      "Copy string",
         MENU_NOTIFY_PROC,  Copy_string, 
         0,
       MENU_ITEM,
         MENU_STRING,      "Insert string",
         MENU_NOTIFY_PROC,  Insert_string, 
         0,
/*       MENU_ITEM,
         MENU_STRING,      "Remove string",
         MENU_NOTIFY_PROC,  Remove_string, 
         0,*/
       0);
  optmenu = (Menu) xv_create((int)0, MENU,
       MENU_ITEM,
         MENU_STRING,      "Shortcut window",
         MENU_NOTIFY_PROC,  Shortwin, 
         0,
       MENU_ITEM,
         MENU_STRING,      "Choose Configuration",
         MENU_PULLRIGHT,
         xv_create(panel, MENU,
           MENU_ITEM,
             MENU_STRING,       compilername_buf[0],
             MENU_CLIENT_DATA,  0,
             MENU_NOTIFY_PROC,  select_conf,
             0,
           MENU_ITEM,
             MENU_STRING,       compilername_buf[1],
             MENU_CLIENT_DATA,  1,
             MENU_NOTIFY_PROC,  select_conf,
             0,
           MENU_ITEM,
             MENU_STRING,       compilername_buf[2],
             MENU_CLIENT_DATA,  2,
             MENU_NOTIFY_PROC,  select_conf,
             0,
           MENU_ITEM,
             MENU_STRING,       compilername_buf[3],
             MENU_CLIENT_DATA,  3,
             MENU_NOTIFY_PROC,  select_conf,
             0,
           MENU_ITEM,
             MENU_STRING,       compilername_buf[4],
             MENU_CLIENT_DATA,  4,
             MENU_NOTIFY_PROC,  select_conf,
             0,
           MENU_ITEM,
             MENU_STRING,       compilername_buf[5],
             MENU_CLIENT_DATA,  5,
             MENU_NOTIFY_PROC,  select_conf,
             0,
           MENU_ITEM,
             MENU_STRING,       compilername_buf[6],
             MENU_CLIENT_DATA,  6,
             MENU_NOTIFY_PROC,  select_conf,
             0,
           MENU_ITEM,
             MENU_STRING,       compilername_buf[7],
             MENU_CLIENT_DATA,  7,
             MENU_NOTIFY_PROC,  select_conf,
             0,
           MENU_ITEM,
             MENU_STRING,       compilername_buf[8],
             MENU_CLIENT_DATA,  8,
             MENU_NOTIFY_PROC,  select_conf,
             0,
           MENU_ITEM,
             MENU_STRING,       compilername_buf[9],
             MENU_CLIENT_DATA,  9,
             MENU_NOTIFY_PROC,  select_conf,
             0,
           MENU_ITEM,
             MENU_STRING,       compilername_buf[10],
             MENU_CLIENT_DATA,  10,
             MENU_NOTIFY_PROC,  select_conf,
             0,
           MENU_ITEM,
             MENU_STRING,       compilername_buf[11],
             MENU_CLIENT_DATA,  11,
             MENU_NOTIFY_PROC,  select_conf,
             0,
           MENU_ITEM,
             MENU_STRING,       compilername_buf[12],
             MENU_CLIENT_DATA,  12,
             MENU_NOTIFY_PROC,  select_conf,
             0,
           MENU_ITEM,
             MENU_STRING,       compilername_buf[13],
             MENU_CLIENT_DATA,  13,
             MENU_NOTIFY_PROC,  select_conf,
             0,
           MENU_ITEM,
             MENU_STRING,       compilername_buf[14],
             MENU_CLIENT_DATA,  14,
             MENU_NOTIFY_PROC,  select_conf,
             0,
           MENU_ITEM,
             MENU_STRING,       compilername_buf[15],
             MENU_CLIENT_DATA,  15,
             MENU_NOTIFY_PROC,  select_conf,
             0,
           0),
         0,
       MENU_ITEM,
         MENU_STRING,      "Compiler Description",
         MENU_NOTIFY_PROC,  compname_opt,
         0,
       MENU_ITEM,
         MENU_STRING,      "Compiler",
         MENU_NOTIFY_PROC,  comp_opt,
         0,
       MENU_ITEM,
         MENU_STRING,      "Linker options",
         MENU_NOTIFY_PROC,  link_opt,
         0,
       MENU_ITEM,
         MENU_STRING,      "Attributes",
         MENU_NOTIFY_PROC,  attr_opt,
         0,
       MENU_ITEM,
         MENU_STRING,      "Source file format",
         MENU_NOTIFY_PROC,  srce_opt,
         0,
       MENU_ITEM,
         MENU_STRING,      "Target file format",
         MENU_NOTIFY_PROC,  exec_opt,
         0,
       MENU_ITEM,
         MENU_STRING,      "use target file with",
         MENU_NOTIFY_PROC,  usew_opt,
         0,
       MENU_ITEM,
         MENU_STRING,      "target attributes",
         MENU_NOTIFY_PROC,  tatt_opt,
         0,        
       MENU_ITEM,
         MENU_STRING,      "Printer",
         MENU_NOTIFY_PROC,  prnt_opt,
         0,         
       MENU_ITEM,  
         MENU_STRING,         "formatted print",
         MENU_PULLRIGHT,
         xv_create(panel , MENU,
           MENU_ITEM,
             MENU_STRING,        "print all", 
             MENU_NOTIFY_PROC,    form_opt,
             0,
           MENU_ITEM,
             MENU_STRING,        "print odd sides", 
             MENU_NOTIFY_PROC,    form_odd_opt,
             0,
           MENU_ITEM,
             MENU_STRING,        "print even sides", 
             MENU_NOTIFY_PROC,    form_even_opt,
             0,
           0),
         0,
         
       /*MENU_ITEM,
         MENU_STRING,      "Editor Colors",
         MENU_PULLRIGHT,
         xv_create(panel, MENU,
           *MENU_ITEM,
             MENU_STRING,      "Menu Background",
             MENU_PULLRIGHT,
             xv_create(panel, MENU,
               MENU_ITEM,
                 MENU_STRING,                  "white",
                 MENU_NOTIFY_PROC,             sc_white,*
                 0,
             0),*

*

 red[0] = 255 ;    green[0] = 255 ;    blue[0] = 255 ;    white      
  red[1] = 255 ;    green[1] =  0  ;    blue[1] =  0  ;    red        
  red[2] =  0  ;    green[2] = 255 ;    blue[2] =  0  ;    green      
  red[3] =  0  ;    green[3] =  0  ;    blue[3] = 255 ;    blue       
  red[4] = 255 ;    green[4] = 255 ;    blue[4] =  0  ;    yellow     
  red[5] = 188 ;    green[5] = 143 ;    blue[5] = 143 ;    brown      
  red[6] = 220 ;    green[6] = 220 ;    blue[6] = 220 ;    gray       
  red[7] =  0  ;    green[7] =  0  ;    blue[7] =  0  ;    black      
  red[8] = 255 ;    green[8] = 128 ;    blue[8] =  0  ;    orange     
  red[9] =  0  ;    green[9] = 128 ;    blue[9] =  0  ;    mid_green  
  red[10] = 128 ;   green[10] =  0  ;   blue[10] =  0  ;   mid_red    
  red[11] =  0  ;   green[11] =  0  ;   blue[11] = 128 ;   mid_blue   
  red[12] = 96  ;   green[12] =  0  ;   blue[12] = 128  ;  violet    
  red[13] =  0  ;   green[13] = 64  ;   blue[13] =  0  ;   dark_green 
  red[14] = 64  ;   green[14] =  0  ;   blue[14] =  0  ;   dark_red   
  red[15] =  0  ;   green[15] =  0  ;   blue[15] = 64  ;   dark_blue  


*
           MENU_ITEM,
             MENU_STRING,      "Button",
            * PANEL_CHOICE_IMAGES,
               chips[0], chips[1], chips[2], chips[3], chips[4], chips[5], chips[6], chips[7], 0,*
             MENU_NOTIFY_PROC,  choosecolor,
             0,
           MENU_ITEM,
             MENU_STRING,      "Button Text",
             MENU_NOTIFY_PROC,  choosecolor,
             0,
           MENU_ITEM,
             MENU_STRING,      "Editor Background",
             MENU_NOTIFY_PROC,  choosecolor,
             0,
            MENU_ITEM,
             MENU_STRING,      "Editor Text",
             MENU_NOTIFY_PROC,  choosecolor,
             0,
           MENU_ITEM,
             MENU_STRING,      "Message Background",
             MENU_NOTIFY_PROC,  choosecolor,
             0,
           MENU_ITEM,
             MENU_STRING,      "Message Text",
             MENU_NOTIFY_PROC,  choosecolor,
             0,
           0),*/
       0);
  execmenu = (Menu) xv_create((int)0, MENU,
       MENU_ITEM,
         MENU_STRING,      "Compile actual file",
         MENU_NOTIFY_PROC,  Compile,
         0,
       MENU_ITEM,
         MENU_STRING,      "Run Xwin application",
         MENU_NOTIFY_PROC,  RunX,
         0,
       MENU_ITEM,
         MENU_STRING,      "Run in foreground",
         MENU_NOTIFY_PROC,  Run, 
         0,
       MENU_ITEM,
         MENU_STRING,      "Break execution",
         MENU_NOTIFY_PROC,  Break,
         0,
       0);
  helpmenu = (Menu) xv_create((int)0, MENU,
       MENU_ITEM,
         MENU_STRING,      "About",
         MENU_NOTIFY_PROC,  Helpabout,
         0,
       MENU_ITEM,
         MENU_STRING,      "Quick Introduction",
         MENU_NOTIFY_PROC, Helpquick ,
         0,
       MENU_ITEM,
         MENU_STRING,      "Summary",
         MENU_NOTIFY_PROC,  Helpdetail, 
         0,
       MENU_ITEM,
         MENU_STRING,      "Internals",
         MENU_NOTIFY_PROC,  Helpinternals,
         0,
      0);

  /* Initialize menu buttons */
  panel = (Panel)xv_create (frame, PANEL, 
       XV_X,                     0,
       XV_Y,                     0,
       /*XV_WIDTH,                 (frame, XV_WIDTH),*/
       /*XV_HEIGHT,                25,*/
        WIN_ROWS,               1,

       0);
  (void)xv_create (panel, PANEL_BUTTON,
       PANEL_LABEL_STRING,       "File",
       PANEL_ITEM_COLOR,         6,
       PANEL_ITEM_MENU,          filemenu,
       0);
  (void)xv_create (panel, PANEL_BUTTON,
       PANEL_LABEL_STRING,       "Tools",
       PANEL_ITEM_COLOR,         7,
       PANEL_ITEM_MENU,          toolmenu,
       0);
  (void)xv_create (panel, PANEL_BUTTON,
       PANEL_LABEL_STRING,       "Options",
       PANEL_ITEM_COLOR,         8,
       PANEL_ITEM_MENU,          optmenu,
       0);
  (void)xv_create (panel, PANEL_BUTTON,
       PANEL_LABEL_STRING,       "Execute",
       PANEL_ITEM_COLOR,         9,
       PANEL_ITEM_MENU,          execmenu,
       0);
  (void)xv_create (panel, PANEL_BUTTON,
       PANEL_LABEL_STRING,       "Help",   
       PANEL_ITEM_COLOR,         10,
       PANEL_ITEM_MENU,          helpmenu,
       0);

  /* Canvas: root for Editor */
  canvas = (Canvas)xv_create (frame, CANVAS,
       CANVAS_AUTO_SHRINK,   TRUE,
       CANVAS_AUTO_EXPAND,   TRUE,
       XV_X,            0,
       /*XV_Y,            25,*/
       WIN_BELOW,       panel,
       XV_WIDTH,        xv_get(frame, XV_WIDTH),
       XV_HEIGHT,       xv_get(frame, XV_HEIGHT)-25,
       0);
  
  /* Text-subwindow */
  textsw=(Textsw)xv_create(canvas,TEXTSW,
       TEXTSW_MEMORY_MAXIMUM,     textsw_buf_size,
       TEXTSW_AUTO_INDENT,        TRUE,
       XV_X,            0,
       XV_Y,            0,
       XV_WIDTH,        xv_get(canvas, XV_WIDTH),
       XV_HEIGHT,      (xv_get(canvas, XV_HEIGHT)/5)*4,
       WIN_FOREGROUND_COLOR, 2,
       0);

      /*xv_set(textsw,
        WIN_CONSUME_EVENTS,     WIN_MOUSE_BUTTONS, 0,
        WIN_EVENT_PROC,         my_event_proc,
        WIN_CLIENT_DATA,        menu,
        0);*/

     
/* Picture-Subwindow */
  piccanvas = (Canvas) xv_create (canvas, CANVAS,
       CANVAS_REPAINT_PROC,   piccanvas_repaint,
       CANVAS_X_PAINT_WINDOW, TRUE,
       WIN_DYNAMIC_VISUAL,    FALSE,
       XV_X,              0,
       /*XV_Y,              xv_get(textsw, XV_HEIGHT),*/
        WIN_BELOW,              textsw,
      XV_WIDTH,          (215*xv_get(canvas, XV_HEIGHT))/700,
       XV_HEIGHT,         xv_get(canvas, XV_HEIGHT)-xv_get(textsw, XV_HEIGHT),
       WIN_CMS_NAME,      "palette",
       WIN_CMS_DATA,      &cms_data,
       0);

  /* Tty-Subwindow */
  tty = (Tty)xv_create (canvas, TTY,
       XV_X,              xv_get(piccanvas, XV_WIDTH),
       /*XV_Y,              xv_get(textsw, XV_HEIGHT),*/
       WIN_BELOW,              textsw,
       XV_WIDTH,        xv_get(canvas, XV_WIDTH)-xv_get(piccanvas, XV_WIDTH), 
       XV_HEIGHT,       xv_get(canvas, XV_HEIGHT)-xv_get(textsw, XV_HEIGHT),
       0); 
  
      window_fit(frame);

  
  /* Required Configuration for graphic Display */ 
  display = (Display *)xv_get (frame, XV_DISPLAY);
  piccanvas_xid     = (XID)      xv_get (canvas_paint_window(canvas), XV_XID);
  if(!(font = XLoadQueryFont(display, "fixed")))  { printf("cannot find fixed font\n"); exit(1); }
  gc_val.font = font->fid;
  gc_val.stipple = XCreateBitmapFromData(display, piccanvas_xid, stipple_bits, 16, 2);
  gc = XCreateGC(display, piccanvas_xid, GCFont | GCStipple, &gc_val);
  colors = (unsigned long *)xv_get (piccanvas, WIN_X_COLOR_INDICES);
  piccanvas_xid = (XID)xv_get (canvas_paint_window(frame), XV_XID); 
  

  
  
  window_fit(canvas);
  /*ttysw_output(tty, *message, strlen(*message) );*/  
  xv_main_loop (frame) ;
  exit(0);
}

void     
New_text_frame(Menu menu, Menu_item menu_item)
{
  ttysw_input (tty, thisfile, strlen(thisfile));
  ttysw_input (tty, " &\n", strlen(" &\n"));
}

void     
Actual_text_frame(Menu menu, Menu_item menu_item)
{
  int result;
  Panel mypanel;
  long int end = (Textsw_index)xv_get (textsw, TEXTSW_LENGTH)-1;
  mypanel = (Panel)xv_get ( panel , PANEL_PARENT_PANEL);
  result = notice_prompt(mypanel, 0,
      NOTICE_MESSAGE_STRINGS, "REALLY CLEAR TEXT WINDOW?", 0,
      NOTICE_BUTTON_YES,      "Do it",
      NOTICE_BUTTON_NO,       "cancel",
      0);
  if (result==NOTICE_YES)
  {
    Saved_notify();
    textsw_erase(textsw, 0, end);
    xv_set(textsw, TEXTSW_FILE_CONTENTS, "", TEXTSW_FIRST, 0, 0);
  }
}

/* the following functions are for the 'Open' file path-pullright routine */

char *
getfilename(char * path)
{
    char *p;
    if (p = rindex(path, '/'))
        p++;
    else
        p = path;
    return strcpy(malloc(strlen(p)+1), p);
}

Menu
gen_pullright(Menu_item mi, Menu_generate op)
{
    Menu menu;
    Menu_item new, old = mi;
    char buf[MAXPATHLEN];

    if (op == MENU_DISPLAY) {
        menu = (Menu)xv_get(mi, MENU_PARENT);
        sprintf(buf, "%s/%s",
            (char *)xv_get(menu, MENU_CLIENT_DATA), (char *)xv_get(mi, MENU_STRING));
        /* get old menu and free it -- we're going to build another */
        if (menu = (Menu)xv_get(mi, MENU_PULLRIGHT)) {
            free((char *)xv_get(menu, MENU_CLIENT_DATA));
            xv_destroy(menu);
        }  
        if (new = add_path_to_menu(buf)) {
            menu = (Menu)xv_get(new, MENU_PULLRIGHT);
            xv_destroy(new);
            return menu;
        }
    }
    if (!(menu = (Menu)xv_get(mi, MENU_PULLRIGHT)))
            menu = (Menu)xv_create((int)0, MENU,
                MENU_STRINGS, "Couldn't build a menu.", 0,
                0);
    return menu;
}


Menu_item
add_path_to_menu(char * path)
{
  DIR            *dirp;
  struct dirent  *dp;
  struct stat    s_buf;
  Menu_item      mi;
  Menu           next_menu;
  char           buf[MAXPATHLEN];
  static int     recursion;
                  
  /* don't add a folder to the list if user can't read it */
  if (stat(path, &s_buf) == -1 || !(s_buf.st_mode & S_IREAD))
      return (int)0;
  if (s_buf.st_mode & S_IFDIR) {
      int cnt = 0;
      if (!(dirp = opendir(path)))
          /* don't bother adding to list if we can't scan it */
          return (int)0;
      if (recursion)
          return (Menu_item)-1;
      recursion++;
      next_menu = (Menu)xv_create(XV_NULL, MENU, 0);
      
      xv_set(next_menu,
          MENU_ITEM,
          MENU_STRING,        "..",
          MENU_GEN_PULLRIGHT, gen_pullright,
          MENU_CLIENT_DATA, strcpy(malloc(strlen(path)+1), path), /* wegen free-funktion */
          /*0,*/
        0);
      if (path_type==1)
      xv_set(next_menu,
          MENU_ITEM,
          MENU_NOTIFY_PROC,     Save_as,
          MENU_STRING,          source_buf[compnum],
          MENU_CLIENT_DATA,     strcpy(malloc(strlen(path)+1), path),
       0);
                                  
         /* unreadable file or dir - deactivate item */
          while (dp = readdir(dirp))
            if (strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..")) {
                (void) sprintf(buf, "%s/%s", path, dp->d_name);
                mi = add_path_to_menu(buf);
                if (!mi || mi == (Menu_item)-1) {
                    int do_gen_pullright = (mi == (Menu_item)-1);
                    /* unreadable file or dir - deactivate item */
                    mi = (Menu_item)xv_create(XV_NULL, MENUITEM,
                        MENU_STRING,  getfilename(dp->d_name),
                        MENU_RELEASE,
                        MENU_RELEASE_IMAGE,
                    0);
                    if (path_type==1)
                    {
                      sprintf(filename, "/%s/", path /*xv_get(mi, MENU_STRING)*/),
                      xv_set(mi,
                          MENU_NOTIFY_PROC,     Save_as,
                          /*MENU_CLIENT_DATA,     filename,*/
                          MENU_CLIENT_DATA,     strcpy(malloc(strlen(path)+1), path),
                      0);
                    }
                    if (do_gen_pullright)
                        xv_set(mi,
                            MENU_GEN_PULLRIGHT, gen_pullright,
                            MENU_CLIENT_DATA, strcpy(malloc(strlen(path)+1), path), /* wegen free-funktion */
                        0);
                    else
                        xv_set(mi, MENU_INACTIVE, TRUE, 0);
                }
                xv_set(next_menu, MENU_APPEND_ITEM, mi, 0);
                cnt++;
            }            
            
            
        closedir(dirp);
        mi = xv_create(XV_NULL, MENUITEM,
            MENU_STRING,        getfilename(path),
            MENU_RELEASE,
            MENU_RELEASE_IMAGE,
            /*MENU_NOTIFY_PROC,   Open,
            MENU_CLIENT_DATA, strcpy(malloc(strlen(path)+1), path),*/
          0);
        if (path_type==1)
          xv_set(mi,
              MENU_CLIENT_DATA, strncpy(malloc(strlen(path)+1), path, strlen(path)/*-strlen(getfilename(path))-1*/),
              MENU_NOTIFY_PROC,   Save_as,
          0);
        else
          xv_set(mi,
            MENU_NOTIFY_PROC,   Open,
            MENU_CLIENT_DATA, strcpy(malloc(strlen(path)+1), path),
          0);
      if (!cnt) {
            /*xv_destroy(next_menu);*/
            /* An empty or unsearchable directory - deactivate item */
            xv_set(mi, MENU_INACTIVE, TRUE, 0);
        } else {
              if(strlen(path)>16)
             xv_set(next_menu,
                MENU_TITLE_ITEM, strncpy(malloc(strlen(path)+1), path+strlen(path)-8, 8),
                MENU_CLIENT_DATA, strcpy(malloc(strlen(path)+1), path),
                0);
             else
             xv_set(next_menu,
                MENU_TITLE_ITEM, strcpy(malloc(strlen(path)+1), path),
                MENU_CLIENT_DATA, strcpy(malloc(strlen(path)+1), path),
                0);
                
            xv_set(mi, MENU_PULLRIGHT, next_menu, 0);
        }
        recursion--;
        return mi;
  }
  strcpy(xpath, path);
  xpath[strlen(xpath)-strlen(rindex(path, '/')+1)-1]='\0';
  if (path_type==0)
    return (Menu_item)xv_create((int)0, MENUITEM,
        MENU_STRING,            getfilename(path),
        MENU_RELEASE,
        MENU_RELEASE_IMAGE,
        MENU_NOTIFY_PROC,       Open,
        MENU_CLIENT_DATA, strcpy(malloc(strlen(path)+1), path),
        0);
  else
    return (Menu_item)xv_create((int)0, MENUITEM,
        MENU_STRING,            getfilename(path),
        MENU_RELEASE,
        MENU_RELEASE_IMAGE,
        MENU_NOTIFY_PROC,   Save_as,
        MENU_CLIENT_DATA,   xpath,/*strncpy(malloc(strlen(path)+1), path,
                            strlen(path)),*/
        0);
}

Menu 
gen_path_menu(Menu_item menu_item, Menu_generate op)
{
  if (opt_text>0)  { xv_destroy_safe(opt_text); opt_text=0; /*return;*/ }
  path_type=xv_get(menu_item, MENU_CLIENT_DATA);
  if (op==MENU_DISPLAY)
  {
    path_item = (Menu_item)xv_create (panel, MENUITEM, MENU_STRING, "xxxx", 0);
    return (Menu)xv_get (add_path_to_menu(argc_glob > 1 ? argv_glob[1] : "."), MENU_PULLRIGHT);
  } else return (Menu)0;
}
 
/*int x;*/

void
Open(Menu menu, Menu_item menu_item)
{
  char *tempstr;
  int i=0;
  tempstr=malloc(sizeof(compilername_buf[compnum])+sizeof(filename)+1);
  sprintf(filename, "%s", xv_get(menu_item, MENU_CLIENT_DATA));
  xv_set(textsw, TEXTSW_FILE, filename, 0);
  if (strlen(filename)>strlen(source_buf[compnum]))
    if (strcmp(source_buf[compnum], filename+strlen(filename)-strlen(source_buf[compnum])))
      while (strlen(filename)>strlen(source_buf[i]) && i<16)
      {
        if (strcmp(source_buf[i], filename+strlen(filename)-strlen(source_buf[i])))
          i++;
        else
        {
          if (i<16) compnum=i;
          i=16;
        }
      }
  sprintf(tempstr,"%s: <%s>",compilername_buf[compnum],filename);
  xv_set(frame, FRAME_LABEL, tempstr );
  free(tempstr);
}

void 
File_proc(Menu menu, Menu_item menu_item)
{
  printf("Menu Item: %s\n", xv_get(menu_item, MENU_STRING));
/*  if (!strcmp((char*)xv_get(menu_item, MENU_STRING), "QUIT"))
     exit(0);*/
}

int getedittext(void)
{
  filename[0]='\0';
  return(textsw_append_file_name(textsw, filename) );
}

void 
Save(Menu menu, Menu_item menu_item)
{
  Save2();
}

void 
Save1(Panel_item item)
{
  Save2();
}

void 
Save2(void)
{
  Panel mypanel;
  mypanel = (Panel)xv_get ( panel , PANEL_PARENT_PANEL);
  if((int)xv_get (textsw, TEXTSW_MODIFIED) == FALSE)
    notice_prompt(mypanel, 0,
        NOTICE_MESSAGE_STRINGS, "Text has not been modified,  or", "no file name specified", 0,
        NOTICE_BUTTON_YES,      "Acknowledged",
        0);
    else
  if (getedittext()==0)
    textsw_store_file(textsw, filename, 0, 0);
}



void 
Save_as(Menu menu, Menu_item menu_item)
{
  int do_save();
  sprintf(filename, "%s/%s", xv_get(menu_item, MENU_CLIENT_DATA), xv_get(menu_item, MENU_STRING));
  opt_text = (Panel_item)xv_create (panel, PANEL_TEXT,
       /*XV_X,                       400,*/
       XV_Y,                       8,      
       PANEL_VALUE,                filename,
       PANEL_LABEL_STRING,         "Save file as ",
       PANEL_NOTIFY_PROC,          do_save,
       PANEL_LAYOUT,               PANEL_HORIZONTAL,
       PANEL_VALUE_DISPLAY_LENGTH, 30,
       PANEL_VALUE_STORED_LENGTH,  256,            
       0);
}

int
do_save(Panel_item item, Event * event)
{
  char *tempstr;
  int i=0;
  (void)strncpy(filename, (char *)xv_get(item, PANEL_VALUE), sizeof filename );
  if (filename!="")
  {
    textsw_store_file(textsw, filename, 0, 0);
  }
  xv_destroy_safe(opt_text);
  opt_text=0;
  if (strlen(filename)>strlen(source_buf[compnum]) && filename!="")
    if (strcmp(source_buf[compnum], filename+strlen(filename)-strlen(source_buf[compnum])))
      while (strlen(filename)>strlen(source_buf[i]) && i<16)
      {
        if (strcmp(source_buf[i], filename+strlen(filename)-strlen(source_buf[i])))
          i++;
        else
        {
          if (i<16) compnum=i;
          i=16;
        }
      }
  tempstr=malloc(sizeof(compilername_buf[compnum])+sizeof(filename)+1);
  sprintf(tempstr,"%s: <%s>",compilername_buf[compnum],filename);
  xv_set(frame, FRAME_LABEL, tempstr );
  free(tempstr);
}

void 
Saved_notify()
{ /*Panel panel;*/
  int result;
  Panel mypanel;
  if (getedittext()!=0) return;
  if((int)xv_get (textsw, TEXTSW_MODIFIED) != FALSE)
  {  
    mypanel = (Panel)xv_get (panel , PANEL_PARENT_PANEL);
    result = notice_prompt(mypanel, 0,
      NOTICE_MESSAGE_STRINGS, "text modified, save?", 0,
      NOTICE_BUTTON_YES,      "SAVE",
      NOTICE_BUTTON_NO,       "Don't save",
      0);
  if (result==NOTICE_YES)
      textsw_store_file(textsw, filename, 0, 0);
  }
}

void
Copyright(void)
{ 
  int result;
  Panel mypanel = (Panel) xv_get (panel, PANEL_PARENT_PANEL);
  result = notice_prompt(mypanel, 0,
      NOTICE_MESSAGE_STRINGS, "       --------------------------       ",
                              "Schlampisoft",
                              "XWindows-Workbench",
                              "Version 1.0",
                              "--------------------------",
                              "",
                              "(c)1995 by Michael Sohmen",
                              "uk6i@ibm3090.rz.uni-karlsruhe.de",
                              "or:   uk6i@dkauni2.bitnet       ",
                              "See Help, About for details",
                              "",
      0,
      NOTICE_BUTTON_YES,      " Ok ",
      0);
  if (result==NOTICE_NO)
    Copyright;
}
  

void     
Print(Menu menu, Menu_item menu_item)
{
  int result;
  Panel mypanel;
  mypanel = (Panel)xv_get ( panel , PANEL_PARENT_PANEL);
  Saved_notify();
  result = notice_prompt(mypanel, 0,
      NOTICE_MESSAGE_STRINGS, "Print file?", 0,
      NOTICE_BUTTON_YES,      "GO!",
      NOTICE_BUTTON_NO,       "cancel",
      0);
  if (result==NOTICE_YES)
  {
    ttysw_input (tty, "cat ", strlen("cat "));
    ttysw_input (tty, filename, strlen(filename));
    ttysw_input (tty, " > ", strlen(" > "));
    ttysw_input (tty, printer_buf[compnum], strlen(printer_buf[compnum]));
    ttysw_input (tty, "& \n", strlen("& \n"));
  }
}
 
void 
Printformat_function(char *xx)
{  
  int i, j, result;
  Panel mypanel;
  char temp[4096];
  mypanel = (Panel)xv_get ( panel , PANEL_PARENT_PANEL);
  Saved_notify();
  /*result = notice_prompt(mypanel, 0,
      NOTICE_MESSAGE_STRINGS,  "Print formatted output?", 0,
      NOTICE_BUTTON_YES,      "GO!",
      NOTICE_BUTTON_NO,       "cancel",
      0);*/
  /*if (result==NOTICE_YES)*/
  for(i=j=0;xx[i]!='\0' && j<4096; i++)
    if ((int)xx[i]!=92 && xx[i]!='*')
      temp[j++]=xx[i];
    else
      if ((int)xx[i]==92 && xx[i+1]=='n')
      { 
        temp[j++]='\n';
        i++;
      }
      else
        if (xx[i]=='*')
        { 
          strncpy(temp+j, (char *)filename, strlen(filename)-strlen(source_buf[compnum]));
          j+=strlen(filename)-strlen(source_buf[compnum]);
        }
  temp[j++]='\0';
  ttysw_input (tty, temp, strlen(temp)+1);
}

void     
Printformat(Menu menu, Menu_item menu_item)
{
  Printformat_function(strcpy (malloc(sizeof(printformat_buf[compnum])+1),printformat_buf[compnum])); 
}

void     
Printformatoddsides(Menu menu, Menu_item menu_item)
{
  Printformat_function(strcpy (malloc(sizeof(printformat_odd_buf[compnum])+1),printformat_odd_buf[compnum])); 
}

void     
Printformatevensides(Menu menu, Menu_item menu_item)
{
  Printformat_function(strcpy (malloc(sizeof(printformat_even_buf[compnum])+1),printformat_even_buf[compnum])); 
}

void 
Quit(Menu menu, Menu_item menu_item)
{
  Quit2();
}

void 
Quit1(Panel_item item)
{
  Quit2();
}

void 
Quit2(void)
{
  int result;
  Panel mypanel;
  mypanel = (Panel)xv_get ( panel , PANEL_PARENT_PANEL);
  result = notice_prompt(mypanel, 0,
      NOTICE_MESSAGE_STRINGS, "REALLY QUIT?", 0,
      NOTICE_BUTTON_YES,      "Quit",
      NOTICE_BUTTON_NO,       "cancel",
      0);
  if (result==NOTICE_YES)
  {
    Saved_notify();
    exit(0);
  }
}

void 
Linenumber(Menu menu, Menu_item menu_item)
{
  Linenumber2();
}

void 
Linenumber1(Panel_item item)
{
  Linenumber2();
}

void 
Linenumber2(void)
{
  int set_line_number();
  Panel_item item;
  if (opt_text>0)  { xv_destroy_safe(opt_text); opt_text=0; return; }
  opt_text = (Panel_item)xv_create (panel, PANEL_TEXT,
      /*XV_X,                       400,*/
      XV_Y,                       8,
      PANEL_LABEL_STRING,        "Select line number ",      
      PANEL_LAYOUT,               PANEL_HORIZONTAL,
      PANEL_NOTIFY_PROC,          set_line_number,
      PANEL_VALUE_DISPLAY_LENGTH, 8,
      PANEL_VALUE_STORED_LENGTH,  256,      
      0);
}

int
set_line_number(Panel_item item, Event * event)
{
  char number[10];
  long num;
  long i,j=0,k=0;
  char buf[2];
  long int end = (Textsw_index)xv_get (textsw, TEXTSW_LENGTH)-1;
  (void)strncpy(number, (char *)xv_get(item, PANEL_VALUE), sizeof(number) );
  num = atol(number)-1;
  if (num<0) {xv_destroy_safe(opt_text); return 1;}
  i=num;
  /*if (i-3>-1) i-=3;
  while (i>0)
  {
    if ((Textsw_index)xv_get (textsw, TEXTSW_CONTENTS, j, buf, 1)<1 || j>end) {xv_destroy_safe(opt_text); return 1;}
      j++;
    if(buf[0]=='\n') i--;
  }
  if (num-3>-1) i=2;*/
  while (i>0)
  {
    if ((Textsw_index)xv_get (textsw, TEXTSW_CONTENTS, j, buf, 1)<1 || j>end) {xv_destroy_safe(opt_text); return 1;}
      j++;
    if(buf[0]=='\n') i--;
  }
  textsw_possibly_normalize(textsw, j);
  buf[0]=' ';
   while (buf[0]!='\n')
  {
    if ((Textsw_index)xv_get (textsw, TEXTSW_CONTENTS, j+k, buf, 1)<1 || j+k>end) {xv_destroy_safe(opt_text); return 1;}
      k++;
  } 
  textsw_set_selection(textsw, j, j+k, 1);
  xv_destroy_safe(opt_text);
  opt_text=0;
}


void 
Undo(Menu menu, Menu_item menu_item)
{
    textsw_reset(textsw, 0, 0);/*locx, locy);*/
}

void 
Redo(Menu menu, Menu_item menu_item)
{

}

void 
Find(Menu menu, Menu_item menu_item)
{
  Find2(xv_get(menu_item, MENU_CLIENT_DATA));
}

void 
Find1(Panel_item item)
{
  Find2(xv_get(item, PANEL_CLIENT_DATA));
}

void 
Find2(int direction)
{
  int set_find_string();
  Panel_item item;
  Seln_holder   holder;
  Seln_request *response;
  Textsw_index        first, last;
  register char       *ptr;
  point = (Textsw_index)xv_get (textsw, TEXTSW_INSERTION_POINT);
  if (opt_text>0)  { xv_destroy_safe(opt_text); opt_text=0; return; }
  server = (Xv_Server)xv_get ( xv_get(frame, XV_SCREEN), SCREEN_SERVER );
  holder = selection_inquire(server, SELN_PRIMARY);
  response = selection_ask(server, &holder,
      SELN_REQ_FIRST,             0,
      SELN_REQ_LAST,              0,
      SELN_REQ_CONTENTS_ASCII,    0,
      0);
  
  ptr = response->data;
  first = *(Textsw_index *)(ptr += sizeof(SELN_REQ_FIRST));
  ptr += sizeof(Textsw_index); 
  last  = *(Textsw_index *)(ptr += sizeof(SELN_REQ_LAST));
  ptr += sizeof(Textsw_index); 
  ptr += sizeof(SELN_REQ_CONTENTS_ASCII);
  findstring=malloc(strlen(ptr)+1); 
  strcpy(findstring, ptr);
  if (strlen(findstring)>0) 
  {
    if (direction==0)
    point = last+1; else point = first;
    search_string(direction);
  }
  else  
  {
    opt_text = (Panel_item)xv_create (panel, PANEL_TEXT,
        /*XV_X,                       400,*/
        XV_Y,                       8,
        PANEL_LABEL_STRING,        "Search for -> ",      
        PANEL_LAYOUT,               PANEL_HORIZONTAL,
        PANEL_NOTIFY_PROC,          set_find_string,
        PANEL_CLIENT_DATA,           direction,
        PANEL_VALUE_DISPLAY_LENGTH, 30,
        PANEL_VALUE_STORED_LENGTH,  256,
      0);
  }
}



int
set_find_string(Panel_item item, Event * event)
{
  findstring=malloc(sizeof(xv_get(item, PANEL_VALUE))+1);
  (void) sprintf(findstring, "%s", xv_get(item, PANEL_VALUE));
  if (search_string(xv_get(item, PANEL_CLIENT_DATA))>0)
  {
    xv_destroy_safe(opt_text);
    opt_text=0;
  }
}

void 
Findnext(Menu menu, Menu_item menu_item)
{
  Findnext2(xv_get(menu_item, MENU_CLIENT_DATA));
}

void 
Findnext1(Panel_item item)
{
  Findnext2(xv_get(item, PANEL_CLIENT_DATA));
}


void 
Findnext2(int direction)
{
  point = (Textsw_index)xv_get (textsw, TEXTSW_INSERTION_POINT);
  (void)search_string(direction);
}

int
search_string(int client)
{
  Textsw_index where;
  Textsw_index *first = malloc(sizeof(Textsw_index));
  Textsw_index *lastplusone = malloc(sizeof(Textsw_index));
  *first = point;
  *lastplusone = (Textsw_index)xv_get (textsw, TEXTSW_LENGTH)-1;
  where = textsw_find_bytes(textsw, first, lastplusone, findstring, strlen(findstring), client);
  if (where<0)
  {
    *first = 0;
    *lastplusone = point;
    where = textsw_find_bytes(textsw, first, lastplusone, findstring, strlen(findstring), 0);
  } 
  if (where>-1) 
  {  
    textsw_possibly_normalize(textsw, where);
    textsw_set_selection(textsw, where, where+strlen(findstring), 1);
    if  (client==0)
      xv_set(textsw, TEXTSW_INSERTION_POINT, *lastplusone, 0);
    else
      xv_set(textsw, TEXTSW_INSERTION_POINT, *first, 0);
    return 1;
  }
  return 0;
}


void 
Replace(Menu menu, Menu_item menu_item)
{
  int set_find_string();
  Panel_item item;
  Seln_holder   holder;
  Seln_request *response;
  Textsw_index        first, last;
  register char       *ptr;
  point = (Textsw_index)xv_get (textsw, TEXTSW_INSERTION_POINT);
  if (opt_text>0)  { xv_destroy_safe(opt_text); opt_text=0; return; }
  server = (Xv_Server)xv_get ( xv_get(frame, XV_SCREEN), SCREEN_SERVER );
  holder = selection_inquire(server, SELN_PRIMARY);
  response = selection_ask(server, &holder,
      SELN_REQ_FIRST,             0,
      SELN_REQ_LAST,              0,
      SELN_REQ_CONTENTS_ASCII,    0,
      0);
      
  ptr = response->data;
  first = *(Textsw_index *)(ptr += sizeof(SELN_REQ_FIRST));
  ptr += sizeof(Textsw_index); 
  last  = *(Textsw_index *)(ptr += sizeof(SELN_REQ_LAST));
  ptr += sizeof(Textsw_index); 
  ptr += sizeof(SELN_REQ_CONTENTS_ASCII);
  findstring=malloc(strlen(ptr)+1); 
  strcpy(findstring, ptr);
  if (strlen(findstring)>0) 
  {
    point = first;
    set_replace_string2(); 
  }
  else  
  {
    /*free(findstring);*/
    opt_text = (Panel_item)xv_create (panel, PANEL_TEXT,
        /*XV_X,                       400,*/
        XV_Y,                       8,
        PANEL_LABEL_STRING,        "Replace this -> ",      
        PANEL_LAYOUT,               PANEL_HORIZONTAL,
        PANEL_NOTIFY_PROC,          set_replace_string,
        PANEL_VALUE_DISPLAY_LENGTH, 30,
        PANEL_VALUE_STORED_LENGTH,  256,
      0);
  }
}

void
set_replace_string(Panel_item item, Event * event)
{
  /*free(findstring);*/
  findstring=malloc(sizeof(xv_get(item, PANEL_VALUE))+1);
  (void) sprintf(findstring, "%s", xv_get(item, PANEL_VALUE));
  set_replace_string2();
}

void
set_replace_string2(void)
{
  xv_destroy_safe(opt_text); 
  opt_text=0;
  if (search_string(0)==1)
    opt_text = (Panel_item)xv_create (panel, PANEL_TEXT,
       /*XV_X,                       ,*/
       XV_Y,                       8,
       PANEL_LABEL_STRING,        "Replace with -> ",      
       PANEL_LAYOUT,               PANEL_HORIZONTAL,
       PANEL_NOTIFY_PROC,          replace_string,
       PANEL_VALUE_DISPLAY_LENGTH, 30,
       PANEL_VALUE_STORED_LENGTH,  256,
     0);
}

void
replace_string(Panel_item item, Event * event)
{
  int result;
  int prompt;
  Textsw_index where, begin, end ;
  Panel mypanel;
  Textsw_index *first = malloc(sizeof(Textsw_index)+1);
  Textsw_index *lastplusone = malloc(sizeof(Textsw_index)+1);
  replacestring=malloc(sizeof(xv_get(item, PANEL_VALUE))+20);
  (void) sprintf(replacestring, "%s", xv_get(item, PANEL_VALUE));
          /*printf("len[%d,%d]",sizeof(xv_get(item, PANEL_VALUE)),strlen(xv_get(item, PANEL_VALUE)));*/
  mypanel = (Panel)xv_get ( panel , PANEL_PARENT_PANEL);
  result = notice_prompt(mypanel, 0,
      NOTICE_MESSAGE_STRINGS, "PROMPT ON REPLACE?", 0,
      NOTICE_BUTTON_YES,      "prompt",
      NOTICE_BUTTON_NO,       "replace all",
      NOTICE_BUTTON,          "cancel", 101,
      0);
  xv_destroy_safe(opt_text); 
  opt_text=0; 
  if (result==NOTICE_YES)
  {
    prompt = 1;
  }
  else 
    if (result==NOTICE_NO) 
      {
        prompt = 0;
      }
    else 
      { free(findstring); free(replacestring); return;}
  *first = begin = point;
  *lastplusone = end = (Textsw_index)xv_get (textsw, TEXTSW_LENGTH)-1;
  while(prompt!=2)
  {
    *first = begin;
    *lastplusone = end;
    where = textsw_find_bytes(textsw, first, lastplusone, findstring, strlen(findstring), 0);
    if (where>end) return;
    if (where<0 || where<begin)
    {
      if (begin==0) return;
      *first = begin = 0;
      *lastplusone = end = point-1;
      where = textsw_find_bytes(textsw, first, lastplusone, findstring, strlen(findstring), 0);
      if (where<0) return;
    }
      if (prompt==1)
      {
        textsw_possibly_normalize(textsw, where);
        textsw_set_selection(textsw, where, where+strlen(findstring), 1);
        result = notice_prompt(mypanel, 0,
           XV_X,                    0,
           XV_Y,                    700,
           NOTICE_MESSAGE_STRINGS, "REPLACE THIS STRING?", 0,
           NOTICE_BUTTON_YES,      "YES",                            
           NOTICE_BUTTON_NO,       "NO",      
           NOTICE_BUTTON,          "cancel", 101,
           0); 
        if (result==NOTICE_YES)
        {
          textsw_replace_bytes(textsw, *first, *first+strlen(findstring), replacestring, strlen(replacestring));
          end  += strlen(replacestring)-strlen(findstring);
        }
        else 
          if (result==NOTICE_NO) 
            {}
          else 
            {free(findstring); free(replacestring); return;}
      }
      else 
      {
        textsw_replace_bytes(textsw, *first, *first+strlen(findstring), replacestring, strlen(replacestring));
        end  += strlen(replacestring)-strlen(findstring);
      }
     begin = where+strlen(replacestring);
  }
  free(findstring); free(replacestring);
}

void 
Copy_string(Menu menu, Menu_item menu_item)
{
  Copy_string2();
}

void 
Copy_string1(Panel_item item)
{
  Copy_string2();
}

void 
Copy_string2(void)
{
  Seln_holder   holder;
  Seln_request *response;
  server = (Xv_Server)xv_get ( xv_get(frame, XV_SCREEN), SCREEN_SERVER );
  holder = selection_inquire(server, SELN_PRIMARY);
  response = selection_ask(server, &holder,
      SELN_REQ_CONTENTS_ASCII, 0,
      0);
  strcpy(copytext, response->data + sizeof (SELN_REQ_CONTENTS_ASCII) );
}

void 
Insert_string(Menu menu, Menu_item menu_item)
{
  Insert_string2();
}

void 
Insert_string1(Panel_item item)
{
  Insert_string2();
}

void 
Insert_string2(void)
{
  Seln_holder   holder;
  server = (Xv_Server)xv_get ( xv_get(frame, XV_SCREEN), SCREEN_SERVER );
  holder = selection_inquire(server, SELN_PRIMARY);
  (void)selection_ask(server, &holder,
      SELN_REQ_DELETE, 0,
      0);
  textsw_insert(textsw, copytext, strlen(copytext) );
}

void
configurate(void)
{
  FILE *fp;
  char xx[16000];
  char configfile[256];
  int  i,j;
  findstring=malloc(sizeof(" "));strcpy(findstring,"\0");
  xx[0]=' ';
  strncpy(configfile, thisfile, strlen(thisfile));
  strncpy(configfile+strlen(thisfile),".config",strlen(".config"));
  if(!(fp = fopen(configfile,"r")))
    { printf("Error: Configuration file '%s.config' not found\n", thisfile); return; }
  while(xx[0]!='#')
    if(read(fileno(fp), xx, 1)==0) return;
  if(read(fileno(fp), xx, 1)==0) return;
  i=0;
  while((int)xx[i]<58 && (int)xx[i]>47)
    if(read(fileno(fp), xx+(++i), 1)==0) return;
  textsw_buf_size=atol(xx);
  while(xx[0]!='@')
    if(read(fileno(fp), xx, 1)==0) return;
  if(read(fileno(fp), xx, 1)==0) return;
  
  /* Reading configuration file and initialising compiler options */
  for(i=0; i<16; i++)
  {
    while(xx[0]!='{' && xx[0]!='@')
    {  if(xx[0]=='@') break;   if(read(fileno(fp), xx, 1)==0) return;  }
    while(xx[0]!='\n')
      if(read(fileno(fp), xx, 1)==0) return;
      
    j=0;
    while(j==0 || xx[j-1]!='\n')
      { if(read(fileno(fp), xx+j, 1)==0) return;  j++; }
    xx[j-1]='\0';
    strncpy (compilername_buf[i], xx, strlen(xx)) ;
    
    j=0;
    while(j==0 || xx[j-1]!='\n')
      { if(read(fileno(fp), xx+j, 1)==0) return;  j++; }
    xx[j-1]='\0';
    strncpy (compiler_buf[i]    , xx, strlen(xx)) ;
    
    j=0;
    while(j==0 || xx[j-1]!='\n')
      { if(read(fileno(fp), xx+j, 1)==0) return;  j++; }
    xx[j-1]='\0';
    strncpy (linker_buf[i]      , xx, strlen(xx) ) ;
    
    j=0;
    while(j==0 || xx[j-1]!='\n')
      { if(read(fileno(fp), xx+j, 1)==0) return;  j++; }
    xx[j-1]='\0';
    strncpy (attributes_buf[i]      , xx, strlen(xx) ) ;
    
    j=0;
    while(j==0 || xx[j-1]!='\n')
      { if(read(fileno(fp), xx+j, 1)==0) return;  j++; }
    xx[j-1]='\0';
    strncpy (source_buf[i]      , xx, strlen(xx) ) ;
    
    j=0;
    while(j==0 || xx[j-1]!='\n')
      { if(read(fileno(fp), xx+j, 1)==0) return;  j++; }
    xx[j-1]='\0';
    strncpy (exec_buf[i]      , xx, strlen(xx) ) ;
      
    j=0;
    while(j==0 || xx[j-1]!='\n')
      { if(read(fileno(fp), xx+j, 1)==0) return;  j++; }
    xx[j-1]='\0';
    strncpy (usew_buf[i]      , xx, strlen(xx) ) ;
    
    j=0;
    while(j==0 || xx[j-1]!='\n')
      { if(read(fileno(fp), xx+j, 1)==0) return;  j++; }
    xx[j-1]='\0';
    strncpy (printer_buf[i]      , xx, strlen(xx) ) ;
    
    j=0;
    while(j==0 || xx[j-1]!='\n')
      { if(read(fileno(fp), xx+j, 1)==0) return;  j++; }
    xx[j-1]='\0';
    strncpy (printformat_buf[i]      , xx, strlen(xx) ) ;
    
    j=0;
    while(j==0 || xx[j-1]!='\n')
      { if(read(fileno(fp), xx+j, 1)==0) return;  j++; }
    xx[j-1]='\0';
    strncpy (printformat_odd_buf[i]      , xx, strlen(xx) ) ;
    
    j=0;
    while(j==0 || xx[j-1]!='\n')
      { if(read(fileno(fp), xx+j, 1)==0) return;  j++; }
    xx[j-1]='\0';
    strncpy (printformat_even_buf[i]      , xx, strlen(xx) ) ;
        
    while(xx[0]!='}')
      if(read(fileno(fp), xx, 1)==0) return;
  }
  while(xx[0]!='@')
    if(read(fileno(fp), xx, 1)==0) return;
    
  /* Reading help text from configuration file */
  if(read(fileno(fp), xx, 1)==0) return;
  while(xx[0]!='{' && xx[0]!='@')
  {  if(xx[0]=='@') break;   if(read(fileno(fp), xx, 1)==0) return;  }  
  while(xx[0]!='{')
    if(read(fileno(fp), xx, 1)==0) return;
  while(xx[0]!='\n')
    if(read(fileno(fp), xx, 1)==0) return;
    
  j=0;
  while(j==0 || xx[j-1]!='}')
    { if(read(fileno(fp), xx+j, 1)==0) return;  j++; }
  xx[j-1]='\0';
  strncpy (help_buf[0], xx, strlen(xx)) ;
    
  while(xx[0]!='{' && xx[0]!='@')
  {  if(xx[0]=='@') break;   if(read(fileno(fp), xx, 1)==0) return;  }
  while(xx[0]!='{')
    if(read(fileno(fp), xx, 1)==0) return;
  while(xx[0]!='\n')
    if(read(fileno(fp), xx, 1)==0) return;
    
  j=0;
  while(j==0 || xx[j-1]!='}')
    { if(read(fileno(fp), xx+j, 1)==0) return;  j++; }
  xx[j-1]='\0';
  strncpy (help_buf[1], xx, strlen(xx)) ;
  
  while(xx[0]!='{' && xx[0]!='@')
  {  if(xx[0]=='@') break;   if(read(fileno(fp), xx, 1)==0) return;  }
  while(xx[0]!='{')
    if(read(fileno(fp), xx, 1)==0) return;
  while(xx[0]!='\n')
    if(read(fileno(fp), xx, 1)==0) return;
    
  j=0;
  while(j==0 || xx[j-1]!='}')
    { if(read(fileno(fp), xx+j, 1)==0) return;  j++; }
  xx[j-1]='\0';
  strncpy (help_buf[2], xx, strlen(xx)) ;
  
  while(xx[0]!='{' && xx[0]!='@')
  {  if(xx[0]=='@') break;   if(read(fileno(fp), xx, 1)==0) return;  }
  while(xx[0]!='{')
    if(read(fileno(fp), xx, 1)==0) return;
  while(xx[0]!='\n')
    if(read(fileno(fp), xx, 1)==0) return;

  j=0;
  while(j==0 || xx[j-1]!='}')
    { if(read(fileno(fp), xx+j, 1)==0) return;  j++; }
  xx[j-1]='\0';
  strncpy (help_buf[3], xx, strlen(xx)) ;
}

void 
Shortwin(Menu menu, Menu_item menu_item)
{
  /* Extra Button Window */
  if (addframe!=0)  return;
  addframe = (Frame)xv_create ((int)0, FRAME, 
       FRAME_LABEL,            "Button Menu",
       0) ;
  /* Initialize menu buttons */
  addpanel = (Panel)xv_create (addframe, PANEL, 
       PANEL_LAYOUT, PANEL_VERTICAL, 
       0); 
  (void)xv_create(addpanel, PANEL_MESSAGE, 
       PANEL_LABEL_STRING, "========", 
       0);
  (void)xv_create(addpanel, PANEL_MESSAGE, 
       PANEL_LABEL_STRING,       "File", 
       0);
  (void)xv_create (addpanel, PANEL_BUTTON,
       PANEL_LABEL_STRING,       "Save",
       PANEL_NOTIFY_PROC,        Save1,
       0);
  (void)xv_create (addpanel, PANEL_BUTTON,
       PANEL_LABEL_STRING,       "Quit",
       PANEL_NOTIFY_PROC,        Quit1,
       0);
       
  (void)xv_create(addpanel, PANEL_MESSAGE, 
       PANEL_LABEL_STRING, "========", 
       0);
  (void)xv_create(addpanel, PANEL_MESSAGE, 
       PANEL_LABEL_STRING, "Tools", 
       0);
  (void)xv_create (addpanel, PANEL_BUTTON,
       PANEL_LABEL_STRING,       "line n",
       PANEL_NOTIFY_PROC,        Linenumber1,
       0);
  (void)xv_create (addpanel, PANEL_BUTTON,
       PANEL_LABEL_STRING,       "find >",
       PANEL_NOTIFY_PROC,        Find1,
       PANEL_CLIENT_DATA,        0,
       0);
  (void)xv_create (addpanel, PANEL_BUTTON,
       PANEL_LABEL_STRING,       "find <",
       PANEL_NOTIFY_PROC,        Find1,
       PANEL_CLIENT_DATA,        1,
       0);
  (void)xv_create (addpanel, PANEL_BUTTON,
       PANEL_LABEL_STRING,       "find` >",
       PANEL_NOTIFY_PROC,        Findnext1,
       PANEL_CLIENT_DATA,        0,
       0);
  (void)xv_create (addpanel, PANEL_BUTTON,
       PANEL_LABEL_STRING,       "find` <",
       PANEL_NOTIFY_PROC,        Findnext1,
       PANEL_CLIENT_DATA,        1,
       0);
  (void)xv_create (addpanel, PANEL_BUTTON,
       PANEL_LABEL_STRING,       "Copy",   
       PANEL_NOTIFY_PROC,        Copy_string1,
       0);
  (void)xv_create (addpanel, PANEL_BUTTON,
       PANEL_LABEL_STRING,       "Insert",   
       PANEL_NOTIFY_PROC,        Insert_string1,
       0);
 
  (void)xv_create(addpanel, PANEL_MESSAGE, 
       PANEL_LABEL_STRING, "========", 
       0);
  (void)xv_create(addpanel, PANEL_MESSAGE, 
       PANEL_LABEL_STRING, "Execute", 
       0);
  (void)xv_create (addpanel, PANEL_BUTTON,
       PANEL_LABEL_STRING,       "Compile",   
       PANEL_NOTIFY_PROC,        Compile1,
       0);
  (void)xv_create (addpanel, PANEL_BUTTON,
       PANEL_LABEL_STRING,       "Run Xap",   
       PANEL_NOTIFY_PROC,        RunX1,
       0);
  (void)xv_create (addpanel, PANEL_BUTTON,
       PANEL_LABEL_STRING,       "Run fg.",   
       PANEL_NOTIFY_PROC,        Run1,
       0);
  (void)xv_create (addpanel, PANEL_BUTTON,
       PANEL_LABEL_STRING,       "Break",   
       PANEL_NOTIFY_PROC,        Break1,
       0);
  (void)xv_create(addpanel, PANEL_MESSAGE, 
       PANEL_LABEL_STRING, "========", 
       0);
  (void)xv_create(addpanel, PANEL_MESSAGE, 
       PANEL_LABEL_STRING, "Buttons", 
       0);
  (void)xv_create (addpanel, PANEL_BUTTON,
       PANEL_LABEL_STRING,       "Close",   
       PANEL_NOTIFY_PROC,        Close,
       0);
  window_fit(addpanel);
  window_fit(addframe);
  xv_set(addframe, XV_SHOW, TRUE,  0);
}

void Close(Panel_item item, Event * event)
{
  xv_destroy_safe(addframe);
  addframe=0;
}

void 
Fontselect(Menu menu, Menu_item menu_item)
{
  /* Extra Button Window */
  addframe = (Frame)xv_create ((int)0, FRAME, 
       FRAME_LABEL,            "Font configuration",
       0) ;
  /* Initialize menu buttons */
  addpanel = (Panel)xv_create (addframe, PANEL, 
       PANEL_LAYOUT, PANEL_VERTICAL, 
       0); 
  (void)xv_create(addpanel, PANEL_MESSAGE, 
       PANEL_LABEL_STRING, "========", 
       0);
  (void)xv_create(addpanel, PANEL_MESSAGE, 
       PANEL_LABEL_STRING,       "File", 
       0);
  (void)xv_create (addpanel, PANEL_BUTTON,
       PANEL_LABEL_STRING,       "Save",
       PANEL_NOTIFY_PROC,        Save1,
       0);
  (void)xv_create (addpanel, PANEL_BUTTON,
       PANEL_LABEL_STRING,       "Quit",
       PANEL_NOTIFY_PROC,        Quit1,
       0);
  (void)xv_create (addpanel, PANEL_BUTTON,
       PANEL_LABEL_STRING,       "Close",   
       PANEL_NOTIFY_PROC,        Close,
       0);
  window_fit(addpanel);
  window_fit(addframe);
  xv_set(addframe, XV_SHOW, TRUE, /*XV_X,  1, XV_Y, 2,*/  0);
}


void select_conf(Menu menu, Menu_item menu_item)
{
  char *tempstr;
  compnum=xv_get(menu_item, MENU_CLIENT_DATA);
  tempstr=malloc(sizeof(compilername_buf[compnum])+sizeof(filename)+1);
  sprintf(tempstr,"%s: <%s>",compilername_buf[compnum],filename);
  xv_set(frame, FRAME_LABEL, tempstr );
  free(tempstr);
}

void 
compname_opt(Menu menu, Menu_item menu_item)
{
  int get_compname_opt();
  Panel_item item;
  if (opt_text>0)  { xv_destroy_safe(opt_text); opt_text=0; return; }
  opt_text = (Panel_item)xv_create (panel, PANEL_TEXT,
      /*XV_X,                       400,*/
      XV_Y,                       8,
      PANEL_VALUE,                compilername_buf[compnum],
      PANEL_LABEL_STRING,        "COMPILER DESCRIPTION -> ",
      PANEL_NOTIFY_PROC,          get_compname_opt,
      PANEL_LAYOUT,               PANEL_HORIZONTAL,
      PANEL_VALUE_DISPLAY_LENGTH, 30,
      PANEL_VALUE_STORED_LENGTH,  256,
      0);
      xv_set( opt_text,      WIN_SET_FOCUS,           opt_text, 0  );
}

int get_compname_opt(Panel_item item, Event * event)
{
  (void)strncpy(compilername_buf[compnum], (char *)xv_get(item, PANEL_VALUE), sizeof compilername_buf[compnum] );
  xv_destroy_safe(opt_text);
  opt_text=0;
}

void 
comp_opt(Menu menu, Menu_item menu_item)
{
  int get_comp_opt();
  Panel_item item;
  if (opt_text>0)  { xv_destroy_safe(opt_text); opt_text=0; return; }
  opt_text = (Panel_item)xv_create (panel, PANEL_TEXT,
      /*XV_X,                       400,*/
      XV_Y,                       8,
      PANEL_VALUE,                compiler_buf[compnum],
      PANEL_LABEL_STRING,        "COMPILER CALL -> ",
      PANEL_NOTIFY_PROC,          get_comp_opt,
      PANEL_LAYOUT,               PANEL_HORIZONTAL,
      PANEL_VALUE_DISPLAY_LENGTH, 30,
      PANEL_VALUE_STORED_LENGTH,  256,
      0);
}

int get_comp_opt(Panel_item item, Event * event)
{
  (void)strncpy(compiler_buf[compnum], (char *)xv_get(item, PANEL_VALUE), sizeof compiler_buf[compnum] );
  xv_destroy_safe(opt_text);
  opt_text=0;
}

void 
link_opt(Menu menu, Menu_item menu_item)
{
  int get_link_opt();
  Panel_item item;   
  if (opt_text>0)  { xv_destroy_safe(opt_text); opt_text=0; return; }
  opt_text = (Panel_item)xv_create (panel, PANEL_TEXT,
  /*XV_X,                       400,*/
  XV_Y,                       8,
  PANEL_VALUE,                linker_buf[compnum],
  PANEL_LABEL_STRING,        "LINKER CALL -> ",
  PANEL_NOTIFY_PROC,          get_link_opt,
  PANEL_LAYOUT,               PANEL_HORIZONTAL,
  PANEL_VALUE_DISPLAY_LENGTH, 30,
  PANEL_VALUE_STORED_LENGTH,  256,
  0);
}

int get_link_opt(Panel_item item, Event * event)
{
  (void)strncpy(linker_buf[compnum], (char *)xv_get(item, PANEL_VALUE), sizeof linker_buf[compnum] );
  xv_destroy_safe(opt_text);
  opt_text=0;
}

void 
attr_opt(Menu menu, Menu_item menu_item)
{
  int get_attr_opt();
  Panel_item item;
  if (opt_text>0)  { xv_destroy_safe(opt_text); opt_text=0; return; }
  opt_text = (Panel_item)xv_create (panel, PANEL_TEXT,
  /*XV_X,                       400,*/
  XV_Y,                       8,
  PANEL_VALUE,                attributes_buf[compnum],
  PANEL_LABEL_STRING,        "ATTRIBUTES LIST -> ",
  PANEL_NOTIFY_PROC,          get_attr_opt,
  PANEL_LAYOUT,               PANEL_HORIZONTAL,
  PANEL_VALUE_DISPLAY_LENGTH, 30,
  PANEL_VALUE_STORED_LENGTH,  256,
  0);
}

int get_attr_opt(Panel_item item, Event * event)
{
  (void)strncpy(attributes_buf[compnum], (char *)xv_get(item, PANEL_VALUE), sizeof attributes_buf [compnum] );
  xv_destroy_safe(opt_text);
  opt_text=0;
}

void 
srce_opt(Menu menu, Menu_item menu_item)
{
  int get_srce_opt();
  Panel_item item;
  if (opt_text>0)  { xv_destroy_safe(opt_text); opt_text=0; return; }
  opt_text = (Panel_item)xv_create (panel, PANEL_TEXT,
  /*XV_X,                       400,*/
  XV_Y,                       8,
  PANEL_VALUE,                source_buf[compnum],
  PANEL_LABEL_STRING,        "SOURCE FILE EXTENSION -> ",
  PANEL_NOTIFY_PROC,          get_srce_opt,
  PANEL_LAYOUT,               PANEL_HORIZONTAL,
  PANEL_VALUE_DISPLAY_LENGTH, 30,
  PANEL_VALUE_STORED_LENGTH,  256,
  0);
}

int get_srce_opt(Panel_item item, Event * event)
{
  (void)strncpy(source_buf[compnum], (char *)xv_get(item, PANEL_VALUE), sizeof source_buf[compnum] );
  xv_destroy_safe(opt_text);
  opt_text=0;
}

void 
exec_opt(Menu menu, Menu_item menu_item)
{
  int get_exec_opt();
  Panel_item item;
  if (opt_text>0)  { xv_destroy_safe(opt_text); opt_text=0; return; }
  opt_text = (Panel_item)xv_create (panel, PANEL_TEXT,
  /*XV_X,                       400,*/
  XV_Y,                       8,
  PANEL_VALUE,                exec_buf[compnum],
  PANEL_LABEL_STRING,        "TARGET FILE EXTENSION -> ",
  PANEL_NOTIFY_PROC,          get_exec_opt,
  PANEL_LAYOUT,               PANEL_HORIZONTAL,
  PANEL_VALUE_DISPLAY_LENGTH, 30,
  PANEL_VALUE_STORED_LENGTH,  256,
  0);
}

int get_exec_opt(Panel_item item, Event * event)
{
  (void)strncpy(exec_buf[compnum], (char *)xv_get(item, PANEL_VALUE), sizeof exec_buf[compnum] );
  xv_destroy_safe(opt_text);
  opt_text=0;
}

void 
usew_opt(Menu menu, Menu_item menu_item)
{
  int get_usew_opt();
  Panel_item item;
  if (opt_text>0)  { xv_destroy_safe(opt_text); opt_text=0; return; }
  opt_text = (Panel_item)xv_create (panel, PANEL_TEXT,
  /*XV_X,                       400,*/
  XV_Y,                       8,
  PANEL_VALUE,                usew_buf[compnum],
  PANEL_LABEL_STRING,        "CALL TARGET FILE WITH -> ",
  PANEL_NOTIFY_PROC,          get_usew_opt,
  PANEL_LAYOUT,               PANEL_HORIZONTAL,
  PANEL_VALUE_DISPLAY_LENGTH, 30,
  PANEL_VALUE_STORED_LENGTH,  256,
  0);
}

int get_usew_opt(Panel_item item, Event * event)
{
  (void)strncpy(usew_buf[compnum], (char *)xv_get(item, PANEL_VALUE), sizeof usew_buf[compnum] );
  xv_destroy_safe(opt_text);
  opt_text=0;
}

void 
tatt_opt(Menu menu, Menu_item menu_item)
{
  int get_tatt_opt();
  Panel_item item;
  if (opt_text>0)  { xv_destroy_safe(opt_text); opt_text=0; return; }
  opt_text = (Panel_item)xv_create (panel, PANEL_TEXT,
  /*XV_X,                       400,*/
  XV_Y,                       8,
  PANEL_VALUE,                tatt_buf[compnum],
  PANEL_LABEL_STRING,        "TARGET CALL ATTRIBUTES	 -> ",
  PANEL_NOTIFY_PROC,          get_tatt_opt,
  PANEL_LAYOUT,               PANEL_HORIZONTAL,
  PANEL_VALUE_DISPLAY_LENGTH, 30,
  PANEL_VALUE_STORED_LENGTH,  256,
  0);
}

int get_tatt_opt(Panel_item item, Event * event)
{
  (void)strncpy(tatt_buf[compnum], (char *)xv_get(item, PANEL_VALUE), sizeof tatt_buf[compnum] );
  xv_destroy_safe(opt_text);
  opt_text=0;
}

void 
prnt_opt(Menu menu, Menu_item menu_item)
{
  int get_prnt_opt();
  Panel_item item;
  if (opt_text>0)  { xv_destroy_safe(opt_text); opt_text=0; return; }
  opt_text = (Panel_item)xv_create (panel, PANEL_TEXT,
  /*XV_X,                       400,*/
  XV_Y,                       8,
  PANEL_VALUE,                printer_buf[compnum],
  PANEL_LABEL_STRING,        "PRINTER OUTPUT TO -> ",
  PANEL_NOTIFY_PROC,          get_prnt_opt,
  PANEL_LAYOUT,               PANEL_HORIZONTAL,
  PANEL_VALUE_DISPLAY_LENGTH, 30,
  PANEL_VALUE_STORED_LENGTH,  256,
  0);
}

int get_prnt_opt(Panel_item item, Event * event)
{
  (void)strncpy(printer_buf[compnum], (char *)xv_get(item, PANEL_VALUE), sizeof printer_buf[compnum] );
  xv_destroy_safe(opt_text);
  opt_text=0;
}

void 
form_opt(Menu menu, Menu_item menu_item)
{
  int get_form_opt();
  Panel_item item;
  if (opt_text>0)  { xv_destroy_safe(opt_text); opt_text=0; return; }
  opt_text = (Panel_item)xv_create (panel, PANEL_TEXT,
  /*XV_X,                       400,*/
  XV_Y,                       8,
  PANEL_VALUE,                printformat_buf[compnum],
  PANEL_LABEL_STRING,        "FORMATTED PRINT CALL -> ",
  PANEL_NOTIFY_PROC,          get_form_opt,
  PANEL_LAYOUT,               PANEL_HORIZONTAL,
  PANEL_VALUE_DISPLAY_LENGTH, 30,
  PANEL_VALUE_STORED_LENGTH,  1024,
  0);
}

int get_form_opt(Panel_item item, Event * event)
{
  (void)strncpy(printformat_buf[compnum], (char *)xv_get(item, PANEL_VALUE), sizeof printformat_buf[compnum] );
  xv_destroy_safe(opt_text);
  opt_text=0;
}

void 
form_odd_opt(Menu menu, Menu_item menu_item)
{
  int get_odd_form_opt();
  Panel_item item;
  if (opt_text>0)  { xv_destroy_safe(opt_text); opt_text=0; return; }
  opt_text = (Panel_item)xv_create (panel, PANEL_TEXT,
  /*XV_X,                       400,*/
  XV_Y,                       8,
  PANEL_VALUE,                printformat_odd_buf[compnum],
  PANEL_LABEL_STRING,        "ODD SIDES FORMATTED PRINT CALL -> ",
  PANEL_NOTIFY_PROC,          get_odd_form_opt,
  PANEL_LAYOUT,               PANEL_HORIZONTAL,
  PANEL_VALUE_DISPLAY_LENGTH, 30,
  PANEL_VALUE_STORED_LENGTH,  1024,
  0);
}

int get_odd_form_opt(Panel_item item, Event * event)
{
  (void)strncpy(printformat_odd_buf[compnum], (char *)xv_get(item, PANEL_VALUE), sizeof printformat_odd_buf[compnum] );
  xv_destroy_safe(opt_text);
  opt_text=0;
}

void 
form_even_opt(Menu menu, Menu_item menu_item)
{
  int get_even_form_opt();
  Panel_item item;
  if (opt_text>0)  { xv_destroy_safe(opt_text); opt_text=0; return; }
  opt_text = (Panel_item)xv_create (panel, PANEL_TEXT,
  /*XV_X,                       400,*/
  XV_Y,                       8,
  PANEL_VALUE,                printformat_even_buf[compnum],
  PANEL_LABEL_STRING,        "EVEN SIDES FORMATTED PRINT CALL -> ",
  PANEL_NOTIFY_PROC,          get_even_form_opt,
  PANEL_LAYOUT,               PANEL_HORIZONTAL,
  PANEL_VALUE_DISPLAY_LENGTH, 30,
  PANEL_VALUE_STORED_LENGTH,  1024,
  0);
}

int get_even_form_opt(Panel_item item, Event * event)
{
  (void)strncpy(printformat_even_buf[compnum], (char *)xv_get(item, PANEL_VALUE), sizeof printformat_even_buf[compnum] );
  xv_destroy_safe(opt_text);
  opt_text=0;
}

void 
Compile(Menu menu, Menu_item menu_item)
{
  Compile2();
}

void 
Compile1(Panel_item item)
{
  Compile2();
}

void 
Compile2(void)
{
  /*Saved_notify(panel);*/
  /*ttysw_input (tty, compiler_buf[compnum], strlen(compiler_buf[compnum]));*/
  Printformat_function(strcpy (malloc(sizeof(compiler_buf[compnum])+1),compiler_buf[compnum])); 
  /*ttysw_input (tty, " ", 1);
  ttysw_input (tty, filename, strlen(filename));
  ttysw_input (tty, " -o ", strlen(" -o "));
  ttysw_input (tty, filename, strlen(filename)-strlen(source_buf[compnum]));
  ttysw_input (tty, exec_buf[compnum], strlen(exec_buf[compnum]));*/
  ttysw_input (tty, " ", 1);
  ttysw_input (tty, linker_buf[compnum], strlen(linker_buf[compnum]));
  ttysw_input (tty, " ", 1);
  ttysw_input (tty, attributes_buf[compnum], strlen(attributes_buf[compnum]));
  ttysw_input (tty, "\n", 1);
}

void 
RunX(Menu menu, Menu_item menu_item)
{
RunX2();
}

void 
RunX1(Panel_item item)
{
RunX2();
}

void 
RunX2(void)
{
  getedittext();            
  ttysw_input (tty, usew_buf[compnum],  strlen(usew_buf[compnum]) );
  ttysw_input (tty, " ", 1);
  ttysw_input (tty, filename, strlen(filename)-strlen(source_buf[compnum]));
  ttysw_input (tty, exec_buf[compnum], strlen(exec_buf[compnum]));
  ttysw_input (tty, " ", 1);
  ttysw_input (tty, tatt_buf[compnum], strlen(tatt_buf[compnum]));
  ttysw_input (tty, " &\n", strlen(" &\n") );
}

void 
Run(Menu menu, Menu_item menu_item)
{
  Run2();
}

void 
Run1(Panel_item item)
{
  Run2();
}

void 
Run2(void)
{
  getedittext();
  ttysw_input (tty, usew_buf[compnum],  strlen(usew_buf[compnum]) );
  ttysw_input (tty, " ", 1);
  ttysw_input (tty, filename, strlen(filename)-strlen(source_buf[compnum]));
  ttysw_input (tty, exec_buf[compnum], strlen(exec_buf[compnum]));
  ttysw_input (tty, " ", 1);
  ttysw_input (tty,  tatt_buf[compnum], strlen(tatt_buf[compnum]));
  ttysw_input (tty, "\n", strlen("\n") );
}

void 
Break(Menu menu, Menu_item menu_item)
{
  Break2();
}

void 
Break1(Panel_item item)
{
  Break2();
}

void 
Break2(void)
{
  getedittext();
  ttysw_input (tty, "", 1);
}

void
move_to_top(void)
{
  /*textsw_possibly_normalize(helptext, 1);*/
}

void 
Helpabout(Menu menu, Menu_item menu_item)
{ 
  Panel mypanel;
  int exithelp();
  if (helpframe!=0)  return;
  helpframe = (Frame)xv_create ((int)0, FRAME, 
    FRAME_LABEL,     "Help: About this Workbench",
  0);
  mypanel = (Panel)xv_create(helpframe, PANEL,
       0);
  helptext = (Textsw)xv_create(mypanel, PANEL_MULTILINE_TEXT,
    PANEL_DISPLAY_ROWS, 25,
    PANEL_VALUE_DISPLAY_LENGTH, 72,
    /*XV_WIDTH,       500,
    XV_HEIGHT,      200,*/
    PANEL_VALUE, help_buf[0],
  0);
  xv_set(mypanel, XV_SHOW, TRUE, 0);
  (void)xv_create (mypanel, PANEL_BUTTON,
    PANEL_LABEL_STRING,       "OK",
    PANEL_NOTIFY_PROC,          exithelp,
  0);  
  /*xv_set(mypanel, PANEL_LAYOUT, PANEL_VERTICAL, 0);
  (void)xv_create (mypanel, PANEL_BUTTON,
    PANEL_LABEL_STRING,       "Up",
    PANEL_NOTIFY_PROC,          move_to_top,
    PANEL_NOTIFY_PROC,          move_to_top,
  0);  */
  window_fit(helptext);
  window_fit(mypanel);
  window_fit(helpframe);
  xv_set(helpframe, XV_SHOW, TRUE, 0);
}

void 
Helpquick(Menu menu, Menu_item menu_item)
{
  Panel mypanel;
  int exithelp();
  if (helpframe!=0)  return;
  helpframe = (Frame)xv_create ((int)0, FRAME, 
    FRAME_LABEL,    "Help: Quick Description" ,
  0);
  mypanel = (Panel)xv_create(helpframe, PANEL,
       0);
  helptext = (Textsw)xv_create(mypanel, PANEL_MULTILINE_TEXT,
    PANEL_DISPLAY_ROWS, 25,
    PANEL_VALUE_DISPLAY_LENGTH, 72,
    PANEL_VALUE, help_buf[1],
  0);
  xv_set(mypanel, XV_SHOW, TRUE, 0);
  (void)xv_create (mypanel, PANEL_BUTTON,
    PANEL_LABEL_STRING,       "OK",
    PANEL_NOTIFY_PROC,          exithelp,
  0);
  window_fit(mypanel);
  window_fit(helpframe);
  xv_set(helpframe, XV_SHOW, TRUE, 0);
}

void 
Helpdetail(Menu menu, Menu_item menu_item)
{
  Panel mypanel;
  int exithelp();
  if (helpframe!=0)  return;
  helpframe = (Frame)xv_create ((int)0, FRAME, 
    FRAME_LABEL,     "Help: Summary",
  0);
  mypanel = (Panel)xv_create(helpframe, PANEL,
       0);
  helptext = (Textsw)xv_create(mypanel, PANEL_MULTILINE_TEXT,
    PANEL_DISPLAY_ROWS, 25,
    PANEL_VALUE_DISPLAY_LENGTH, 72,
    PANEL_VALUE, help_buf[2],
  0);
  xv_set(mypanel, XV_SHOW, TRUE, 0);
  (void)xv_create (mypanel, PANEL_BUTTON,
    PANEL_LABEL_STRING,       "OK",
    PANEL_NOTIFY_PROC,          exithelp,
  0);
  window_fit(mypanel);
  window_fit(helpframe);
  xv_set(helpframe, XV_SHOW, TRUE, 0);
}

void 
Helpinternals(Menu menu, Menu_item menu_item)
{
  Panel mypanel;
  int exithelp();
  if (helpframe!=0)  return;
  helpframe = (Frame)xv_create ((int)0, FRAME, 
    FRAME_LABEL,     "Help: Internal informations",
  0);
  mypanel = (Panel)xv_create(helpframe, PANEL,
       0);
  helptext = (Textsw)xv_create(mypanel, PANEL_MULTILINE_TEXT,
    PANEL_DISPLAY_ROWS, 25,
    PANEL_VALUE_DISPLAY_LENGTH, 72,
    PANEL_VALUE, help_buf[3],
  0);
  xv_set(mypanel, XV_SHOW, TRUE, 0);
  (void)xv_create (mypanel, PANEL_BUTTON,
    PANEL_LABEL_STRING,       "OK",
    PANEL_NOTIFY_PROC,          exithelp,
  0);
  window_fit(mypanel);
  window_fit(helpframe);
  /*xv_set(helptext, TEXTSW_INSERTION_POINT, 1, 0);
  textsw_possibly_normalize(helptext, 1);*/
  xv_set(helpframe, XV_SHOW, TRUE, 0);
}

int exithelp(Panel_item item, Event * event)
{
  xv_destroy_safe(helpframe);
  helpframe=0;
}

void
initcolor(Xv_cmsdata * cms_data, unsigned char * red, unsigned char *green, unsigned char *blue)
{
  red[0] = 255 ;    green[0] = 255 ;    blue[0] = 255 ;   /* white      */
  red[1] = 255 ;    green[1] =  0  ;    blue[1] =  0  ;   /* red        */
  red[2] =  0  ;    green[2] = 255 ;    blue[2] =  0  ;   /* green      */
  red[3] =  0  ;    green[3] =  0  ;    blue[3] = 255 ;   /* blue       */
  red[4] = 255 ;    green[4] = 255 ;    blue[4] =  0  ;   /* yellow     */
  red[5] = 188 ;    green[5] = 143 ;    blue[5] = 143 ;   /* brown      */
  red[6] = 220 ;    green[6] = 220 ;    blue[6] = 220 ;   /* gray       */
  red[7] =  0  ;    green[7] =  0  ;    blue[7] =  0  ;   /* black      */
  red[8] = 255 ;    green[8] = 128 ;    blue[8] =  0  ;   /* orange     */
  red[9] =  0  ;    green[9] = 128 ;    blue[9] =  0  ;   /* mid_green  */
  red[10] = 128 ;   green[10] =  0  ;   blue[10] =  0  ;  /* mid_red    */
  red[11] =  0  ;   green[11] =  0  ;   blue[11] = 128 ;  /* mid_blue   */
  red[12] = 96  ;   green[12] =  0  ;   blue[12] = 128  ; /* violet     */
  red[13] =  0  ;   green[13] = 64  ;   blue[13] =  0  ;  /* dark_green */
  red[14] = 64  ;   green[14] =  0  ;   blue[14] =  0  ;  /* dark_red   */
  red[15] =  0  ;   green[15] =  0  ;   blue[15] = 64  ;  /* dark_blue  */

  cms_data->type      = XV_STATIC_CMS ;
  cms_data->size      = NUM_COLORS ;
  cms_data->rgb_count = NUM_COLORS ;
  cms_data->index     = 0 ;
  cms_data->red       = red ;
  cms_data->green     = green ;
  cms_data->blue      = blue ;
}

void
piccanvas_repaint(Canvas canvas, Xv_Window pw, Display      * display, Window xid, Xv_xrectlist * xrects)
{
  float m = (float) (xv_get(frame, XV_WIDTH)-50) / 1000. ;
  XSetForeground(display, gc, colors[10]);
  XFillArc(display, xid, gc, -10.*m, 15.*m, 425.*m, 140.*m, 5750, 3833);
  XSetForeground(display, gc, colors[8]);
  XFillArc(display, xid, gc, -10.*m, 20.*m, 400.*m, 140.*m, 5750, 3833);
  XSetForeground(display, gc, colors[4]);
  XFillArc(display, xid, gc, -10.*m, 25.*m, 375.*m, 140.*m, 5750, 3833);
  XSetForeground(display, gc, colors[2]);  XFillArc(display, xid, gc, -10.*m, 30.*m, 350.*m, 140.*m, 5750, 3833);
  XSetForeground(display, gc, colors[9]);/* ++ */
  XFillArc(display, xid, gc, -10.*m, 35.*m, 325.*m, 140.*m, 5750, 3833);
  XSetForeground(display, gc, colors[3]);
  XFillArc(display, xid, gc, -10.*m, 40.*m, 300.*m, 140.*m, 5750, 3833);
  XSetForeground(display, gc, colors[11]);
  XFillArc(display, xid, gc, -10.*m, 45.*m, 275.*m, 140.*m, 5750, 3833);
  XSetForeground(display, gc, colors[15]);
  XFillArc(display, xid, gc, -10.*m, 50.*m, 250.*m, 140.*m, 5750, 3833);
  XFillRectangle(display, xid, gc, 8.*m, 80.*m, 111.*m, 52.*m);
  XSetForeground(display, gc, colors[0]);
  XDrawString(display, xid, gc, 7.*m, 95.*m, "  Schlampisoft", 14);
  XDrawString(display, xid, gc, 7.*m, 110.*m, "   X-Windows", 12);
  XDrawString(display, xid, gc, 7.*m, 125.*m, "   Workbench", 12);
  XFillArc(display, xid, gc, 114.*m, 20.*m, 60.*m, 40.*m, 0, 17300);
  XFillArc(display, xid, gc, 114.*m, 50.*m, 60.*m, 40.*m, 11550, 17300);
  XSetForeground(display, gc, colors[7]);
  XFillArc(display, xid, gc, 116.*m, 22.*m, 58.*m, 38.*m, 0, 17300);
  XFillArc(display, xid, gc, 116.*m, 52.*m, 58.*m, 38.*m, 11550, 17300);
  XSetForeground(display, gc, colors[0]);
  XFillArc(display, xid, gc, 129.*m, 30.*m, 32.*m, 22.*m, 0, 23500);
  XFillArc(display, xid, gc, 129.*m, 60.*m, 32.*m, 22.*m, 0, 23500);
  XSetForeground(display, gc, colors[2]);
  XFillArc(display, xid, gc, 117.*m, 24.*m, 45.*m, 30.*m, 19650, 3250);
  XFillArc(display, xid, gc, 128.*m, 30.*m, 30.*m, 20.*m, 0, 23500);
  XFillArc(display, xid, gc, 128.*m, 60.*m, 30.*m, 20.*m, 0, 23500);
  XSetForeground(display, gc, colors[9]);
  XFillArc(display, xid, gc, 128.*m, 30.*m, 30.*m, 20.*m, 10000, 9800);
  XSetForeground(display, gc, colors[3]);
  XFillArc(display, xid, gc, 128.*m, 30.*m, 30.*m, 20.*m, 11200, 5150);
  XFillArc(display, xid, gc, 128.*m, 60.*m, 30.*m, 20.*m, 6300, 10200);
  XSetForeground(display, gc, colors[9]);
  XFillArc(display, xid, gc, 128.*m, 60.*m, 30.*m, 20.*m, 16300, 3600);
  XFillArc(display, xid, gc, 128.*m, 60.*m, 30.*m, 20.*m, 2800, 2800);
  XFillRectangle(display, xid, gc, 131.*m, 36.*m, 20.*m, 5.*m);
  XFillRectangle(display, xid, gc, 140.*m, 36.*m, 12.*m, 13.*m);
  XFillRectangle(display, xid, gc, 148.*m, 36.*m, 4.*m, 14.*m);
  XFillRectangle(display, xid, gc, 141.*m, 61.*m, 4.*m, 18.*m);
  XFillRectangle(display, xid, gc, 143.*m, 65.*m, 10.*m, 12.*m);
}


void
initchips(Panel panel, Server_image chips[])
{
  register int i;
  Display *dpy = (Display *)xv_get (panel, XV_DISPLAY);
  unsigned long mask, *pixels;
  XGCValues values;
  Pixmap pixmap;

  if (DefaultDepth(dpy, DefaultScreen(dpy)) == 1)
  {
    ttysw_output(tty, "Editor colors disabled in two-color mode\n" , 42);
  }  
  else
  {
    values.graphics_exposures = FALSE ;
    values.foreground = 1 ;
    mask = GCGraphicsExposures | GCForeground ;
    gc = XCreateGC(dpy,
         RootWindow(dpy, DefaultScreen(dpy)), mask, &values);

    pixels = (unsigned long *)xv_get (panel, WIN_X_COLOR_INDICES);
    for(i=0; i<NUM_COLORS; i++)
    {
      chips[i] = (Server_image)xv_create (XV_NULL, SERVER_IMAGE,
                  SERVER_IMAGE_COLORMAP, "palette",
      XV_WIDTH,           16,
      XV_HEIGHT,          16,
      SERVER_IMAGE_DEPTH,  8,
      0);
    XSetForeground(dpy, gc, pixels[i]);
    pixmap = (Pixmap)xv_get (chips[i], XV_XID);
    XFillRectangle(dpy, pixmap, gc, 0, 0, 16, 16);
    }
  }   
}



void 
choosecolor(Menu menu, Menu_item menu_item)
{
  int i;
  Frame myframe = (Frame)xv_create ((int)0, FRAME,
        XV_WIDTH,       120,
        XV_HEIGHT,       80, 
        FRAME_LABEL,    "SELECT A COLOR:",
        0);
  Panel mypanel = (Panel)xv_create (myframe, PANEL,
        PANEL_LAYOUT,   PANEL_VERTICAL,
        WIN_CMS_NAME,   "Palette",
        WIN_CMS_DATA,    &cms_data,
        XV_WIDTH,       120,
        XV_HEIGHT,       80,
        0);
}

void dummy () {}
