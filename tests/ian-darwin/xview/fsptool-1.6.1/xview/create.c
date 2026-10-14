/********************************************************************************/
/* create.c --									*/
/*										*/
/* (c)1993 A.J.Doherty								*/
/********************************************************************************/

#include "fsptool.h"
#include "frame.h"
#include "create.h"
#include "file.h"
#include "menu.h"
#include "resource.h"
#include "icon.h"
#include "batch.h"

#include "../lib/common.h"
#include "../lib/cache.h"
#include "../lib/fsp.h"
#include "../lib/unix.h"
#include "../config.h"

/********************************************************************************/

extern char 		*buf, *fsp_ver_cmd;

extern Server_image	fsptool_glyph, drop_busy_glyph, drop_idle_glyph;

extern HostInfo	       *hosts_list[MAX_HOSTS];

extern int		hosts_loaded;

/********************************************************************************/

Panel make_exec_action(int,const char*);

void load_fonts();

void make_baseframe();
void make_localframe();
void make_transferframe();
void make_filterframe();
void make_sethostframe();
void make_hostlistframe();
void make_aboutfsptoolframe();
void make_aboutfspframe(int);
void make_actionframe();
void make_toolframe();
void make_fspframe();
void make_dirlistframe();
void make_clientsframe();
void make_batchframe();
void make_filerframe();

/********************************************************************************/

Base_frame              baseframe;
Transfer_frame          transferframe;
Local_frame		localframe;
Filter_frame            filterframe;
Sethost_frame           sethostframe;
Hostlist_frame		hostlistframe;
Action_frame            actionframe;
Tool_frame		toolframe;
Fsp_frame		fspframe;
Dirlist_frame		dirlistframe;
Clients_frame		clientsframe;
Batch_frame		batchframe;
Generic_frame           aboutfsptoolframe, aboutfspframe;
Filer_frame		filerframe;

Xv_font                 fixed_font, bold_fixed_font;

/********************************************************************************/

void load_fonts(void)
{
fixed_font = (Xv_font) xv_find(baseframe.frame, FONT,
			FONT_FAMILY,	FONT_FAMILY_LUCIDA_FIXEDWIDTH,
			FONT_STYLE,	FONT_STYLE_NORMAL,
			0);

if (!fixed_font) {
    error_report(WARNING,"FSPtool: unable to load required fixed font\n");
    fixed_font = (Xv_font) xv_get(baseframe.frame, XV_FONT);
    }

bold_fixed_font = (Xv_font) xv_find(baseframe.frame, FONT,
			FONT_FAMILY,	FONT_FAMILY_LUCIDA_FIXEDWIDTH,
			FONT_STYLE,	FONT_STYLE_BOLD,
			0);

if (!bold_fixed_font) {
    error_report(WARNING,"FSPtool: unable to load required fixed bold font\n");
    bold_fixed_font = (Xv_font) xv_get(baseframe.frame, XV_FONT);
    }
}

/********************************************************************************/

void make_child_frames()

/* create all of the applications child frames initially required		*/

{
make_localframe();
make_transferframe();
make_filterframe();
make_sethostframe();
make_hostlistframe();
make_aboutfsptoolframe();
make_aboutfspframe(FALSE);
make_actionframe();
make_toolframe();
make_dirlistframe();
make_fspframe();
make_clientsframe();
make_batchframe();
make_filerframe();
}

/********************************************************************************/

void make_baseframe()

{ char *file_menu[] = { "Copy Files",
			"Add to Batch List",
			"File Filter ...",
			"File Viewers ...",
			"Quit Program", 0 },

       *view_menu[] = {	"Local Directory List ...",
			"View Selected File",
			"Batch Transfer List ...",
			"Transfer Status ...",
			"Host Information ...",
			"About FSP Client/Server...",
			"About FSPtool...", 0 },

 *properties_menu[] = { "FSPtool ...",
			"Directory Lists ...",
			"FSP ...",
			"FSP Clients...",
			"Save Layout",	0 },

	*dir_menu[] = { "Unselect All",
			"Select All",
			"Get Selection",
			"Add to Batch List",
			0 };


baseframe.frame = (Frame) xv_create(XV_NULL, FRAME,
	XV_WIDTH,		480,
	XV_HEIGHT,		500,
	FRAME_LABEL,		FSPTOOL_VERSION,
	FRAME_SHOW_FOOTER,	TRUE,
	FRAME_LEFT_FOOTER,	"No Host Set",
	FRAME_RIGHT_FOOTER,	"0 items, 0 selected",
	0);

load_fonts();

baseframe.panel = (Panel) xv_create(baseframe.frame, PANEL,
			XV_HELP_DATA,	"FSPtool:base_panel",
			PANEL_LAYOUT,	PANEL_HORIZONTAL,
			0);

baseframe.file_button = (Panel_item) xv_create(baseframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:file_button",
			PANEL_LABEL_STRING,	"File",
			PANEL_ITEM_MENU,     make_menu(file_menu_proc,file_menu),
			0);

baseframe.view_button = (Panel_item) xv_create(baseframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:view_button",
			PANEL_LABEL_STRING,	"View",
			PANEL_ITEM_MENU,     make_menu(view_menu_proc,view_menu),
			0);

baseframe.properties_button = (Panel_item) xv_create(baseframe.panel,PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:properties_button",
			PANEL_LABEL_STRING,	"Properties",
			PANEL_ITEM_MENU,
				make_menu(properties_menu_proc,properties_menu),
			0);

baseframe.sethost_button = (Panel_item) xv_create(baseframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:sethost_button",
			PANEL_LABEL_STRING,	"Set Host...",
			PANEL_NOTIFY_PROC,	sethost_proc,
			0);

baseframe.abort_button = (Panel_item) xv_create(baseframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:aborttransfer",
			PANEL_LABEL_STRING,	"Abort",
			PANEL_NOTIFY_PROC,	abort_proc,
			PANEL_INACTIVE,		TRUE,
			0);

baseframe.dnd = xv_create(baseframe.panel, DRAGDROP, 0);

baseframe.drop_target = xv_create(baseframe.panel, PANEL_DROP_TARGET,
			XV_HELP_DATA,		"FSPtool:remotedropsite",
			XV_X,			452,
			PANEL_DROP_GLYPH,	drop_idle_glyph,
			PANEL_DROP_BUSY_GLYPH,	drop_busy_glyph,
			PANEL_DROP_DND,		baseframe.dnd,
			PANEL_DROP_FULL,	TRUE,
			PANEL_DROP_SITE_DEFAULT,TRUE,
			PANEL_NOTIFY_PROC,	drop_notify_proc,
			0);

baseframe.cache_button = (Panel_item) xv_create(baseframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:remote_dir_cache",
			XV_Y,			36,
			PANEL_LABEL_STRING,	"Remote Directory",
			PANEL_ITEM_MENU,	make_cache_menu(),
			PANEL_INACTIVE,		TRUE,
			0);

baseframe.dir_text = (Panel_item) xv_create(baseframe.panel, PANEL_TEXT,
			XV_HELP_DATA,			"FSPtool:remote_dir_text",
			XV_Y,				38,
			PANEL_LABEL_STRING,		":",
			PANEL_VALUE_DISPLAY_LENGTH,	35,
			PANEL_LABEL_BOLD,		TRUE,
			PANEL_VALUE_STORED_LENGTH,	256,
			PANEL_VALUE,			"/",
			PANEL_INACTIVE,			TRUE,
			PANEL_NOTIFY_PROC,		remote_dir_proc,
			PANEL_NOTIFY_LEVEL,		PANEL_SPECIFIED,
			PANEL_NOTIFY_STRING,		"\r\n\t\033\040",
			0);

baseframe.dir_list = (Panel_item) xv_create(baseframe.panel, PANEL_LIST,
			XV_HELP_DATA,			"FSPtool:remote_dir_list",
			PANEL_NEXT_ROW,			-1,
			PANEL_LIST_DISPLAY_ROWS,	22,
			PANEL_LIST_WIDTH,		446,
			PANEL_LIST_ROW_HEIGHT,		17,
			PANEL_NOTIFY_PROC,		select_files,
			PANEL_ITEM_MENU,
				make_menu(dir_list_menu_proc,dir_menu),
			PANEL_CHOOSE_ONE,		FALSE,
			PANEL_CHOOSE_NONE,		TRUE,
			PANEL_INACTIVE,			TRUE,
			PANEL_LIST_MODE,		PANEL_LIST_READ,
			0);

xv_set(baseframe.frame,
		WIN_EVENT_PROC,	handle_baseframe,
		WIN_CONSUME_EVENTS,
			WIN_RESIZE, WIN_MAP_NOTIFY,
			0,
		0);
}

