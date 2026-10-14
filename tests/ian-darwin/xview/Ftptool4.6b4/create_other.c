#include "ftptool.h"

#pragma ident   "@(#)create_other.c 1.12     93/11/05"

#ifdef USE_PROTOTYPES
void create_host_popup(void)
#else
void create_host_popup()
#endif
{
	struct passwd *pwd;
	char	domainname[MAXHOSTNAMELEN + 1];
	char	*sundomain;
	Rect	rect;
	int		width, height, x, y;
	Menu	host_menu;
	Menu	host_list_menu;
	Panel	message;

	XSync(dpy, False);
	frame_get_rect(base_window.frame, &rect);

	host_window.frame = (Frame)xv_create(base_window.frame,
		FRAME_CMD,
		XV_LABEL, "Ftptool: Host Information",
		FRAME_SHOW_RESIZE_CORNER, TRUE,
		0);

	host_window.panel = xv_get(host_window.frame, FRAME_CMD_PANEL);
	xv_set(host_window.panel,
		XV_HELP_DATA, "ftptool:HostWindow",
		0);

	host_window.new = xv_create(host_window.panel,
		PANEL_BUTTON,
		PANEL_LABEL_STRING, "New",
		PANEL_NOTIFY_PROC, host_list_clean_proc,
		0);

	host_window.anonymous = xv_create(host_window.panel,
		PANEL_BUTTON,
		PANEL_LABEL_STRING, "Anonymous",
		PANEL_NOTIFY_PROC, host_list_clean_proc,
		0);

	host_menu = xv_create(XV_NULL,
		MENU,
#ifdef PIN_HOST_LIST
		MENU_GEN_PIN_WINDOW, base_window.frame, "Hosts",
#endif
		MENU_TITLE_ITEM, "Hosts",
		MENU_ITEM,
			MENU_STRING, "No Hosts!",
			0,
		0);

	host_window.hosts = xv_create(host_window.panel,
		PANEL_BUTTON,
		PANEL_LABEL_STRING, "Hosts",
		PANEL_ITEM_MENU, host_menu,
		XV_HELP_DATA, "ftptool:HostsButton",
		0);

#ifdef LINT
	host_list_menu = 0;
	host_list_menu = host_list_menu;
#else
	host_list_menu = xv_create(XV_NULL,
		MENU,
		MENU_GEN_PROC, host_menu_gen,
		MENU_ITEM,
			MENU_STRING, "Save",
			MENU_NOTIFY_PROC, host_save_proc,
			XV_HELP_DATA, "ftptool:HostSaveHostList",
			0,
		MENU_ITEM,
			MENU_STRING, "Load",
			MENU_NOTIFY_PROC, host_load_proc,
			XV_HELP_DATA, "ftptool:HostLoadHostList",
			0,
		MENU_ITEM,
			MENU_STRING, "Append .netrc",
			MENU_NOTIFY_PROC, host_append_netrc_proc,
			XV_HELP_DATA, "ftptool:HostAppendNetRC",
			0,
		MENU_ITEM,
			MENU_STRING, "Add",
			MENU_NOTIFY_PROC, host_list_add_proc,
			XV_HELP_DATA, "ftptool:HostListAdd",
			0,
		MENU_ITEM,
			MENU_STRING, "Change",
			MENU_NOTIFY_PROC, host_list_change_proc,
			XV_HELP_DATA, "ftptool:HostListChange",
			0,
		MENU_ITEM,
			MENU_STRING, "Delete",
			MENU_NOTIFY_PROC, host_list_delete_proc,
			XV_HELP_DATA, "ftptool:HostListDelete",
			0,
		0);
#endif

	host_window.host_list_ops = xv_create(host_window.panel,
		PANEL_BUTTON,
		PANEL_LABEL_STRING, "Host List",
		PANEL_ITEM_MENU, host_list_menu,
		XV_HELP_DATA, "ftptool:HostListOptions",
		0);

	xv_set(host_window.panel,
		PANEL_LAYOUT, PANEL_VERTICAL,
		0);

	window_fit_height(host_window.panel);

	host_window.basic.panel = xv_create(host_window.frame,
		PANEL,
		PANEL_LAYOUT, PANEL_VERTICAL,
		WIN_BORDER, TRUE,
		XV_SHOW, TRUE,
		0);

	host_window.basic.list = xv_create(host_window.basic.panel,
		PANEL_LIST,
		PANEL_LIST_DISPLAY_ROWS, 4,
		PANEL_LABEL_STRING, "Hosts:",
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		PANEL_CHOOSE_ONE, TRUE,
		PANEL_CHOOSE_NONE, TRUE,
		PANEL_READ_ONLY, TRUE,
		PANEL_NOTIFY_PROC, host_list_proc,
		XV_HELP_DATA, "ftptool:HostList",
		0);

	host_window.basic.host = xv_create(host_window.basic.panel,
		PANEL_TEXT,
		PANEL_NEXT_ROW, xv_row(host_window.basic.panel, 1),
		PANEL_VALUE_DISPLAY_LENGTH, 25,
		PANEL_VALUE_STORED_LENGTH, MAXHOSTNAMELEN,
		PANEL_LABEL_STRING, "Remote host:",
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		PANEL_VALUE, DEFAULT_HOST,
		PANEL_READ_ONLY, FALSE,
		XV_HELP_DATA, "ftptool:HostWindowHostname",
		0);

	host_window.basic.login = xv_create(host_window.basic.panel,
		PANEL_TEXT,
		PANEL_VALUE_DISPLAY_LENGTH, 25,
		PANEL_VALUE_STORED_LENGTH, MAXLOGINLEN,
		PANEL_LABEL_STRING, "Login:",
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		PANEL_VALUE, "anonymous",
		PANEL_READ_ONLY, FALSE,
		PANEL_NOTIFY_STRING, " ",
		PANEL_NOTIFY_LEVEL, PANEL_SPECIFIED,
		PANEL_NOTIFY_PROC, reject_spaces,
		XV_HELP_DATA, "ftptool:HostWindowLogin",
		0);

	pwd = getpwuid(getuid());
	if (pwd == 0) {
		fprintf(stderr, "Who are you?\n");
		login_name = "unknown";
	} else {
		login_name = strdup(pwd->pw_name);
		if (login_name == 0) {
			fprintf(stderr, "Out of memory.\n");
			login_name = "unknown";
		}
	}

#ifdef SYSV
	if (sysinfo(SI_SRPC_DOMAIN, domainname, MAXHOSTNAMELEN) == -1) {
#else
	if (getdomainname(domainname, MAXHOSTNAMELEN) == -1) {
#endif
		fprintf(stderr, "What domain is this?\n");
		strcpy(domainname, "unknown");
	}

#ifdef FIX_DOMAIN
	sundomain = index(domainname, '.');
	if (sundomain != 0)
		sundomain++; /* assume domain XX.Domain.Sun.COM */
	else
		sundomain = domainname;
#else
	sundomain = domainname;
#endif


	if (anonftp_password == 0) {
		if (index(myhostname, '.'))
			sprintf(scratch, "%s@%s", login_name, myhostname);
		else
			sprintf(scratch, "%s@%s.%s", login_name, myhostname,
			    sundomain);

		anonftp_password = strdup(scratch);
	} else {
		strcpy(scratch, anonftp_password);
	}

	footer_message("Initial password for ftp is %s.", scratch);

	host_window.basic.password = xv_create(host_window.basic.panel,
		PANEL_TEXT,
		PANEL_VALUE_DISPLAY_LENGTH, 25,
		PANEL_VALUE_STORED_LENGTH, MAXPASSWORDLEN,
		PANEL_LABEL_STRING, "Password:",
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		PANEL_VALUE, anonftp_password,
		PANEL_READ_ONLY, FALSE,
		PANEL_NOTIFY_LEVEL, PANEL_SPECIFIED,
		PANEL_NOTIFY_PROC, connect_proc,
		PANEL_MASK_CHAR, '*',
		XV_HELP_DATA, "ftptool:HostWindowPassword",
		0);

	host_window.basic.account = xv_create(host_window.basic.panel,
		PANEL_TEXT,
		PANEL_VALUE_DISPLAY_LENGTH, 25,
		PANEL_VALUE_STORED_LENGTH, MAXPASSWORDLEN,
		PANEL_LABEL_STRING, "Account:",
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		PANEL_VALUE, "",
		PANEL_READ_ONLY, FALSE,
		PANEL_NOTIFY_LEVEL, PANEL_SPECIFIED,
		PANEL_MASK_CHAR, '*',
		PANEL_NOTIFY_PROC, connect_proc,
		XV_HELP_DATA, "ftptool:HostWindowAccount",
		0);

	window_fit_width(host_window.basic.panel);

	host_window.basic.connect = xv_create(host_window.basic.panel,
		PANEL_BUTTON,
		PANEL_LABEL_STRING, " Connect  ",
		PANEL_NOTIFY_PROC, connect_proc,
		XV_HELP_DATA, "ftptool:HostWindowConnectButton",
		0);

	xv_set(host_window.basic.panel,
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		0);

	host_window.basic.dismiss = xv_create(host_window.basic.panel,
		PANEL_BUTTON,
		PANEL_LABEL_STRING, "Dismiss",
		PANEL_NOTIFY_PROC, dismiss_host_window,
		XV_SHOW, openlook_mode ? FALSE : TRUE,
		XV_HELP_DATA, "ftptool:DismissButton",
		0);

	host_window.basic.plus = xv_create(host_window.basic.panel,
		PANEL_BUTTON,
		PANEL_LABEL_STRING, "+",
		PANEL_NOTIFY_PROC, plus_proc,
		XV_HELP_DATA, "ftptool:PlusButton",
		0);

	xv_set(host_window.basic.panel,
		PANEL_DEFAULT_ITEM, host_window.basic.connect,
		0);

	xv_set(host_window.panel,
		PANEL_LAYOUT, PANEL_VERTICAL,
		0);

	host_window.advanced.panel = xv_create(host_window.frame,
		PANEL,
		PANEL_LAYOUT, PANEL_VERTICAL,
		XV_SHOW, FALSE,
		0);

	host_window.advanced.alias = xv_create(host_window.advanced.panel,
		PANEL_TEXT,
		PANEL_VALUE_DISPLAY_LENGTH, 25,
		PANEL_VALUE_STORED_LENGTH, MAXALIASLEN,
		PANEL_LABEL_STRING, "Alias:",
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		PANEL_VALUE, DEFAULT_ALIAS,
		XV_HELP_DATA, "ftptool:HostWindowAlias",
		PANEL_READ_ONLY, FALSE,
		0);

	message = xv_create(host_window.advanced.panel,
		PANEL_MESSAGE,
		PANEL_LABEL_STRING, "Last Visited:",
		PANEL_LABEL_BOLD, TRUE,
		XV_HELP_DATA, "ftptool:HostWindowLastVisited",
		0);

	host_window.advanced.comment = xv_create(host_window.advanced.panel,
		PANEL_TEXT,
		PANEL_VALUE_DISPLAY_LENGTH, 25,
		PANEL_VALUE_STORED_LENGTH, MAXCOMMENTLEN,
		PANEL_LABEL_STRING, "Comment:",
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		PANEL_VALUE, DEFAULT_COMMENT,
		PANEL_READ_ONLY, FALSE,
		XV_HELP_DATA, "ftptool:HostWindowComment",
		0);

	/* Make sure the values match the defines in readdir.h */
	host_window.advanced.os_type = xv_create(host_window.advanced.panel,
		PANEL_CHOICE_STACK,
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		PANEL_LABEL_STRING, "Remote host runs:",
		PANEL_CHOICE_STRINGS,
			"UNIX",
			"VMS",
			"DOS",
			"Other",
			0,
		PANEL_VALUE, 0,
		PANEL_NOTIFY_PROC, remote_os_proc,
		XV_HELP_DATA, "ftptool:RemoteOSType",
		0);

	host_window.advanced.dir_parse = xv_create(host_window.advanced.panel,
		PANEL_TEXT,
		PANEL_VALUE_DISPLAY_LENGTH, 25,
		PANEL_VALUE_STORED_LENGTH, MAXPATHLEN,
		PANEL_LABEL_STRING, "DIR Template:",
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		PANEL_VALUE, UNIX_DIR_PATTERN,
		PANEL_READ_ONLY, FALSE,
		XV_HELP_DATA, "ftptool:HostWindowDirTemplate",
		0);

	host_window.advanced.proxy = xv_create(host_window.advanced.panel,
		PANEL_TEXT,
		PANEL_VALUE_DISPLAY_LENGTH, 25,
		PANEL_VALUE_STORED_LENGTH, MAXHOSTNAMELEN,
		PANEL_LABEL_STRING, "Proxy host:",
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		PANEL_VALUE, DEFAULT_PROXY,
		PANEL_READ_ONLY, FALSE,
		PANEL_NOTIFY_STRING, " ",
		PANEL_NOTIFY_LEVEL, PANEL_SPECIFIED,
		PANEL_NOTIFY_PROC, reject_spaces,
		XV_HELP_DATA, "ftptool:HostWindowProxy",
		0);


	host_window.advanced.transfer_mode =
	    xv_create(host_window.advanced.panel,
		PANEL_CHOICE,
		PANEL_LABEL_STRING, "Transfer mode:",
		PANEL_CHOOSE_ONE, TRUE,
		PANEL_CHOOSE_NONE, FALSE,
		PANEL_CHOICE_STRINGS,
			"Binary",
			"ASCII",
			"Tenex",
			/*
			"Image",
			"EBCDIC",
			 */
			0,
		XV_HELP_DATA, "ftptool:TransferMode",
		0);

	host_window.advanced.remote_auto_cd =
	    xv_create(host_window.advanced.panel,
		PANEL_TEXT,
		PANEL_VALUE_DISPLAY_LENGTH, 25,
		PANEL_VALUE_STORED_LENGTH, MAXPATHLEN,
		PANEL_LABEL_STRING, "Remote CD to:",
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		PANEL_VALUE, ".",
		PANEL_READ_ONLY, FALSE,
		PANEL_NOTIFY_STRING, " ",
		PANEL_NOTIFY_LEVEL, PANEL_SPECIFIED,
		PANEL_NOTIFY_PROC, reject_spaces,
		XV_HELP_DATA, "ftptool:HostWindowDirectory",
		0);


	host_window.advanced.local_auto_cd =
	    xv_create(host_window.advanced.panel,
		PANEL_TEXT,
		PANEL_VALUE_DISPLAY_LENGTH, 25,
		PANEL_VALUE_STORED_LENGTH, MAXPATHLEN,
		PANEL_LABEL_STRING, "Local CD to:",
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		PANEL_VALUE, ".",
		PANEL_READ_ONLY, FALSE,
		PANEL_NOTIFY_STRING, " ",
		PANEL_NOTIFY_LEVEL, PANEL_SPECIFIED,
		PANEL_NOTIFY_PROC, reject_spaces,
		XV_HELP_DATA, "ftptool:HostWindowLocalDirectory",
		0);

	host_window.advanced.minus = xv_create(host_window.advanced.panel,
		PANEL_BUTTON,
		PANEL_LABEL_STRING, "-",
		PANEL_NOTIFY_PROC, minus_proc,
		XV_HELP_DATA, "ftptool:MinusButton",
		0);

	xv_set(host_window.panel,
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		0);


	/*
	window_fit(host_window.panel);
	justify_items(host_window.panel, TRUE);
	 */

	/*
	window_fit(host_window.basic.panel);
	 */
	justify_items(host_window.basic.panel, TRUE);

	window_fit_width(host_window.advanced.panel);
	justify_items(host_window.advanced.panel, TRUE);

	xv_set(host_window.advanced.dir_parse,
		XV_SHOW, FALSE,
		0);

	xv_set(host_window.advanced.panel,
		WIN_FIT_HEIGHT, 20,
		XV_X, (int)xv_get(host_window.advanced.panel, XV_X) - 3,
		0);

	/*
	height = (int)xv_get(host_window.advanced.panel, XV_HEIGHT);
	xv_set(host_window.basic.panel,
		XV_HEIGHT, height,
		0);
	 */

	y = (int)xv_get(host_window.advanced.minus, XV_Y);
	x = (int)xv_get(host_window.basic.panel, XV_WIDTH)
		- (int)xv_get(host_window.basic.plus, XV_WIDTH) - 5;

	xv_set(host_window.basic.connect,
		XV_Y, y,
		0);
	xv_set(host_window.basic.dismiss,
		XV_Y, y,
		0);
	xv_set(host_window.basic.plus,
		XV_Y, y,
		XV_X, x - 2,
		0);

	xv_set(host_window.basic.panel,
		WIN_FIT_HEIGHT, 20,
		0);


	host_window.advanced.last_visited =
	    xv_create(host_window.advanced.panel,
		PANEL_MESSAGE,
		PANEL_ITEM_X, xv_get(message, XV_X) + xv_get(message, XV_WIDTH)
		    + xv_col(host_window.advanced.panel, 1) / 2,
		PANEL_ITEM_Y, xv_get(message, XV_Y),
		PANEL_LABEL_STRING, "Never",
		XV_HELP_DATA, "ftptool:HostWindowLastVisited",
		0);

	/*
	window_fit_height(host_window.panel);
	*/

	window_fit(host_window.frame);

	xv_set(host_window.advanced.panel,
		XV_WIDTH, WIN_EXTEND_TO_EDGE,
		0);

	x = rect.r_left - 347;
	y = rect.r_top  + 100;
	width = xv_get(host_window.frame, XV_WIDTH);
	height = xv_get(host_window.frame, XV_HEIGHT);

	set_geometry(host_window.geometry, host_window.frame,
	    width, height, x, y);

	add_dismiss(host_window.basic.panel, host_window.basic.connect,
		host_window.basic.dismiss);

	minus_proc();

	XFlush(dpy);

	hostlist_head = new_hostlist();
	read_ftptoolrc();
	reload_host_list_menu(hostlist_head);
	if (hostlist_head->next) {
		struct hostlist *tmp = hostlist_head->next;

		/* default to first entry */
		if (try_proxy) {
			xv_set(host_window.advanced.proxy,
				PANEL_VALUE, tmp->proxy,
				XV_SHOW, TRUE,
				0);
		} else {
			xv_set(host_window.advanced.proxy,
				XV_SHOW, FALSE,
				0);
		}
		xv_set(host_window.advanced.alias,
			PANEL_VALUE, tmp->aliasname,
			0);
		xv_set(host_window.advanced.last_visited,
			PANEL_VALUE, tmp->last_visited,
			0);
		xv_set(host_window.advanced.comment,
			PANEL_VALUE, tmp->comment,
			0);
		xv_set(host_window.basic.host,
			PANEL_VALUE, tmp->host,
			0);
		xv_set(host_window.basic.login,
			PANEL_VALUE, tmp->login,
			0);
		xv_set(host_window.basic.password,
			PANEL_VALUE, tmp->password,
			0);
		xv_set(host_window.advanced.transfer_mode,
			PANEL_VALUE, tmp->transfer_mode,
			0);
		xv_set(host_window.advanced.remote_auto_cd,
			PANEL_VALUE, tmp->remote_directory,
			0);
		xv_set(host_window.advanced.local_auto_cd,
			PANEL_VALUE, tmp->local_directory,
			0);
		xv_set(host_window.advanced.os_type,
			PANEL_VALUE, tmp->os_type,
			0);
		xv_set(host_window.advanced.dir_parse,
			PANEL_VALUE, tmp->dir_parse,
			0);
		if (tmp->os_type == REMOTE_OS_OTHER) {
			xv_set(host_window.advanced.dir_parse,
				XV_SHOW, TRUE,
				0);
		} else {
			xv_set(host_window.advanced.dir_parse,
				XV_SHOW, FALSE,
				0);
		}
	} else {
		xv_set(host_window.advanced.proxy,
			XV_SHOW, FALSE,
			0);
	}
	fix_carets();
	xv_set(host_window.frame,
		WIN_EVENT_PROC, host_event_proc,
		0);

}

#ifdef USE_PROTOTYPES
void create_session_log(void)
#else
void create_session_log()
#endif
{
	Rect	rect;
	int		width, height, x, y;

	XSync(dpy, False);

	session_window.frame = xv_create(base_window.frame,
		FRAME_CMD,
		XV_LABEL, "Ftptool: Session Log",
		/*
		XV_WIDTH, rect.r_width,
		 */
		FRAME_SHOW_RESIZE_CORNER, TRUE,
#ifdef XVIEW3
		FRAME_CMD_PIN_STATE, FRAME_CMD_PIN_IN,
#else
		FRAME_CMD_PUSHPIN_IN, TRUE,
#endif
		0);

	session_window.panel = xv_get(session_window.frame, FRAME_CMD_PANEL);

	xv_set(session_window.panel,
		XV_HELP_DATA, "ftptool:SessionLog",
		0);

	window_fit_height(session_window.panel);

	session_window.log = xv_create(session_window.frame,
		TEXTSW,
		XV_HEIGHT, 100,
		TEXTSW_MEMORY_MAXIMUM, 200000,
		TEXTSW_DISABLE_LOAD, TRUE,
		TEXTSW_READ_ONLY, TRUE,
		0);

	height = xv_get(session_window.frame, XV_HEIGHT);

	frame_get_rect(local_window.frame, &rect);

	x = rect.r_left;
	y = rect.r_top + rect.r_height;
	rect.r_top = y;
	rect.r_height = height;
	width = rect.r_width;
	frame_set_rect(session_window.frame, &rect);
	frame_set_rect(session_window.panel, &rect);

	set_geometry(session_window.geometry, session_window.frame,
	    width, height, x, y);

	window_fit(session_window.frame);
#ifdef notdef
	if (logging) {
		xv_set(session_window.frame,
			XV_SHOW, TRUE,
			0);
	}
#endif
}

#ifdef USE_PROTOTYPES
void create_about_window(void)
#else
void create_about_window()
#endif
{
	int		width = xv_get(base_window.frame, XV_WIDTH);
	int		height = xv_get(base_window.frame, XV_HEIGHT);
	Rect	*butrect;
	Server_image corner_glyph;

	about_window.frame = xv_create(base_window.frame,
		FRAME_CMD,
		XV_LABEL, "Ftptool: About Ftptool",
#ifdef XVIEW3
		FRAME_CMD_PIN_STATE, FRAME_CMD_PIN_IN,
#else
		FRAME_CMD_PUSHPIN_IN, TRUE,
#endif
		XV_WIDTH, width,
		XV_HEIGHT, height,
		0);

	about_window.panel = xv_get(about_window.frame, FRAME_CMD_PANEL);

	xv_set(about_window.panel,
		XV_HELP_DATA, "ftptool:AboutWindow",
		0);

	xv_set(about_window.panel,
		PANEL_LAYOUT, PANEL_VERTICAL,
		0);

	xv_create(about_window.panel,
		PANEL_MESSAGE,
		XV_X, 100,
		PANEL_LABEL_STRING, header_name,
		PANEL_LABEL_BOLD, TRUE,
		0);

	xv_create(about_window.panel,
		PANEL_MESSAGE,
		XV_X, 100,
		PANEL_LABEL_STRING, "Copyright 1991",
		PANEL_LABEL_BOLD, TRUE,
		0);

	xv_create(about_window.panel,
		PANEL_MESSAGE,
		XV_X, 100,
		PANEL_LABEL_STRING, "Mike Sullivan and Sun Microsystems",
		PANEL_LABEL_BOLD, TRUE,
		0);

	xv_create(about_window.panel,
		PANEL_MESSAGE,
		XV_X, 100,
		PANEL_LABEL_STRING, "All Rights Reserved",
		PANEL_LABEL_BOLD, TRUE,
		0);

	window_fit_height(about_window.panel);

	about_window.message = xv_create(about_window.frame,
		TEXTSW,
		XV_X, 0,
		XV_WIDTH, WIN_EXTEND_TO_EDGE,
		XV_HEIGHT, 136,
		OPENWIN_SHOW_BORDERS, TRUE,
		TEXTSW_BROWSING, TRUE,
		TEXTSW_DISABLE_LOAD, TRUE,
		0);

	about_window.bottom_panel = xv_create(about_window.frame,
		PANEL,
		XV_X, 0,
		0);

	about_window.mail = xv_create(about_window.bottom_panel,
		PANEL_BUTTON,
		PANEL_LABEL_STRING, "Send Mail...",
		PANEL_NOTIFY_PROC, about_send_proc,
		0);

	xv_set(about_window.panel,
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		0);

	about_window.dismiss = xv_create(about_window.bottom_panel,
		PANEL_BUTTON,
		PANEL_LABEL_STRING, "Dismiss",
		PANEL_NOTIFY_PROC, dismiss_about_window,
		XV_HELP_DATA, "ftptool:DismissButton",
		0);

	xv_set(about_window.panel,
		PANEL_LAYOUT, PANEL_VERTICAL,
		0);

	window_fit_height(about_window.bottom_panel);

	butrect = (Rect *)xv_get(about_window.mail, XV_RECT);

	xv_set(about_window.mail,
		XV_X, (int)xv_get(about_window.bottom_panel, XV_WIDTH)/2
		    - butrect->r_width,
		0);


	window_fit(about_window.frame);

	corner_glyph = xv_create(XV_NULL, SERVER_IMAGE,
		XV_WIDTH, 64,
		XV_HEIGHT, 64,
		SERVER_IMAGE_DEPTH, 1,
		SERVER_IMAGE_BITS, icon_array,
		0);

	xv_create(about_window.panel, PANEL_MESSAGE,
		XV_X, 2,
		XV_Y, xv_get(about_window.panel, XV_HEIGHT)/2 - 32,
		PANEL_LABEL_IMAGE, corner_glyph,
		0);

	add_dismiss(about_window.panel, about_window.mail,
		about_window.dismiss);
}

#ifdef USE_PROTOTYPES
void create_feedback_window(void)
#else
void create_feedback_window()
#endif
{
	int		width = xv_get(base_window.frame, XV_WIDTH);
	int		height = xv_get(base_window.frame, XV_HEIGHT);
	Rect	*butrect;

	feedback_window.frame = xv_create(base_window.frame,
		FRAME_CMD,
		XV_LABEL, "Ftptool: Send Feedback",
#ifdef XVIEW3
		FRAME_CMD_PIN_STATE, FRAME_CMD_PIN_OUT,
#else
		FRAME_CMD_PUSHPIN_IN, FALSE,
#endif
		XV_WIDTH, width,
		XV_HEIGHT, height,
		0);

	feedback_window.panel = xv_get(feedback_window.frame, FRAME_CMD_PANEL);

	xv_set(feedback_window.panel,
		XV_HELP_DATA, "ftptool:FeedbackWindow",
		0);

	xv_set(feedback_window.panel,
		PANEL_LAYOUT, PANEL_VERTICAL,
		0);

	xv_create(feedback_window.panel,
		PANEL_MESSAGE,
		PANEL_LABEL_STRING,
		    "Your feedback is appreciated. To insure that your message arrives,",
		0);

	xv_create(feedback_window.panel,
		PANEL_MESSAGE,
		PANEL_LABEL_STRING,
		    "check that the \"To\" address is valid for your site before clicking \"Send.\"",
		0);

	feedback_window.which = xv_create(feedback_window.panel,
		PANEL_CHOICE_STACK,
		PANEL_CHOICE_NROWS, 1,
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		PANEL_LABEL_STRING, "To:",
		PANEL_CHOICE_STRINGS,
			"Mike.Sullivan@Sun.COM",
			"uunet!sun.com!mike.sullivan%male",
			"Other Address",
			0,
		PANEL_NOTIFY_PROC, feedback_address_proc,
		0);

	feedback_window.other = xv_create(feedback_window.panel, PANEL_TEXT,
		PANEL_LABEL_STRING, "Other:  ",
		PANEL_LABEL_BOLD, TRUE,
		PANEL_VALUE_DISPLAY_LENGTH, 40,
		PANEL_VALUE_STORED_LENGTH, 64,
		PANEL_VALUE, "",
		PANEL_READ_ONLY, FALSE,
		XV_SHOW, FALSE,
		0);

	window_fit_height(feedback_window.panel);

	feedback_window.feedback = xv_create(feedback_window.frame,
		TEXTSW,
		XV_X, 0,
		XV_WIDTH, WIN_EXTEND_TO_EDGE,
		XV_HEIGHT, 188,
		OPENWIN_SHOW_BORDERS, TRUE,
		0);

	feedback_window.bottom_panel = xv_create(feedback_window.frame,
		PANEL,
		XV_X, 0,
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		0);

	feedback_window.send = xv_create(feedback_window.bottom_panel,
		PANEL_BUTTON,
		PANEL_LABEL_STRING, "Send",
		PANEL_NOTIFY_PROC, feedback_send_proc,
		0);

	window_fit_height(feedback_window.bottom_panel);

	butrect = (Rect *)xv_get(feedback_window.send, XV_RECT);

	xv_set(feedback_window.send,
		XV_X, (int)xv_get(feedback_window.bottom_panel, XV_WIDTH)/2
		    - butrect->r_width,
		0);

	feedback_window.cancel = xv_create(feedback_window.bottom_panel,
		PANEL_BUTTON,
		PANEL_LABEL_STRING, "Cancel",
		PANEL_NOTIFY_PROC, feedback_cancel_proc,
		0);


	window_fit(feedback_window.frame);
}

#ifdef USE_PROTOTYPES
void create_file_property_window(struct file_property_window *file_props,
	char *header)
#else
void create_file_property_window(file_props, header)
struct file_property_window *file_props;
char	*header;
#endif
{
	int x_gap;
	int y_gap;
	Rect	rect;
	Rect	*butrect;

	file_props->frame = xv_create(base_window.frame,
		FRAME_PROPS,
		XV_LABEL, header,
#ifdef XVIEW3
		FRAME_CMD_PIN_STATE, FRAME_CMD_PIN_IN,
#else
		FRAME_CMD_PUSHPIN_IN, TRUE,
#endif
		0);

	file_props->panel = (Panel)xv_get(file_props->frame, FRAME_PROPS_PANEL);

	x_gap = xv_get(file_props->panel, PANEL_ITEM_X_GAP);

	y_gap = xv_get(file_props->panel, PANEL_ITEM_Y_GAP);

	xv_set(file_props->panel,
		PANEL_LAYOUT, PANEL_VERTICAL,
		XV_HELP_DATA, "ftptool:FileProperty",
		0);

	file_props->filename = xv_create(file_props->panel,
		PANEL_MESSAGE,
		PANEL_LABEL_STRING, "Name:",
		PANEL_LABEL_BOLD, TRUE,
		XV_HELP_DATA, "ftptool:FilePropertyName",
		0);

	file_props->owner = xv_create(file_props->panel,
		PANEL_MESSAGE,
		PANEL_LABEL_STRING, "Owner:",
		PANEL_LABEL_BOLD, TRUE,
		XV_HELP_DATA, "ftptool:FilePropertyOwner",
		0);

	file_props->group = xv_create(file_props->panel,
		PANEL_MESSAGE,
		PANEL_LABEL_STRING, "Group:",
		PANEL_LABEL_BOLD, TRUE,
		XV_HELP_DATA, "ftptool:FilePropertyGroup",
		0);

	file_props->modtime = xv_create(file_props->panel,
		PANEL_MESSAGE,
		PANEL_LABEL_STRING, "Last Modified:",
		PANEL_LABEL_BOLD, TRUE,
		XV_HELP_DATA, "ftptool:FilePropertyModtime",
		0);

	file_props->size = xv_create(file_props->panel,
		PANEL_MESSAGE,
		PANEL_LABEL_STRING, "Size:",
		PANEL_LABEL_BOLD, TRUE,
		XV_HELP_DATA, "ftptool:FilePropertySize",
		0);

	file_props->type = xv_create(file_props->panel,
		PANEL_MESSAGE,
		PANEL_LABEL_STRING, "Type:",
		PANEL_LABEL_BOLD, TRUE,
		XV_HELP_DATA, "ftptool:FilePropertyType",
		0);

	file_props->perms_message = xv_create(file_props->panel,
		PANEL_MESSAGE,
		PANEL_NEXT_ROW, xv_row(file_props->panel, 1),
		PANEL_LABEL_BOLD, TRUE,
		PANEL_LABEL_STRING, "Permissions:",
		0);

	file_props->user_perms = xv_create(file_props->panel,
		PANEL_MESSAGE,
		PANEL_LABEL_STRING, "Owner:",
		PANEL_LABEL_BOLD, TRUE,
		XV_HELP_DATA, "ftptool:FilePropertyUserPerms",
		0);

	file_props->group_perms = xv_create(file_props->panel,
		PANEL_MESSAGE,
		PANEL_LABEL_STRING, "Group:",
		PANEL_LABEL_BOLD, TRUE,
		XV_HELP_DATA, "ftptool:FilePropertyGroupPerms",
		0);

	file_props->other_perms = xv_create(file_props->panel,
		PANEL_MESSAGE,
		PANEL_LABEL_STRING, "Other:",
		PANEL_LABEL_BOLD, "Other:",
		XV_HELP_DATA, "ftptool:FilePropertyOtherPerms",
		0);

	file_props->dismiss = xv_create(file_props->panel,
		PANEL_BUTTON,
		PANEL_LABEL_STRING, "Dismiss",
		PANEL_NOTIFY_PROC, dismiss_file_props_window,
		XV_SHOW, openlook_mode ? FALSE : TRUE,
		PANEL_CLIENT_DATA, file_props->frame,
		XV_HELP_DATA, "ftptool:DismissButton",
		0);

	window_fit(file_props->panel);
	justify_items(file_props->panel, FALSE);

	file_props->filename = xv_create(file_props->panel,
		PANEL_TEXT,
		PANEL_VALUE_DISPLAY_LENGTH, 25,
		PANEL_VALUE_STORED_LENGTH, MAXHOSTNAMELEN,
		PANEL_READ_ONLY, TRUE,
		PANEL_ITEM_X, xv_get(file_props->filename, PANEL_ITEM_X)
		    + xv_get(file_props->filename, XV_WIDTH) + x_gap,
		PANEL_ITEM_Y, xv_get(file_props->filename, PANEL_ITEM_Y),
		XV_HELP_DATA, "ftptool:FilePropertyName",
		0);

	xv_create(file_props->panel,
		PANEL_MESSAGE,
		PANEL_LABEL_BOLD, TRUE,
		PANEL_LABEL_STRING, "Read  Write Execute",
		PANEL_ITEM_X, xv_get(file_props->perms_message, PANEL_ITEM_X)
		    + xv_get(file_props->perms_message, XV_WIDTH) + x_gap,
		PANEL_ITEM_Y, xv_get(file_props->perms_message, PANEL_ITEM_Y),
		0);

	file_props->owner = xv_create(file_props->panel,
		PANEL_MESSAGE,
		PANEL_INACTIVE, TRUE,
		PANEL_READ_ONLY, TRUE,
		PANEL_ITEM_X, xv_get(file_props->owner, PANEL_ITEM_X)
		    + xv_get(file_props->owner, XV_WIDTH) + x_gap,
		PANEL_ITEM_Y, xv_get(file_props->owner, PANEL_ITEM_Y),
		XV_HELP_DATA, "ftptool:FilePropertyOwner",
		0);

	file_props->group = xv_create(file_props->panel,
		PANEL_MESSAGE,
		PANEL_INACTIVE, TRUE,
		PANEL_READ_ONLY, TRUE,
		PANEL_ITEM_X, xv_get(file_props->group, PANEL_ITEM_X)
		    + xv_get(file_props->group, XV_WIDTH) + x_gap,
		PANEL_ITEM_Y, xv_get(file_props->group, PANEL_ITEM_Y),
		XV_HELP_DATA, "ftptool:FilePropertyGroup",
		0);

	file_props->modtime = xv_create(file_props->panel,
		PANEL_MESSAGE,
		PANEL_INACTIVE, TRUE,
		PANEL_READ_ONLY, TRUE,
		PANEL_ITEM_X, xv_get(file_props->modtime, PANEL_ITEM_X)
		    + xv_get(file_props->modtime, XV_WIDTH) + x_gap,
		PANEL_ITEM_Y, xv_get(file_props->modtime, PANEL_ITEM_Y),
		XV_HELP_DATA, "ftptool:FilePropertyModtime",
		0);

	file_props->size = xv_create(file_props->panel,
		PANEL_MESSAGE,
		PANEL_INACTIVE, TRUE,
		PANEL_READ_ONLY, TRUE,
		PANEL_ITEM_X, xv_get(file_props->size, PANEL_ITEM_X)
		    + xv_get(file_props->size, XV_WIDTH) + x_gap,
		PANEL_ITEM_Y, xv_get(file_props->size, PANEL_ITEM_Y),
		XV_HELP_DATA, "ftptool:FilePropertySize",
		0);

	file_props->type = xv_create(file_props->panel,
		PANEL_MESSAGE,
		PANEL_INACTIVE, TRUE,
		PANEL_READ_ONLY, TRUE,
		PANEL_ITEM_X, xv_get(file_props->type, PANEL_ITEM_X)
		    + xv_get(file_props->type, XV_WIDTH) + x_gap,
		PANEL_ITEM_Y, xv_get(file_props->type, PANEL_ITEM_Y),
		XV_HELP_DATA, "ftptool:FilePropertyType",
		0);

	file_props->user_perms = xv_create(file_props->panel,
		PANEL_CHECK_BOX,
		PANEL_INACTIVE, TRUE,
		PANEL_CHOICE_STRINGS,
			"",
			"",
			"",
			0,
		PANEL_ITEM_X, xv_get(file_props->user_perms, PANEL_ITEM_X)
		    + xv_get(file_props->user_perms, XV_WIDTH) + x_gap * 2,
		PANEL_ITEM_Y, xv_get(file_props->user_perms, PANEL_ITEM_Y) -
		    y_gap/2,
		XV_HELP_DATA, "ftptool:FilePropertyUserPerms",
		0);

	file_props->group_perms = xv_create(file_props->panel,
		PANEL_CHECK_BOX,
		PANEL_INACTIVE, TRUE,
		PANEL_CHOICE_STRINGS,
			"",
			"",
			"",
			0,
		PANEL_ITEM_X, xv_get(file_props->group_perms, PANEL_ITEM_X)
		    + xv_get(file_props->group_perms, XV_WIDTH) + x_gap * 2,
		PANEL_ITEM_Y, xv_get(file_props->group_perms, PANEL_ITEM_Y) -
		    y_gap/2,
		XV_HELP_DATA, "ftptool:FilePropertyGroupPerms",
		0);

	file_props->other_perms = xv_create(file_props->panel,
		PANEL_CHECK_BOX,
		PANEL_INACTIVE, TRUE,
		PANEL_CHOICE_STRINGS,
			"",
			"",
			"",
			0,
		PANEL_ITEM_X, xv_get(file_props->other_perms, PANEL_ITEM_X)
		    + xv_get(file_props->other_perms, XV_WIDTH) + x_gap * 2,
		PANEL_ITEM_Y, xv_get(file_props->other_perms, PANEL_ITEM_Y) -
		    y_gap/2,
		XV_HELP_DATA, "ftptool:FilePropertyOtherPerms",
		0);

	window_fit(file_props->panel);
	window_fit(file_props->frame);

	XSync(dpy, False);

	frame_get_rect(file_props->frame, &rect);

	butrect = (Rect *)xv_get(file_props->dismiss, XV_RECT);

	xv_set(file_props->dismiss,
		XV_X, rect.r_width / 2 - butrect->r_width / 2,
		0);
}

#ifdef USE_PROTOTYPES
void create_tar_file_popup(void)
#else
void create_tar_file_popup()
#endif
{
	Panel_button_item ok;
	Rect	*butrect;
	Panel	panel;

	tar_frame = xv_create(base_window.frame,
		FRAME_CMD,
		XV_LABEL, "Ftptool: Create Tar File",
		0);

	panel = xv_get(tar_frame, FRAME_CMD_PANEL);

	xv_set(panel,
		PANEL_LAYOUT, PANEL_VERTICAL,
		XV_HELP_DATA, "ftptool:TarFileNameWindow",
		0);

	tar_text = xv_create(panel,
		PANEL_TEXT,
		PANEL_VALUE_DISPLAY_LENGTH, 20,
		PANEL_VALUE_STORED_LENGTH, MAXPATHLEN + 1,
		PANEL_LABEL_STRING, "Tar Filename: ",
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		XV_HELP_DATA, "ftptool:TarFileNameText",
		0);

	ok = xv_create(panel, PANEL_BUTTON,
		PANEL_NOTIFY_PROC, create_tar_proc,
		PANEL_LABEL_STRING, "Start Tar",
		XV_HELP_DATA, "ftptool:TarFileNameButton",
		0);

	xv_set(panel,
		PANEL_DEFAULT_ITEM, ok,
		0);

	window_fit(panel);
	window_fit(tar_frame);

	butrect = (Rect *)xv_get(ok, XV_RECT);

	xv_set(ok,
		XV_X, (int)xv_get(panel, XV_WIDTH)/2 - butrect->r_width,
		0);
}

#ifdef USE_PROTOTYPES
void create_load_save_popup(Frame *framep, Panel *textp, Panel *buttonp)
#else
void create_load_save_popup(framep, textp, buttonp)
Frame	*framep;
Panel	*textp;
Panel	*buttonp;
#endif
{
	Frame frame;
	Panel panel;
	Panel text;
	Panel button;
	Rect	*butrect;

	frame = xv_create(base_window.frame,
		FRAME_CMD,
		0);

	panel = xv_get(frame, FRAME_CMD_PANEL);

	xv_set(panel,
		PANEL_LAYOUT, PANEL_VERTICAL,
		0);

	text = xv_create(panel,
		PANEL_TEXT,
		PANEL_VALUE_DISPLAY_LENGTH, 40,
		PANEL_VALUE_STORED_LENGTH, MAXPATHLEN + 1,
		PANEL_LABEL_STRING, "Filename: ",
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		XV_HELP_DATA, "ftptool:LoadSaveBatchList",
		0);

	button = xv_create(panel,
		PANEL_BUTTON,
		PANEL_LABEL_STRING, "Load",
		0);

	xv_set(panel, PANEL_DEFAULT_ITEM, button, 0);

	window_fit(panel);
	window_fit(frame);

	butrect = (Rect *)xv_get(button, XV_RECT);
	xv_set(button,
		XV_X, (int)xv_get(panel, XV_WIDTH)/2 - butrect->r_width,
		0);

	*framep = frame;
	*textp = text;
	*buttonp = button;
}

#ifdef USE_PROTOTYPES
void create_schedule_window(void)
#else
void create_schedule_window()
#endif
{
	Menu	host_menu;
	Menu	options_menu;
	Menu	list_menu;
	Panel	message;
	int	x, y, width, height;

	schedule_window.frame = (Frame)xv_create(base_window.frame,
		FRAME_CMD,
		XV_LABEL, "Ftptool: Schedule Batch Transfer",
		FRAME_SHOW_RESIZE_CORNER, TRUE,
		FRAME_SHOW_FOOTER, TRUE,
		XV_HELP_DATA, "ftptool:ScheduleWindow",
		0);

	schedule_window.panel = xv_get(schedule_window.frame, FRAME_CMD_PANEL);

	xv_set(schedule_window.panel,
		PANEL_LAYOUT, PANEL_VERTICAL,
		0);

	schedule_window.process = xv_create(schedule_window.panel,
		PANEL_BUTTON,
		PANEL_LABEL_STRING, "Process Batch",
		PANEL_NOTIFY_PROC, batch_process_proc,
		XV_HELP_DATA, "ftptool:ProcessBatch",
		0);

	xv_set(schedule_window.panel,
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		0);

	host_menu = xv_create(XV_NULL,
		MENU,
		MENU_GEN_PIN_WINDOW, base_window.frame, "Hosts",
		MENU_TITLE_ITEM, "Hosts",
		MENU_ITEM,
			MENU_STRING, "No Hosts!",
			0,
		0);

	schedule_window.hosts = xv_create(schedule_window.panel,
		PANEL_BUTTON,
		PANEL_LABEL_STRING, "Hosts",
		PANEL_ITEM_MENU, host_menu,
		XV_HELP_DATA, "ftptool:ScheduleHosts",
		0);

#ifdef LINT
	options_menu = 0;
	options_menu = options_menu;
#else
	options_menu = xv_create(XV_NULL,
		MENU,
		MENU_ITEM,
			MENU_STRING, "Set Current",
			MENU_NOTIFY_PROC, set_current_schedule_proc,
			XV_HELP_DATA, "ftptool:ScheduleCurrent",
			0,
		MENU_ITEM,
			MENU_STRING, "Add",
			MENU_NOTIFY_PROC, schedule_add_proc,
			XV_HELP_DATA, "ftptool:ScheduleAdd",
			0,
		MENU_ITEM,
			MENU_STRING, "Change",
			MENU_NOTIFY_PROC, schedule_change_proc,
			XV_HELP_DATA, "ftptool:ScheduleChange",
			0,
		MENU_ITEM,
			MENU_STRING, "Delete",
			MENU_NOTIFY_PROC, schedule_delete_proc,
			XV_HELP_DATA, "ftptool:ScheduleDelete",
			0,
		0);
#endif

	schedule_window.options = xv_create(schedule_window.panel,
		PANEL_BUTTON,
		PANEL_LABEL_STRING, "Options",
		PANEL_ITEM_MENU, options_menu,
		XV_HELP_DATA, "ftptool:ScheduleOptions",
		0);


	schedule_window.dismiss = xv_create(schedule_window.panel,
		PANEL_BUTTON,
		PANEL_LABEL_STRING, "Dismiss",
		PANEL_NOTIFY_PROC, dismiss_schedule_window,
		XV_SHOW, openlook_mode ? FALSE : TRUE,
		XV_HELP_DATA, "ftptool:DismissButton",
		0);

	xv_set(schedule_window.panel,
		PANEL_LAYOUT, PANEL_VERTICAL,
		0);

	message = xv_create(schedule_window.panel,
		PANEL_MESSAGE,
		PANEL_LABEL_STRING, "Time Now:",
		PANEL_LABEL_BOLD, TRUE,
		XV_HELP_DATA, "ftptool:ScheduleTime",
		0);

	schedule_window.direction = xv_create(schedule_window.panel,
		PANEL_CHOICE,
		PANEL_LABEL_STRING, "Action:",
		PANEL_CHOOSE_ONE, TRUE,
		PANEL_CHOICE_STRINGS,
			"Receive From",
			"Send to",
			0,
		XV_HELP_DATA, "ftptool:ScheduleDirection",
		PANEL_NOTIFY_PROC, action_choice_proc,
		0);

	schedule_window.menu_name = xv_create(schedule_window.panel,
		PANEL_TEXT,
		PANEL_LABEL_STRING, "Target:",
		PANEL_VALUE, "",
		PANEL_READ_ONLY, TRUE,
		PANEL_VALUE_DISPLAY_LENGTH, 30,
		PANEL_VALUE_STORED_LENGTH, MAXHOSTNAMELEN,
		XV_HELP_DATA, "ftptool:ScheduleTarget",
		0);


	schedule_window.hour = xv_create(schedule_window.panel,
		PANEL_NUMERIC_TEXT,
		PANEL_MIN_VALUE, 0,
		PANEL_MAX_VALUE, 23,
		PANEL_VALUE_STORED_LENGTH, 20,
		PANEL_VALUE_DISPLAY_LENGTH, 2,
		PANEL_LABEL_STRING, "Hour:",
		XV_HELP_DATA, "ftptool:ScheduleHour",
		0);

	schedule_window.month = xv_create(schedule_window.panel,
		PANEL_CHOICE_STACK,
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		PANEL_LABEL_STRING, "Date:",
		PANEL_CHOICE_STRINGS,
			"Jan", "Feb", "Mar", "Apr",
			"May", "Jun", "Jul", "Aug",
			"Sep", "Oct", "Nov", "Dec",
			0,
		PANEL_VALUE, 0,
		XV_HELP_DATA, "ftptool:ScheduleMonth",
		0);

	schedule_window.repeat = xv_create(schedule_window.panel,
		PANEL_CHECK_BOX,
		PANEL_CHOICE_STRINGS,
			"",
			0,
		PANEL_LABEL_STRING, "Repeat:",
		PANEL_NOTIFY_PROC, repeat_check_box,
		XV_HELP_DATA, "ftptool:ScheduleRepeat",
		0);

	window_fit(schedule_window.panel);
	justify_items(schedule_window.panel, TRUE);

	schedule_window.receive_list = xv_create(schedule_window.panel,
		PANEL_LIST,
		PANEL_LIST_DISPLAY_ROWS, 6,
		PANEL_LAYOUT, PANEL_VERTICAL,
		PANEL_CHOOSE_NONE, TRUE,
		PANEL_CHOOSE_ONE, FALSE,
		PANEL_READ_ONLY, TRUE,
		PANEL_NOTIFY_PROC, receive_list_proc,
		XV_HELP_DATA, "ftptool:BatchReceiveList",
		0);

	list_menu = xv_get(schedule_window.receive_list, PANEL_ITEM_MENU);

	xv_set(list_menu,
		MENU_GEN_PROC, receive_list_menu_gen,
		MENU_TITLE_ITEM, "Batch Receive",
		MENU_ITEM,
			MENU_STRING, "",
			MENU_FEEDBACK, FALSE,
			0,
		MENU_ITEM,
			MENU_STRING, "Delete",
			MENU_NOTIFY_PROC, receive_list_delete_proc,
			XV_HELP_DATA, "ftptool:BatchReceiveDelete",
			0,
		MENU_ITEM,
			MENU_STRING, "Copy to Local",
			MENU_NOTIFY_PROC, batchget_proc,
			XV_HELP_DATA, "ftptool:BatchReceiveCopy",
			0,
		MENU_ITEM,
			MENU_STRING, "Load",
			MENU_NOTIFY_PROC, show_load_receive_list_proc,
			XV_HELP_DATA, "ftptool:BatchLoad",
			0,
		MENU_ITEM,
			MENU_STRING, "Save",
			MENU_NOTIFY_PROC, show_save_receive_list_proc,
			XV_HELP_DATA, "ftptool:BatchSave",
			0,
		0);

	schedule_window.send_list = xv_create(schedule_window.panel,
		PANEL_LIST,
		PANEL_LIST_DISPLAY_ROWS, 6,
		PANEL_LAYOUT, PANEL_VERTICAL,
		PANEL_CHOOSE_NONE, TRUE,
		PANEL_CHOOSE_ONE, FALSE,
		PANEL_READ_ONLY, TRUE,
		PANEL_ITEM_X, xv_get(schedule_window.receive_list, XV_X),
		PANEL_ITEM_Y, xv_get(schedule_window.receive_list, XV_Y),
		PANEL_NOTIFY_PROC, send_list_proc,
		XV_HELP_DATA, "ftptool:BatchSendList",
		XV_SHOW, FALSE,
		0);

	list_menu = xv_get(schedule_window.send_list, PANEL_ITEM_MENU);

	xv_set(list_menu,
		MENU_GEN_PROC, send_list_menu_gen,
		MENU_TITLE_ITEM, "Batch Send",
		MENU_ITEM,
			MENU_STRING, "",
			MENU_FEEDBACK, FALSE,
			0,
		MENU_ITEM,
			MENU_STRING, "Delete",
			MENU_NOTIFY_PROC, send_list_delete_proc,
			XV_HELP_DATA, "ftptool:BatchSendDelete",
			0,
		MENU_ITEM,
			MENU_STRING, "Copy to Remote",
			MENU_NOTIFY_PROC, batchput_proc,
			XV_HELP_DATA, "ftptool:BatchSendCopy",
			0,
		MENU_ITEM,
			MENU_STRING, "Load",
			MENU_NOTIFY_PROC, show_load_send_list_proc,
			XV_HELP_DATA, "ftptool:BatchLoad",
			0,
		MENU_ITEM,
			MENU_STRING, "Save",
			MENU_NOTIFY_PROC, show_save_send_list_proc,
			XV_HELP_DATA, "ftptool:BatchSave",
			0,
		0);

	xv_set(schedule_window.panel,
		PANEL_LAYOUT, PANEL_HORIZONTAL,
		0);


	schedule_window.current_time = xv_create(schedule_window.panel,
		PANEL_MESSAGE,
		PANEL_ITEM_X, xv_get(message, XV_X) + xv_get(message, XV_WIDTH)
		    + xv_col(schedule_window.panel, 1) / 2,
		PANEL_ITEM_Y, xv_get(message, XV_Y),
		PANEL_LABEL_STRING, "",
		XV_HELP_DATA, "ftptool:ScheduleTime",
		0);

	schedule_window.minute = xv_create(schedule_window.panel,
		PANEL_NUMERIC_TEXT,
		PANEL_VALUE_STORED_LENGTH, 20,
		PANEL_VALUE_DISPLAY_LENGTH, 2,
		PANEL_MIN_VALUE, 0,
		PANEL_MAX_VALUE, 59,
		PANEL_LABEL_STRING, "Minute:",
		PANEL_ITEM_X, xv_get(schedule_window.hour, XV_X)
		    + xv_get(schedule_window.hour, XV_WIDTH)
		    + xv_col(schedule_window.panel, 1),
		PANEL_ITEM_Y, xv_get(schedule_window.hour, XV_Y),
		XV_HELP_DATA, "ftptool:ScheduleMinute",
		0);

	schedule_window.day = xv_create(schedule_window.panel,
		PANEL_NUMERIC_TEXT,
		PANEL_VALUE_STORED_LENGTH, 20,
		PANEL_VALUE_DISPLAY_LENGTH, 2,
		PANEL_MIN_VALUE, 1,
		PANEL_MAX_VALUE, 31,
		PANEL_ITEM_X, xv_get(schedule_window.month, XV_X)
		    + xv_get(schedule_window.month, XV_WIDTH)
		    + xv_col(schedule_window.panel, 1),
		PANEL_ITEM_Y, xv_get(schedule_window.month, XV_Y)
		    + xv_row(schedule_window.panel, 1)/8,
		XV_HELP_DATA, "ftptool:ScheduleDay",
		0);

	schedule_window.year = xv_create(schedule_window.panel,
		PANEL_NUMERIC_TEXT,
		PANEL_VALUE_DISPLAY_LENGTH, 4,
		PANEL_VALUE_STORED_LENGTH, 40,
		PANEL_MIN_VALUE, 1992,
		PANEL_MAX_VALUE, 2000,
		PANEL_ITEM_X, xv_get(schedule_window.day, XV_X)
		    + xv_get(schedule_window.day, XV_WIDTH)
		    + xv_col(schedule_window.panel, 1),
		PANEL_ITEM_Y, xv_get(schedule_window.day, XV_Y),
		XV_HELP_DATA, "ftptool:ScheduleYear",
		0);

	schedule_window.repeat_minutes = xv_create(schedule_window.panel,
		PANEL_NUMERIC_TEXT,
		PANEL_VALUE_DISPLAY_LENGTH, 4,
		PANEL_VALUE_STORED_LENGTH, 40,
		PANEL_MIN_VALUE, 1,
		PANEL_MAX_VALUE, 100000,
		PANEL_VALUE, 1,
		PANEL_ITEM_X, xv_get(schedule_window.repeat, XV_X)
		    + xv_get(schedule_window.repeat, XV_WIDTH)
		    + xv_col(schedule_window.panel, 2),
		PANEL_ITEM_Y, xv_get(schedule_window.repeat, XV_Y) + 5,
		PANEL_NOTIFY_PROC, repeat_minute_check,
		XV_HELP_DATA, "ftptool:ScheduleRepeatMinutes",
		0);

	schedule_window.repeat_message = xv_create(schedule_window.panel,
		PANEL_MESSAGE,
		PANEL_LABEL_STRING, "minutes",
		PANEL_VALUE, 1,
		PANEL_ITEM_X, xv_get(schedule_window.repeat_minutes, XV_X)
		    + xv_get(schedule_window.repeat_minutes, XV_WIDTH)
		    + xv_col(schedule_window.panel, 1),
		PANEL_ITEM_Y, xv_get(schedule_window.repeat_minutes, XV_Y),
		XV_HELP_DATA, "ftptool:ScheduleRepeatMinutes",
		0);

	window_fit(schedule_window.panel);
	window_fit(schedule_window.frame);

	xv_set(schedule_window.repeat_minutes,
		XV_SHOW, FALSE,
		0);

	xv_set(schedule_window.repeat_message,
		XV_SHOW, FALSE,
		0);

	xv_set(schedule_window.frame,
		WIN_EVENT_PROC, schedule_event_proc,
		0);
	x = xv_get(schedule_window.frame, XV_X);
	y = xv_get(schedule_window.frame, XV_Y);
	width = xv_get(schedule_window.frame, XV_WIDTH);
	height = xv_get(schedule_window.frame, XV_HEIGHT);
	set_geometry(schedule_window.geometry, schedule_window.frame,
	    width, height, x, y);

	/* create load/save popup.  */
	create_load_save_popup(&schedule_window.lsframe,
	    &schedule_window.filename, &schedule_window.lsbutton);
}

#ifdef USE_PROTOTYPES
void create_status_window(void)
#else
void create_status_window()
#endif
{
	Rect	*butrect;
	Rect	rect;
	Panel_item	status_message;
	Panel_item	size_message;
	int		x;
	int		y;
	int		width;
	int		height;

	frame_get_rect(base_window.frame, &rect);

	status_window.frame = xv_create(base_window.frame,
		FRAME_CMD,
		XV_LABEL, "Ftptool: Transfer Status",
		XV_HELP_DATA, "ftptool:StatusWindow",
		FRAME_SHOW_FOOTER, TRUE,
		0);

	status_window.panel = xv_get(status_window.frame, FRAME_CMD_PANEL);

	xv_set(status_window.panel,
		PANEL_LAYOUT, PANEL_VERTICAL,
		WIN_BORDER, TRUE,
		0);

	status_message = xv_create(status_window.panel,
		PANEL_MESSAGE,
		PANEL_LABEL_STRING, "Status:",
		PANEL_LABEL_BOLD, TRUE,
		0);

	size_message = xv_create(status_window.panel,
		PANEL_MESSAGE,
		PANEL_LABEL_STRING, "Size:",
		PANEL_LABEL_BOLD, TRUE,
		0);

	status_window.total_gauge = xv_create(status_window.panel,
		PANEL_GAUGE,
		PANEL_NEXT_ROW, xv_row(status_window.panel, 1) / 2,
		PANEL_LABEL_STRING, "Total:",
		PANEL_LABEL_BOLD, TRUE,
		PANEL_MIN_VALUE, 0,
		PANEL_MAX_VALUE, 100,
		PANEL_GAUGE_WIDTH, 200,
		PANEL_TICKS, 21,
		XV_HELP_DATA, "ftptool:TotalGauge",
		0);

	status_window.dismiss = xv_create(status_window.panel,
		PANEL_BUTTON,
		PANEL_LABEL_STRING, "Dismiss",
		PANEL_NOTIFY_PROC, dismiss_status_window,
		XV_HELP_DATA, "ftptool:DismissButton",
		0);

	xv_set(status_window.panel,
		WIN_FIT_HEIGHT, 10,
		WIN_FIT_WIDTH, 10,
		0);

	justify_items(status_window.panel, TRUE);

	status_window.message = xv_create(status_window.panel,
		PANEL_MESSAGE,
		PANEL_LABEL_STRING, "Not Transferring",
		PANEL_ITEM_X, xv_get(status_message, XV_X)
		    + xv_get(status_message, XV_WIDTH)
		    + xv_col(status_window.panel, 1) / 2,
		PANEL_ITEM_Y, xv_get(status_message, XV_Y),
		0);

	status_window.size = xv_create(status_window.panel,
		PANEL_MESSAGE,
		PANEL_LABEL_STRING, "0 bytes",
		PANEL_ITEM_X, xv_get(size_message, XV_X)
		    + xv_get(size_message, XV_WIDTH)
		    + xv_col(status_window.panel, 1) / 2,
		PANEL_ITEM_Y, xv_get(size_message, XV_Y),
		0);

	xv_set(status_window.panel,
		WIN_FIT_HEIGHT, 10,
		WIN_FIT_WIDTH, 10,
		0);

	butrect = (Rect *)xv_get(status_window.dismiss, XV_RECT);
	xv_set(status_window.dismiss,
		XV_X, xv_get(status_window.panel, XV_WIDTH) / 2
		    - butrect->r_width / 2,
		XV_SHOW, openlook_mode ? FALSE : TRUE,
		0);

	window_fit(status_window.frame);

	x = rect.r_left - xv_get(status_window.frame, XV_WIDTH);
	y = rect.r_top;
	width = xv_get(status_window.frame, XV_WIDTH);
	height = xv_get(status_window.frame, XV_HEIGHT);

	set_geometry(status_window.geometry, status_window.frame,
	    width, height, x, y);
}