/********************************************************************************/

void make_transferframe(void)
{
transferframe.frame = (Frame) xv_create(baseframe.frame, FRAME_CMD,
			XV_X,			0,
			XV_Y,			0,
			FRAME_LABEL,		"FSPtool: Transfer Status",
			FRAME_SHOW_FOOTER,	TRUE,	
			FRAME_LEFT_FOOTER,	"",
			FRAME_RIGHT_FOOTER,	"0 MBytes/s",
			0);

transferframe.panel = (Panel) xv_get(transferframe.frame, FRAME_CMD_PANEL);
xv_set(transferframe.panel, XV_HELP_DATA, "FSPtool:transfer_panel", 0);

transferframe.status = (Panel_item) xv_create(transferframe.panel, PANEL_TEXT,
				XV_HELP_DATA,		"FSPtool:transfer_status",
				PANEL_READ_ONLY,	TRUE,
				PANEL_LABEL_STRING,	"Status:",
				PANEL_VALUE,		"Not Transferring",
				PANEL_VALUE_DISPLAY_LENGTH,	25,
				PANEL_VALUE_STORED_LENGTH,	60,
				PANEL_VALUE_UNDERLINED,	FALSE,
				0);

transferframe.size = (Panel_item) xv_create(transferframe.panel, PANEL_TEXT,
				XV_HELP_DATA,		"FSPtool:transfer_size",
				PANEL_NEXT_ROW,		-1,
				XV_X,			19,
				PANEL_READ_ONLY,	TRUE,
				PANEL_LABEL_STRING,	"Size:",
				PANEL_VALUE_DISPLAY_LENGTH,	25,
				PANEL_VALUE_STORED_LENGTH,	60,
				PANEL_VALUE,		"0 Bytes",
				PANEL_VALUE_UNDERLINED,	FALSE,
				0);

transferframe.gauge = (Panel_item) xv_create(transferframe.panel, PANEL_GAUGE,
				XV_HELP_DATA,		"FSPtool:transfer_file",
				PANEL_NEXT_ROW,		-1,
				XV_X,			22,
				PANEL_LABEL_STRING,	"File:",
				PANEL_MIN_VALUE,	0,
				PANEL_MAX_VALUE,	100,
				PANEL_TICKS,		20,
				PANEL_GAUGE_WIDTH,	200,
				PANEL_VALUE,		0,
				0);

transferframe.full_gauge = (Panel_item) xv_create(transferframe.panel,PANEL_GAUGE,
				XV_HELP_DATA,		"FSPtool:transfer_total",
				PANEL_NEXT_ROW,		-1,
				XV_X,			12,
				PANEL_LABEL_STRING,	"Total:",
				PANEL_MIN_VALUE,	0,
				PANEL_MAX_VALUE,	100,
				PANEL_TICKS,		20,
				PANEL_GAUGE_WIDTH,	200,
				PANEL_VALUE,		0,
				0);

window_fit(transferframe.panel);
window_fit(transferframe.frame);
}

/********************************************************************************/

void make_sethostframe()

{ Panel_item item;
  char	*host = get_fsp_host(),
	*port = get_fsp_port();


sethostframe.frame = (Frame) xv_create(baseframe.frame, FRAME_CMD,
				FRAME_LABEL,	"Set Host",
				XV_X,		0,
				XV_Y,		0,
				0);

sethostframe.panel = (Panel) xv_get(sethostframe.frame, FRAME_CMD_PANEL);

xv_set(sethostframe.panel,
		PANEL_CLIENT_DATA,	-1,
		XV_HELP_DATA,		"FSPtool:sethost_panel",
		0);

(void) xv_create(sethostframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:sethost_clear",
			PANEL_LABEL_STRING,	"Clear",
			PANEL_NOTIFY_PROC,	set_host_clear,
			0);

sethostframe.hostlist_button
		= (Panel_item) xv_create(sethostframe.panel,PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:sethost_hosts",
			PANEL_LABEL_STRING,	"Hosts",
			PANEL_ITEM_MENU,	make_hosts_menu(hosts_menu_proc),
			PANEL_CLIENT_DATA,	-1,
			0);

sethostframe.host = (Panel_item) xv_create(sethostframe.panel, PANEL_TEXT,
			XV_HELP_DATA,		"FSPtool:sethost_host",
			PANEL_NEXT_ROW,		-1,
			PANEL_LABEL_STRING,	"Host:",
			PANEL_VALUE_DISPLAY_LENGTH,	22,
			PANEL_VALUE_STORED_LENGTH,	30,
			PANEL_VALUE,		host ? host : "",
			PANEL_NOTIFY_PROC,	handle_host_entry,
			PANEL_CLIENT_DATA,	c_strdup(""),
			0);

sethostframe.port = (Panel_item) xv_create(sethostframe.panel, PANEL_TEXT,
			XV_HELP_DATA,		"FSPtool:sethost_port",
			XV_X,			8,
			PANEL_NEXT_ROW,		-1,
			PANEL_LABEL_STRING,	"Port:",
			PANEL_VALUE_DISPLAY_LENGTH,	22,
			PANEL_VALUE_STORED_LENGTH,	16,
			PANEL_VALUE, 		port ? port : "",
			PANEL_NOTIFY_PROC,	handle_port_entry,
			PANEL_CLIENT_DATA,	c_strdup(""),
			0);

item = (Panel_item) xv_create(sethostframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:sethost_apply",
			PANEL_NEXT_ROW,		-1,
			XV_X,			50,
			PANEL_LABEL_STRING,	"Apply",
			PANEL_NOTIFY_PROC,	set_host,
			0);

(void) xv_create(sethostframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:cancel",
			XV_X,			130,
			PANEL_LABEL_STRING,	"Cancel",
			PANEL_NOTIFY_PROC,	handle_frame,
			0);

xv_set(sethostframe.panel, PANEL_DEFAULT_ITEM, item, 0);

window_fit(sethostframe.panel);
window_fit(sethostframe.frame);
}

/********************************************************************************/

void make_hostlistframe(void)
{
hostlistframe.frame = (Frame) xv_create(baseframe.frame, FRAME_CMD,
				FRAME_LABEL,		"Host Information",
				XV_X,			0,
				XV_Y,			0,
				XV_WIDTH,		300,
				XV_HEIGHT,		427,
				FRAME_SHOW_FOOTER,	TRUE,
				FRAME_LEFT_FOOTER,	"",
				FRAME_RIGHT_FOOTER,	"",
				0);

hostlistframe.panel = (Panel) xv_get(hostlistframe.frame, FRAME_CMD_PANEL);
xv_set(hostlistframe.panel, XV_HELP_DATA, "FSPtool:hostlist_panel", 0);

hostlistframe.add_button = (Panel_item)
		xv_create(hostlistframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:hostlist_edit",
			PANEL_LABEL_STRING,	"Edit",
			PANEL_NOTIFY_PROC,	hostlist_edit_proc,
			0);

hostlistframe.delete_button = (Panel_item)
		xv_create(hostlistframe.panel,PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:hostlist_delete",
			PANEL_LABEL_STRING,	"Delete",
			PANEL_NOTIFY_PROC,	hostlist_delete_proc,
			0);

hostlistframe.delete_button=(Panel_item)
		xv_create(hostlistframe.panel,PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:hostlist_clear",
			PANEL_LABEL_STRING,	"Clear",
			PANEL_NOTIFY_PROC,	hostlist_clear_proc,
			0);

hostlistframe.load_button=(Panel_item)xv_create(hostlistframe.panel,PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:hostlist_load",
			PANEL_LABEL_STRING,	"Load",
			PANEL_NOTIFY_PROC,	hostlist_load_proc,
			0);

hostlistframe.save_button=(Panel_item)xv_create(hostlistframe.panel,PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:hostlist_save",
			PANEL_LABEL_STRING,	"Save",
			PANEL_NOTIFY_PROC,	hostlist_save_proc,
			0);

hostlistframe.alias = (Panel_item) xv_create(hostlistframe.panel, PANEL_TEXT,
			XV_HELP_DATA,			"FSPtool:hostlist_alias",
			PANEL_NEXT_ROW,			-1,
			PANEL_LABEL_STRING,		"Alias:",
			PANEL_VALUE_DISPLAY_LENGTH,	30,
			PANEL_VALUE_STORED_LENGTH,	31,
			PANEL_NOTIFY_PROC,		handle_host_entry,
			PANEL_VALUE,
				hosts_loaded ? hosts_list[0]->alias : "",
			0);

hostlistframe.host = (Panel_item) xv_create(hostlistframe.panel, PANEL_TEXT,
			XV_HELP_DATA,			"FSPtool:hostlist_host",
			PANEL_NEXT_ROW,			-1,
			XV_X,				8,
			PANEL_LABEL_STRING,		"Host:",
			PANEL_NOTIFY_PROC,		handle_host_entry,
			PANEL_VALUE_DISPLAY_LENGTH,	30,
			PANEL_VALUE_STORED_LENGTH,	63,
			PANEL_VALUE,
				hosts_loaded ? hosts_list[0]->hostname : "",
			0);

if (hosts_loaded)
    sprintf(buf,"%1d",hosts_list[0]->hostport);
else
    strcpy(buf,"");

hostlistframe.port = (Panel_item) xv_create(hostlistframe.panel, PANEL_TEXT,
			XV_HELP_DATA,			"FSPtool:hostlist_port",
			PANEL_NEXT_ROW,			-1,
			XV_X,				12,
			PANEL_LABEL_STRING,		"Port:",
			PANEL_NOTIFY_PROC,		handle_host_entry,
			PANEL_VALUE_DISPLAY_LENGTH,	30,
			PANEL_VALUE_STORED_LENGTH,	16,
			PANEL_VALUE,			buf,
			0);

hostlistframe.description = (Panel_item) xv_create(hostlistframe.panel,PANEL_TEXT,
			XV_HELP_DATA,			"FSPtool:hostlist_info",
			PANEL_NEXT_ROW,			-1,
			XV_X,				11,
			PANEL_LABEL_STRING,		"Info:",
			PANEL_VALUE_DISPLAY_LENGTH,	30,
			PANEL_VALUE_STORED_LENGTH,	127,
			PANEL_VALUE,
				hosts_loaded ? hosts_list[0]->description : "",
			0);

hostlistframe.ldir = (Panel_item) xv_create(hostlistframe.panel, PANEL_TEXT,
			XV_HELP_DATA,			"FSPtool:hostlist_local",
			PANEL_NEXT_ROW,			-1,
			XV_X,				19,
			PANEL_LABEL_STRING,		"Local Dir:",
			PANEL_VALUE_DISPLAY_LENGTH,	25,
			PANEL_VALUE_STORED_LENGTH,	63,
			0);

hostlistframe.rdir = (Panel_item) xv_create(hostlistframe.panel, PANEL_TEXT,
			XV_HELP_DATA,			"FSPtool:hostlist_remote",
			PANEL_NEXT_ROW,			-1,
			PANEL_LABEL_STRING,		"Remote Dir:",
			PANEL_NOTIFY_PROC,		handle_host_entry,
			PANEL_VALUE_DISPLAY_LENGTH,	25,
			PANEL_VALUE_STORED_LENGTH,	63,
			PANEL_VALUE,
				hosts_loaded ? hosts_list[0]->directory : "",
			0);

hostlistframe.host_list = (Panel_item) xv_create(hostlistframe.panel, PANEL_LIST,
			XV_HELP_DATA,		"FSPtool:hostlist_aliases",
			PANEL_NEXT_ROW,		-1,
			PANEL_LIST_TITLE,	"Host Aliases:",
			PANEL_NOTIFY_PROC,	hosts_select_proc,
			PANEL_LIST_DISPLAY_ROWS,10,
			PANEL_LIST_WIDTH,	270,
			PANEL_CHOOSE_ONE,	TRUE,
			PANEL_CHOOSE_NONE,	FALSE,
			PANEL_INACTIVE,		FALSE,
			PANEL_READ_ONLY,	TRUE,
			PANEL_LIST_MODE,	PANEL_LIST_READ,
			0);

return_hosts_list(hostlistframe.host_list);
window_fit(hostlistframe.panel);
}

/********************************************************************************/

void make_actionframe()

/* this function creates the pop-up window for the actions settings, the	*/
/* actions which the program will take when a particular file is double clicked	*/
/* upon. Sending a GIF file to xv etc, the default actions are in config.h	*/

{ Panel_item item;


actionframe.frame = (Frame) xv_create(baseframe.frame, FRAME_CMD,
				FRAME_LABEL,	"File Viewers",
				XV_X,		0,
				XV_Y,		0,
				0);

actionframe.panel = (Panel) xv_get(actionframe.frame, FRAME_CMD_PANEL);
xv_set(actionframe.panel, XV_HELP_DATA, "FSPtool:action_panel", 0);

(void) xv_create(actionframe.panel, PANEL_MESSAGE,
			PANEL_LABEL_STRING,	"File Action Settings:",
			PANEL_LABEL_BOLD,	TRUE,
			0);

actionframe.fileactions[UNKNOWN]   = make_exec_action(UNKNOWN,"Default");
actionframe.fileactions[TEXT_FILE] = make_exec_action(TEXT_FILE,"Text");
actionframe.fileactions[H_FILE]    = make_exec_action(H_FILE,"h file");
actionframe.fileactions[C_FILE]	   = make_exec_action(C_FILE,"c Source");
actionframe.fileactions[GIF_FILE]  = make_exec_action(GIF_FILE,"GIF");
actionframe.fileactions[PBM_FILE]  = make_exec_action(PBM_FILE,"PBM");
actionframe.fileactions[X11_FILE]  = make_exec_action(X11_FILE,"X11");
actionframe.fileactions[RAS_FILE]  = make_exec_action(RAS_FILE,"Raster");
actionframe.fileactions[PS_FILE]   = make_exec_action(PS_FILE,"Postscript");
actionframe.fileactions[JPEG_FILE] = make_exec_action(JPEG_FILE,"JPEG");
actionframe.fileactions[TIFF_FILE] = make_exec_action(TIFF_FILE,"TIFF");
actionframe.fileactions[MPG_FILE]  = make_exec_action(MPG_FILE,"MPG");
actionframe.fileactions[GL_FILE]   = make_exec_action(GL_FILE,"Grasp(GL)");
actionframe.fileactions[RLE_FILE]  = make_exec_action(RLE_FILE,"RLE");
actionframe.fileactions[AU_FILE]   = make_exec_action(AU_FILE,"audio");

(void) xv_create(actionframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:apply",
			PANEL_NEXT_ROW,		-1,
			XV_X,			75,
			PANEL_NOTIFY_PROC,	set_file_actions,
			PANEL_LABEL_STRING,	"Apply",
			0);

(void) xv_create(actionframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:save",
			PANEL_NOTIFY_PROC,	set_file_actions,
			PANEL_LABEL_STRING,	"Save",
			0);

item = (Panel_item) xv_create(actionframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:cancel",
			PANEL_LABEL_STRING,	"Cancel",
			PANEL_NOTIFY_PROC,	handle_frame,
			0);

xv_set(actionframe.panel, PANEL_DEFAULT_ITEM, item, 0);
window_fit(actionframe.panel);
window_fit(actionframe.frame);
}

/********************************************************************************/

Panel_item make_exec_action(int filetype, const char *string)
{
(void) xv_create(actionframe.panel, PANEL_MESSAGE,
			XV_HELP_DATA,			"FSPtool:action_exec",
			PANEL_NEXT_ROW,		-1,
			PANEL_LABEL_IMAGE,	return_file_glyph(filetype),
			0);

(void) xv_create(actionframe.panel, PANEL_MESSAGE,
			XV_HELP_DATA,			"FSPtool:action_exec",
			PANEL_LABEL_STRING,	string,
			PANEL_LABEL_BOLD,	TRUE,
			0);

return(xv_create(actionframe.panel, PANEL_TEXT,
			XV_HELP_DATA,			"FSPtool:action_exec",
			XV_X,				120,
			PANEL_LABEL_STRING,		"Exec:",
			PANEL_VALUE,			get_file_action(filetype),
			PANEL_VALUE_STORED_LENGTH,	255,
			PANEL_VALUE_DISPLAY_LENGTH,	20,
			0));
}

/********************************************************************************/

void make_filterframe()

{ Panel_item item;


filterframe.frame = (Frame) xv_create(baseframe.frame, FRAME_CMD,
				FRAME_LABEL,	"Set File Filter",
				XV_X,		0,
				XV_Y,		0,
				0);

filterframe.panel = (Panel) xv_get(filterframe.frame, FRAME_CMD_PANEL);
xv_set(filterframe.panel, XV_HELP_DATA, "FSPtool:filter_panel", 0);

filterframe.filter = (Panel_item) xv_create(filterframe.panel, PANEL_CHOICE,
			XV_HELP_DATA,		"FSPtool:filter_filter",
			PANEL_LABEL_STRING,	"File Filtering:",
			PANEL_CHOICE_STRINGS,	"On", "Off", 0,
			PANEL_VALUE,		1,
			0);

filterframe.message = (Panel_item) xv_create(filterframe.panel, PANEL_MESSAGE,
			XV_HELP_DATA,		"FSPtool:filter_options",
			PANEL_NEXT_ROW,		-1,
			PANEL_LABEL_STRING,
		"Filetypes ticked are included in the directory listing.",
			0);

filterframe.checkboxes = (Panel_item) xv_create(filterframe.panel,PANEL_CHECK_BOX,
			XV_HELP_DATA,		"FSPtool:filter_options",
			PANEL_NEXT_ROW,		-1,
			PANEL_LAYOUT,		PANEL_HORIZONTAL,
			PANEL_CHOICE_NCOLS,	4,
			PANEL_CHOOSE_ONE,	FALSE,
			PANEL_LABEL_STRING,	"Show:",
			PANEL_CHOICE_STRINGS,
				"TAR", "Z", "z", "ZIP",
				"TEXT", "H", "C", "GIF",
				"PBM", "X11", "RAS", "PS",
				"JPEG", "TIFF", "MPG", "GL",
				"RLE", "AU", "?",
				0,
			PANEL_VALUE,		1048575,
			0);

(void) xv_create(filterframe.panel, PANEL_BUTTON,
			PANEL_NEXT_ROW,		-1,
			XV_X,			85,
			XV_HELP_DATA,		"FSPtool:apply",
			PANEL_LABEL_STRING,	"Apply",
			PANEL_NOTIFY_PROC,	set_filter,
			0);

(void) xv_create(filterframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:save",
			PANEL_LABEL_STRING,	"Save",
			PANEL_NOTIFY_PROC,	set_filter,
			0);

item = (Panel_item) xv_create(filterframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:cancel",
			PANEL_LABEL_STRING,	"Cancel",
			PANEL_NOTIFY_PROC,	handle_frame,
			0);

xv_set(filterframe.panel, PANEL_DEFAULT_ITEM, item, 0);
window_fit(filterframe.panel);
window_fit(filterframe.frame);
}

/********************************************************************************/

void make_aboutfsptoolframe(void)
{
aboutfsptoolframe.frame = (Frame) xv_create(baseframe.frame, FRAME_CMD,
				FRAME_LABEL,	"About FSPtool",
				XV_X,		0,
				XV_Y,		0,
				0);

aboutfsptoolframe.panel = (Panel) xv_get(aboutfsptoolframe.frame,FRAME_CMD_PANEL);
xv_set(aboutfsptoolframe.panel, XV_HELP_DATA, "FSPtool:aboutfsptool_panel", 0);

(void) xv_create(aboutfsptoolframe.panel, PANEL_MESSAGE,
			XV_X,			12,
			XV_Y,			56,
			PANEL_LABEL_IMAGE,	fsptool_glyph,
			0);

strcpy(buf,FSPTOOL_VERSION);
strcat(buf," (c)1993 Andrew J. Doherty");

(void) xv_create(aboutfsptoolframe.panel, PANEL_MESSAGE,
		XV_X,			96,
		XV_Y,			10,
		PANEL_LABEL_STRING,	buf,
		PANEL_LABEL_BOLD,	TRUE,
		0);

(void) xv_create(aboutfsptoolframe.panel, PANEL_MESSAGE,
	XV_X,	96,	XV_Y,	35,
	PANEL_LABEL_STRING, "FSPtool - an easy to use OpenLook FSP interface",
	0);

(void) xv_create(aboutfsptoolframe.panel, PANEL_MESSAGE,
	XV_X,	96,	XV_Y,	60,
	PANEL_LABEL_STRING, "Email to ssudoher@uk.ac.reading",
	0);

(void) xv_create(aboutfsptoolframe.panel, PANEL_MESSAGE,
	XV_X,	96,	XV_Y,	95,
	PANEL_LABEL_STRING, "Similarities to Ftptool are purely coincidental :-)",
		0);

(void) xv_create(aboutfsptoolframe.panel, PANEL_MESSAGE,
	XV_X,	96,	XV_Y,	120,
	PANEL_LABEL_STRING,"The code - and icons - are all my own work. It looks",
	0);

(void) xv_create(aboutfsptoolframe.panel, PANEL_MESSAGE,
	XV_X,	96,	XV_Y,	145,
	PANEL_LABEL_STRING,
		"and functions similarly so people find it easy to use.",
	0);

window_fit(aboutfsptoolframe.panel);
window_fit(aboutfsptoolframe.frame);
}

/********************************************************************************/

void make_aboutfspframe ( int get_data )

{ FILE	*stream_in;

  char	 buffer[80],
	*argv1[] = { fsp_ver_cmd, "-h", 0 },
	*argv2[] = { fsp_ver_cmd, 0 };


aboutfspframe.frame = (Frame) xv_create(baseframe.frame, FRAME_CMD,
				FRAME_LABEL,	"About FSP",
				XV_X,		0,
				XV_Y,		0,
				0);

aboutfspframe.panel = (Panel) xv_get(aboutfspframe.frame, FRAME_CMD_PANEL);

xv_set(aboutfspframe.panel,
		XV_HELP_DATA,	"FSPtool:aboutfsp_panel",
		PANEL_LAYOUT,	PANEL_VERTICAL,
		0);

if (get_data) {
    if ((stream_in = perropen(argv1)) == 0)
	 error_report(ERROR,"Unable to carry out %s\n",FSP_VER_CMD);

    fgets(buffer,79,stream_in);
    perrclose(stream_in);

    (void) xv_create(aboutfspframe.panel, PANEL_MESSAGE,
			PANEL_LABEL_BOLD,	TRUE,
			PANEL_LABEL_STRING,	buffer,
			0);

    if ((stream_in = perropen(argv2)) == 0)
	error_report(ERROR,"Unable to carry out %s\n",FSP_VER_CMD);

    fgets(buffer,79,stream_in);

    while (!feof(stream_in))
	{
	(void) xv_create(aboutfspframe.panel, PANEL_MESSAGE,
		PANEL_NEXT_ROW,		-1,
		PANEL_LABEL_BOLD,	TRUE,
		PANEL_LABEL_STRING,	buffer,
		0);

	fgets(buffer,79,stream_in);
	}

    perrclose(stream_in);
    }

window_fit(aboutfspframe.panel);
window_fit(aboutfspframe.frame);
}

/********************************************************************************/

void make_toolframe()

{ Panel_item item;


toolframe.frame = (Frame) xv_create(baseframe.frame, FRAME_CMD,
				FRAME_LABEL,	"FSPtool Properties",
				XV_X,		0,
				XV_Y,		0,
				0);

toolframe.panel = (Panel) xv_get(toolframe.frame, FRAME_CMD_PANEL);
xv_set(toolframe.panel, XV_HELP_DATA, "FSPtool:tool_panel", 0);

toolframe.openlook = (Panel_item) xv_create(toolframe.panel, PANEL_CHECK_BOX,
			XV_X,			9,
			XV_HELP_DATA,		"FSPtool:tool_openlook",
			PANEL_LAYOUT,		PANEL_HORIZONTAL,
			PANEL_LABEL_STRING,	"OpenLook (Window Manager):",
			PANEL_CHOICE_STRINGS,	"Yes", 0,
			PANEL_VALUE,		1,
			0);

toolframe.cancelclose = (Panel_item) xv_create(toolframe.panel, PANEL_CHECK_BOX,
			PANEL_NEXT_ROW,		-1,
			XV_X,			37,
			XV_HELP_DATA,		"FSPtool:tool_cancelclose",
			PANEL_LAYOUT,		PANEL_HORIZONTAL,
			PANEL_LABEL_STRING,	"Close Frame on <Cancel>:",
			PANEL_CHOICE_STRINGS,	"Yes", 0,
			PANEL_VALUE,		0,
			0);

toolframe.menuclose = (Panel_item) xv_create(toolframe.panel, PANEL_CHECK_BOX,
			PANEL_NEXT_ROW,		-1,
			XV_HELP_DATA,		"FSPtool:tool_menuclose",
			PANEL_LAYOUT,		PANEL_HORIZONTAL,
			PANEL_LABEL_STRING,	"Close Open Frame from Menu:",
			PANEL_CHOICE_STRINGS,	"Yes", 0,
			PANEL_VALUE,		0,
			0);

toolframe.hostread = (Panel_item) xv_create(toolframe.panel, PANEL_CHECK_BOX,
			PANEL_NEXT_ROW,		-1,
			XV_X,			6,
			XV_HELP_DATA,		"FSPtool:tool_hostread",
			PANEL_LAYOUT,		PANEL_HORIZONTAL,
			PANEL_LABEL_STRING,	"Open directory on host select:",
			PANEL_CHOICE_STRINGS,	"Yes", 0,
			PANEL_VALUE,		0,
			0);

(void) xv_create(toolframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:apply",
			PANEL_NEXT_ROW,		-1,
			XV_X,			55,
			PANEL_LABEL_STRING,	"Apply",
			PANEL_NOTIFY_PROC,	set_tool_properties,
			0);

(void) xv_create(toolframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:save",
			PANEL_LABEL_STRING,	"Save",
			PANEL_NOTIFY_PROC,	set_tool_properties,
			0);

item = (Panel_item) xv_create(toolframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:cancel",
			PANEL_LABEL_STRING,	"Cancel",
			PANEL_NOTIFY_PROC,	handle_frame,
			0);

xv_set(toolframe.panel, PANEL_DEFAULT_ITEM, item, 0);

window_fit(toolframe.panel);
window_fit(toolframe.frame);
}

/********************************************************************************/

void make_dirlistframe()

{ Panel_item item;


dirlistframe.frame = (Frame) xv_create(baseframe.frame, FRAME_CMD,
				FRAME_LABEL,	"Directory Listings",
				XV_X,		0,
				XV_Y,		0,
				0);

dirlistframe.panel = (Panel) xv_get(dirlistframe.frame, FRAME_CMD_PANEL);
xv_set(dirlistframe.panel, XV_HELP_DATA, "FSPtool:dirlist_panel", 0);

dirlistframe.cachesize = (Panel_item)xv_create(dirlistframe.panel, PANEL_SLIDER,
			PANEL_NEXT_ROW,		-1,
			XV_HELP_DATA,		"FSPtool:dirlist_cachesize",
			XV_X,			36,
			XV_WIDTH,		400,
			PANEL_LABEL_STRING,	"Cache Size:",
			PANEL_MIN_VALUE,	1,
			PANEL_MAX_VALUE,	MAX_CACHE_SIZE,
			PANEL_VALUE,		MAX_CACHE_SIZE,
			0);

dirlistframe.cachetimeout = (Panel_item)xv_create(dirlistframe.panel,PANEL_SLIDER,
			PANEL_NEXT_ROW,		-1,
			XV_HELP_DATA,		"FSPtool:dirlist_cachetimeout",
			XV_WIDTH,		400,
			PANEL_LABEL_STRING,	"Timeout (Mins):",
			PANEL_MIN_VALUE,	0,
			PANEL_MAX_VALUE,	MAX_CACHE_TIMEOUT,
			PANEL_VALUE,		MAX_CACHE_TIMEOUT,
			0);

dirlistframe.sorttype = (Panel_item) xv_create(dirlistframe.panel, PANEL_CHOICE,
			PANEL_NEXT_ROW,		-1,
			XV_HELP_DATA,		"FSPtool:dirlist_sorttype",
			XV_X,			12,
			PANEL_LABEL_STRING,	"File listing sort:",
			PANEL_CHOICE_STRINGS,
				"By Name", "By Date", "By Size",
				0,
			PANEL_VALUE,		0,
			PANEL_NOTIFY_PROC,	set_order_type,
			0);

dirlistframe.alphasort = (Panel_item) xv_create(dirlistframe.panel, PANEL_CHOICE,
			XV_HELP_DATA,		"FSPtool:dirlist_alphasort",
			PANEL_NEXT_ROW,		-1,
			PANEL_LABEL_STRING,	"Name Sort Order:",
			PANEL_CHOICE_STRINGS,
				"Alphabetic", "Reverse-Alphabetic", 0,
			PANEL_VALUE,		0,
			PANEL_INACTIVE,		FALSE,
			0);

dirlistframe.timesort = (Panel_item) xv_create(dirlistframe.panel, PANEL_CHOICE,
			XV_HELP_DATA,		"FSPtool:dirlist_timesort",
			PANEL_NEXT_ROW,		-1,
			XV_X,			12,
			PANEL_LABEL_STRING,	"Date Sort Order:",
			PANEL_CHOICE_STRINGS,
				"Newest First", "Oldest First", 0,
			PANEL_VALUE,		0,
			PANEL_INACTIVE,		TRUE,
			0);

dirlistframe.sizesort = (Panel_item) xv_create(dirlistframe.panel, PANEL_CHOICE,
			XV_HELP_DATA,		"FSPtool:dirlist_sizesort",
			PANEL_NEXT_ROW,		-1,
			XV_X,			15,
			PANEL_LABEL_STRING,	"Size Sort Order:",
			PANEL_CHOICE_STRINGS,
				"Smallest First", "Largest First", 0,
			PANEL_VALUE,		0,
			PANEL_INACTIVE,		TRUE,
			0);

(void) xv_create(dirlistframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:apply",
			PANEL_NEXT_ROW,		-1,
			XV_X,			85,
			PANEL_LABEL_STRING,	"Apply",
			PANEL_NOTIFY_PROC,	set_dirlist_properties,
			0);

(void) xv_create(dirlistframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:save",
			PANEL_LABEL_STRING,	"Save",
			PANEL_NOTIFY_PROC,	set_dirlist_properties,
			0);

item = (Panel_item) xv_create(dirlistframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:cancel",
			PANEL_LABEL_STRING,	"Cancel",
			PANEL_NOTIFY_PROC,	handle_frame,
			0);

xv_set(dirlistframe.panel, PANEL_DEFAULT_ITEM, item, 0);

window_fit(dirlistframe.panel);
window_fit(dirlistframe.frame);
}

/********************************************************************************/

void make_fspframe()

{ Panel_item item;


fspframe.frame = (Frame) xv_create(baseframe.frame, FRAME_CMD,
				FRAME_LABEL,	"FSP Properties",
				XV_X,		0,
				XV_Y,		0,
				0);

fspframe.panel = (Panel) xv_get(fspframe.frame, FRAME_CMD_PANEL);
xv_set(fspframe.panel, XV_HELP_DATA, "FSPtool:fsp_panel", 0);

fspframe.bufsize = (Panel_item) xv_create(fspframe.panel, PANEL_CHOICE,
			XV_HELP_DATA,		"FSPtool:fsp_buffer_size",
			PANEL_NEXT_ROW,		-1,
			PANEL_LABEL_STRING,	"Buffer (Bytes):",
			PANEL_LAYOUT,		PANEL_HORIZONTAL,
			PANEL_CHOICE_STRINGS,
				"128", "256", "512", "1024", 0,
			PANEL_CHOOSE_ONE,	TRUE,
			PANEL_CHOOSE_NONE,	FALSE,
			PANEL_VALUE,		3,
			0);

fspframe.localport = (Panel_item) xv_create(fspframe.panel, PANEL_NUMERIC_TEXT,
			XV_HELP_DATA,		"FSPtool:fsp_localport",
			XV_X,			33,
			PANEL_NEXT_ROW,		-1,
			PANEL_LABEL_STRING,	"Local Port:",
			PANEL_MIN_VALUE,	0,
			PANEL_MAX_VALUE,	100000,
			0);

fspframe.timeout = (Panel_item) xv_create(fspframe.panel, PANEL_NUMERIC_TEXT,
			XV_HELP_DATA,		"FSPtool:fsp_timeout_size",
			XV_X,			43,
			PANEL_NEXT_ROW,		-1,
			PANEL_LABEL_STRING,	"Timeout:",
			PANEL_VALUE,		4,
			PANEL_MIN_VALUE,	1,
			PANEL_MAX_VALUE,	10,
			0);

fspframe.delaysize = (Panel_item) xv_create(fspframe.panel, PANEL_CHOICE,
			XV_HELP_DATA,		"FSPtool:fsp_delay_size",
			XV_X,			26,
			PANEL_NEXT_ROW,		-1,
			PANEL_LAYOUT,		PANEL_HORIZONTAL,
			PANEL_LABEL_STRING,	"Delay (mS):",
			PANEL_CHOICE_STRINGS,
				"500", "1000", "2000", "3000", "5000", 0,
			PANEL_CHOOSE_ONE,	TRUE,
			PANEL_CHOOSE_NONE,	FALSE,
			PANEL_VALUE,		3,
			0);

(void) xv_create(fspframe.panel, PANEL_BUTTON,
			PANEL_NEXT_ROW,		-1,
			XV_X,			94,
			XV_HELP_DATA,		"FSPtool:apply",
			PANEL_LABEL_STRING,	"Apply",
			PANEL_NOTIFY_PROC,	set_fsp_properties,
			0);

(void) xv_create(fspframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:save",
			PANEL_LABEL_STRING,	"Save",
			PANEL_NOTIFY_PROC,	set_fsp_properties,
			0);

item = (Panel_item) xv_create(fspframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:cancel",
			PANEL_LABEL_STRING,	"Cancel",
			PANEL_NOTIFY_PROC,	handle_frame,
			0);

xv_set(fspframe.panel, PANEL_DEFAULT_ITEM, item, 0);
window_fit(fspframe.panel);
window_fit(fspframe.frame);
}

/********************************************************************************/

void make_clientsframe()

{ Panel_item item;


clientsframe.frame = (Frame) xv_create(baseframe.frame, FRAME_CMD,
				FRAME_LABEL,	"Set FSP Clients",
				XV_X,		0,
				XV_Y,		0,
				0);

clientsframe.panel = (Panel) xv_get(clientsframe.frame, FRAME_CMD_PANEL);
xv_set(clientsframe.panel, XV_HELP_DATA, "FSPtool:clients_panel", 0);

(void) xv_create(clientsframe.panel, PANEL_MESSAGE,
			PANEL_LABEL_BOLD,	TRUE,
			PANEL_LABEL_STRING,	"FSP Client Programs",
			0);


clientsframe.fver = (Panel_item) xv_create(clientsframe.panel, PANEL_TEXT,
			XV_HELP_DATA,			"FSPtool:clients_fver",
			PANEL_LABEL_STRING,		"fver:",
			PANEL_NEXT_ROW,			-1,
			PANEL_VALUE,			FSP_VER_CMD,
			PANEL_VALUE_STORED_LENGTH,	255,
			PANEL_VALUE_DISPLAY_LENGTH,	25,
			0);

clientsframe.fls = (Panel_item) xv_create(clientsframe.panel, PANEL_TEXT,
			XV_HELP_DATA,			"FSPtool:clients_fls",
			PANEL_LABEL_STRING,		"fls  :",
			PANEL_NEXT_ROW,			-1,
			XV_X,				5,
			PANEL_VALUE,			FSP_LS_CMD,
			PANEL_VALUE_STORED_LENGTH,	255,
			PANEL_VALUE_DISPLAY_LENGTH,	25,
			0);

clientsframe.fget = (Panel_item) xv_create(clientsframe.panel, PANEL_TEXT,
			XV_HELP_DATA,			"FSPtool:clients_fget",
			PANEL_LABEL_STRING,		"fget:",
			XV_X,				5,
			PANEL_NEXT_ROW,			-1,
			PANEL_VALUE,			FSP_GET_CMD,
			PANEL_VALUE_STORED_LENGTH,	255,
			PANEL_VALUE_DISPLAY_LENGTH,	25,
			0);

clientsframe.fput = (Panel_item) xv_create(clientsframe.panel, PANEL_TEXT,
			XV_HELP_DATA,			"FSPtool:clients_fput",
			PANEL_NEXT_ROW,			-1,
			XV_X,				3,
			PANEL_LABEL_STRING,		"fput:",
			PANEL_VALUE,			FSP_PUT_CMD,
			PANEL_VALUE_STORED_LENGTH,	255,
			PANEL_VALUE_DISPLAY_LENGTH,	25,
			0);

(void) xv_create(clientsframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:apply",
			PANEL_NEXT_ROW,		-1,
			XV_X,			35,
			PANEL_LABEL_STRING,	"Apply",
			PANEL_NOTIFY_PROC,	set_fsp_clients,
			0);

(void) xv_create(clientsframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:save",
			PANEL_LABEL_STRING,	"Save",
			PANEL_NOTIFY_PROC,	set_fsp_clients,
			0);

item = (Panel_item) xv_create(clientsframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:cancel",
			PANEL_LABEL_STRING,	"Cancel",
			PANEL_NOTIFY_PROC,	handle_frame,
			0);

xv_set(clientsframe.panel, PANEL_DEFAULT_ITEM, item, 0);
window_fit(clientsframe.panel);
window_fit(clientsframe.frame);
}

/********************************************************************************/

void make_batchframe()

{ char	*write_menu[]   = { "Textfile ...", 0 };


batchframe.frame = (Frame) xv_create(baseframe.frame, FRAME_CMD,
				FRAME_LABEL,		"Batch Transfer",
				FRAME_SHOW_RESIZE_CORNER,	TRUE,
				FRAME_SHOW_FOOTER,		TRUE,
				FRAME_LEFT_FOOTER,		"",
				FRAME_RIGHT_FOOTER,		"",
				XV_X,				0,
				XV_Y,				0,
				0);

batchframe.panel = (Panel) xv_get(batchframe.frame, FRAME_CMD_PANEL);
xv_set(batchframe.panel, XV_HELP_DATA, "FSPtool:batch_panel", 0);

batchframe.transfer_button = (Panel_item)xv_create(batchframe.panel,PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:batch_transfer_button",
			PANEL_LABEL_STRING,	"Do Transfer",
			PANEL_NOTIFY_PROC,	do_transfer_callback,
			0);

batchframe.write_button = (Panel_item)xv_create(batchframe.panel,PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:batch_write_button",
			PANEL_LABEL_STRING,	"Write",
			PANEL_ITEM_MENU,
				make_menu(batch_write_menu_proc, write_menu),
			0);

batchframe.options_button = (Panel_item)xv_create(batchframe.panel,PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:batch_options_button",
			PANEL_LABEL_STRING,	"Options",
			PANEL_ITEM_MENU,	make_batch_options_menu(),
			0);

batchframe.batch_list = (Panel_item) xv_create(batchframe.panel, PANEL_LIST,
			XV_HELP_DATA,			"FSPtool:batch_list",
			PANEL_NOTIFY_PROC,		batch_list_notify_proc,
			PANEL_NEXT_ROW,			-1,
			PANEL_LIST_TITLE,		"Batch Transfer Files:",
			PANEL_LIST_DISPLAY_ROWS,	12,
			PANEL_LIST_WIDTH,		275,
			PANEL_CHOOSE_ONE,		FALSE,
			PANEL_CHOOSE_NONE,		TRUE,
			PANEL_READ_ONLY,		TRUE,
			0);

window_fit(batchframe.panel);
window_fit(batchframe.frame);

xv_set(batchframe.frame,
		WIN_EVENT_PROC,	handle_batch_panel_resize,
		WIN_CONSUME_EVENTS,
			WIN_RESIZE,
			0,
		0);
}

/********************************************************************************/

void make_localframe()

/* this fn creates the local directory frame, enabling the current download	*/
/* directory to be changed and it's contents to be displayed			*/

{ char *tmp,
       *file_strings[] = { "Copy Files", 0 },
       *view_strings[] = { "View Selected File", 0 };

   int  freek = 0, freep = 0;


localframe.frame = (Frame) xv_create(baseframe.frame, FRAME_CMD,
				FRAME_LABEL,			"Local Directory",
				FRAME_SHOW_RESIZE_CORNER,	TRUE,
				FRAME_SHOW_FOOTER,		TRUE,
				FRAME_LEFT_FOOTER,		"",
				FRAME_RIGHT_FOOTER,		"",
				XV_X,			  	0,
				XV_Y,			  	0,
				0);

localframe.panel = (Panel) xv_get(localframe.frame, FRAME_CMD_PANEL);
xv_set(localframe.panel, XV_HELP_DATA, "FSPtool:local_panel", 0);

if ((tmp = getenv("HOME")) == 0)
    tmp = "";

localframe.file_button = (Panel_item) xv_create(localframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:local_file_button",
			PANEL_LABEL_STRING,	"File",
			PANEL_ITEM_MENU,
				make_menu(local_file_menu_proc,file_strings),
			0);

localframe.view_button = (Panel_item) xv_create(localframe.panel, PANEL_BUTTON,
			XV_HELP_DATA,		"FSPtool:local_view_button",
			PANEL_LABEL_STRING,	"View",
			PANEL_ITEM_MENU,
				make_menu(local_view_menu_proc,view_strings),
			0);

localframe.localdir = (Panel_item) xv_create(localframe.panel, PANEL_TEXT,
			PANEL_NEXT_ROW,			-1,
			XV_HELP_DATA,			"FSPtool:local_dir_text",
			PANEL_LABEL_STRING,		"Local Directory:",
			PANEL_VALUE_DISPLAY_LENGTH,	27,
			PANEL_LABEL_BOLD,		TRUE,
			PANEL_VALUE_STORED_LENGTH,	512,
			PANEL_VALUE,			tmp,
			PANEL_NOTIFY_PROC,		local_dir_proc,
			PANEL_NOTIFY_LEVEL,		PANEL_SPECIFIED,
			PANEL_NOTIFY_STRING,		"\r\n\t\033\040",
			0);


free_space(&freek,&freep);
sprintf(buf,"%1d Kbytes (%1d%% Free)",freek,freep);

localframe.free = (Panel_item) xv_create(localframe.panel, PANEL_TEXT,
			XV_HELP_DATA,			"FSPtool:local_free",
			XV_X,				34,
			PANEL_NEXT_ROW,			-1,
			PANEL_READ_ONLY,		TRUE,
			PANEL_LABEL_STRING,		"Free Space:",
			PANEL_LABEL_BOLD,		TRUE,
			PANEL_VALUE_STORED_LENGTH,	28,
			PANEL_VALUE_DISPLAY_LENGTH,	28,
			PANEL_VALUE,			buf,
			PANEL_VALUE_UNDERLINED,		FALSE,
			0);

localframe.dir_list = (Panel_item) xv_create(localframe.panel, PANEL_LIST,
			XV_HELP_DATA,			"FSPtool:local_dir_list",
			PANEL_CLIENT_DATA,		0,
			PANEL_NEXT_ROW,			-1,
			PANEL_LIST_DISPLAY_ROWS,	12,
			PANEL_LIST_WIDTH,		300,
			PANEL_LIST_ROW_HEIGHT,		17,
			PANEL_CHOOSE_ONE,		FALSE,
			PANEL_CHOOSE_NONE,		TRUE,
			PANEL_READ_ONLY,		TRUE,
			PANEL_NOTIFY_PROC,		select_local_files,
			0);

xv_set(localframe.panel,
		WIN_EVENT_PROC,	handle_local_panel_resize,
		WIN_CONSUME_EVENTS, WIN_RESIZE, 0,
		0);

window_fit(localframe.panel);
window_fit(localframe.frame);
}

/********************************************************************************/

void make_filerframe(void)
{
filerframe.frame = (Frame) xv_create(baseframe.frame, FRAME_CMD,
				FRAME_LABEL,	"FSPtool:",
				XV_X,		0,
				XV_Y,		0,
				0);

filerframe.panel = (Panel) xv_get(filerframe.frame, FRAME_CMD_PANEL);

filerframe.dirname = (Panel_item) xv_create(filerframe.panel, PANEL_TEXT,
			PANEL_LABEL_STRING,		"Directory:",
			PANEL_VALUE,			"",
			PANEL_VALUE_STORED_LENGTH,	255,
			PANEL_VALUE_DISPLAY_LENGTH,	50,
			0);

filerframe.filename = (Panel_item) xv_create(filerframe.panel, PANEL_TEXT,
			PANEL_NEXT_ROW,			-1,
			XV_X,				41,
			PANEL_LABEL_STRING,		"File:",
			PANEL_VALUE,			"",
			PANEL_VALUE_STORED_LENGTH,	255,
			PANEL_VALUE_DISPLAY_LENGTH,	50,
			0);

filerframe.apply_button = (Panel_item) xv_create(filerframe.panel, PANEL_BUTTON,
			PANEL_NEXT_ROW,		-1,
			XV_X,			200,
			PANEL_LABEL_STRING,	"Apply",
			0);

filerframe.cancel_button = (Panel_item) xv_create(filerframe.panel,PANEL_BUTTON,
			PANEL_LABEL_STRING,	"Cancel",
			0);

xv_set(filerframe.panel, PANEL_DEFAULT_ITEM, filerframe.apply_button, 0);
window_fit(filerframe.panel);
window_fit(filerframe.frame);
}

/********************************************************************************/
