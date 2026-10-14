#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ftptool.h"

#pragma ident   "@(#)schedule.c 1.5     93/08/19"

#ifdef USE_PROTOTYPES
struct schedule *new_schedule(char *menu_name, int direction,
	time_t date, time_t repeat_minutes, char *filename,
	struct hostlist *hl)
#else
struct schedule *new_schedule(menu_name, direction, date,
	repeat_minutes, filename, hl)
char	*menu_name;
int		direction;
time_t	date;
time_t	repeat_minutes;
char	*filename;
struct hostlist *hl;
#endif
{
	struct schedule *tmp = 0;

	tmp = (struct schedule *)malloc(sizeof (struct schedule));
	if (tmp == 0)
		return (0);
	bzero((char *)tmp, sizeof (struct schedule));
	tmp->menu_name = (char *)malloc((unsigned int)(2+strlen(menu_name)+1));
	if (tmp->menu_name == 0) {
		free_schedule(tmp);
		return (0);
	}
	sprintf(tmp->menu_name, "%c:%s", direction == RECV ? 'R' : 'S',
	    menu_name);

	tmp->direction = direction;
	tmp->date = date;
	tmp->repeat_minutes = repeat_minutes;
	tmp->filename = strdup(filename);
	if (tmp->filename == 0) {
		free_schedule(tmp);
		return (0);
	}
	tmp->hl = (struct hostlist *)malloc(sizeof (struct hostlist));
	if (tmp->hl == 0) {
		free_schedule(tmp);
		return (0);
	}
	bcopy((char *)hl, (char *)tmp->hl, sizeof (struct hostlist));
	tmp->next = 0;
	return (tmp);
}

#ifdef USE_PROTOTYPES
struct schedule *add_schedule(struct schedule *head, char *menu_name,
	int direction, time_t date, time_t repeat_minutes, char *filename,
	struct hostlist *hl)
#else
struct schedule *add_schedule(head, menu_name, direction, date,
	repeat_minutes, filename, hl)
struct schedule *head;
char	*menu_name;
int		direction;
time_t	date;
time_t	repeat_minutes;
char	*filename;
struct hostlist *hl;
#endif
{
	struct schedule *tmp = 0;
	struct schedule *oldnext = 0;

	for (tmp = head; tmp->next != 0; tmp = tmp->next)
		if (date < tmp->next->date)
			break;

	oldnext = tmp->next;
	tmp->next = new_schedule(menu_name, direction, date, repeat_minutes,
		filename, hl);
	if (tmp->next == 0) {
		tmp->next = oldnext;
		return (0);
	}

	tmp->next->next = oldnext;
	return (head);
}

#ifdef USE_PROTOTYPES
void free_schedule(struct schedule *head)
#else
void free_schedule(head)
struct schedule *head;
#endif
{
	struct schedule *tmp = 0;
	struct schedule *item = 0;

	item = head;
	while (item) {
		tmp = item->next;
		if (item->menu_name)
			free(item->menu_name);
		if (item->filename) {
			(void) unlink(item->filename);
			free(item->filename);
		}
		if (item->hl)
			free_hostlist(item->hl);
		free((char *)item);
		item = tmp;
	}
	return;
}

#ifdef USE_PROTOTYPES
void delete_schedule(struct schedule *head, char *menu_name, int direction)
#else
void delete_schedule(head, menu_name, direction)
struct schedule *head;
char	*menu_name;
int		direction;
#endif
{
	struct schedule *tmp = 0;
	struct schedule *item = 0;

	for (tmp = head; tmp->next != 0; tmp = tmp->next)
		if (tmp->next->direction == direction &&
			!strcmp(menu_name, &tmp->next->menu_name[2])) {
			item = tmp->next;
			tmp->next = tmp->next->next;
			item->next = 0;
			free_schedule(item);
			return;
		}
	return;
}

#ifdef USE_PROTOTYPES
void reorder_list(struct schedule *head)
#else
void reorder_list(head)
struct schedule *head;
#endif
{
	struct schedule *tmp = 0;
	struct schedule nl;
	struct schedule *newlist = &nl;
	struct schedule *item = 0;
	struct schedule *oldnext = 0;

	newlist->next = 0;
	item = head->next;
	while (item != 0) {
		/* insert item in tmp */
		for (tmp = newlist; tmp->next != 0; tmp = tmp->next)
			if (item->date < tmp->next->date)
				break;
		oldnext = tmp->next;
		tmp->next = item;
		item = item->next;
		tmp->next->next = oldnext;
	}
	head->next = newlist->next;
}

#ifdef USE_PROTOTYPES
void load_schedule(void)
#else
void load_schedule()
#endif
{
	static char scratch[2 * MAXHOSTNAMELEN + 4];
	char	*host = 0;
	char	*login = 0;

	schedule_footer_message("");
	host = (char *)xv_get(host_window.basic.host, PANEL_VALUE);
	login = (char *)xv_get(host_window.basic.login, PANEL_VALUE);
	sprintf(scratch, "%s@%s", login, host);
	xv_set(schedule_window.menu_name,
		PANEL_VALUE, scratch,
		0);
	update_date(1);
	xv_set(schedule_window.frame,
		XV_SHOW, TRUE,
		0);
}

#ifdef USE_PROTOTYPES
void set_current_schedule_proc(Menu menu, Menu_item menu_item)
#else
void set_current_schedule_proc(menu, menu_item)
Menu	menu;
Menu_item	menu_item;
#endif
{
	load_schedule();
	xv_set(menu,
		MENU_NOTIFY_STATUS, XV_ERROR,
		0);
}

#ifdef USE_PROTOTYPES
int schedule_exists(struct schedule *head, char *menu_name,
	int direction)
#else
int schedule_exists(head, menu_name, direction)
struct schedule *head;
char	*menu_name;
int		direction;
#endif
{
	struct schedule *tmp = 0;

	for (tmp = head->next; tmp != 0; tmp = tmp->next) {
		if (direction == tmp->direction &&
		    !strcmp(menu_name, &tmp->menu_name[2]))
			return (1);
	}
	return (0);
}

#ifdef USE_PROTOTYPES
void enter_schedule_info(int warnchange)
#else
void enter_schedule_info(warnchange)
int	warnchange;
#endif
{
	char	*menu_name = 0;
	int		direction;
	time_t	repeat_minutes;
	time_t		date;
	struct tm	tm;
	char	*filename = 0;
	int		answer;
	struct hostlist *hl = 0;
	int		nitems;
	int		ret;
#ifdef XVIEW3
	Xv_notice notice;
#endif

	direction = (int)xv_get(schedule_window.direction, PANEL_VALUE);
	if (direction == RECV)
		nitems = (int)xv_get(schedule_window.receive_list,
		    PANEL_LIST_NROWS);
	else
		nitems = (int)xv_get(schedule_window.send_list,
		    PANEL_LIST_NROWS);

	if (nitems == 0) {
		schedule_footer_message("Select items to transfer first.");
		goto out;
	}

	menu_name = (char *)xv_get(schedule_window.menu_name, PANEL_VALUE);
	if (menu_name == '\0') {
		schedule_footer_message("Specify login@host first.");
		goto out;
	}
	if (schedule_exists(&schedule_list, menu_name, direction)) {
		if (warnchange) {
#ifdef XVIEW3
			notice = xv_create(schedule_window.panel, NOTICE,
				NOTICE_MESSAGE_STRINGS,
					"That host/action is scheduled. Do you really",
					"want to change it?",
					0,
				NOTICE_BUTTON_YES, "Yes",
				NOTICE_BUTTON_NO, "No",
				NOTICE_STATUS, &answer,
				XV_SHOW, TRUE,
				0);
			xv_destroy_safe(notice);
#else
			answer = notice_prompt(schedule_window.panel, 0,
				NOTICE_MESSAGE_STRINGS,
					"That host/action is scheduled. Do you really",
					"want to change it?",
					0,
				NOTICE_BUTTON_YES, "Yes",
				NOTICE_BUTTON_NO, "No",
				0);
#endif
			if (answer != NOTICE_YES)
				goto out;
		}
		delete_schedule(&schedule_list, menu_name, direction);
	}
	bzero((char *)&tm, sizeof (tm));
	tm.tm_hour = (int)xv_get(schedule_window.hour, PANEL_VALUE);
	tm.tm_min = (int)xv_get(schedule_window.minute, PANEL_VALUE);
	tm.tm_mon = (int)xv_get(schedule_window.month, PANEL_VALUE);
	tm.tm_mday = (int)xv_get(schedule_window.day, PANEL_VALUE);
	tm.tm_year = (int)xv_get(schedule_window.year, PANEL_VALUE) - 1900;
	tm.tm_isdst = is_dst;
#if defined(SYSV) || defined(ultrix) || defined(SYSV386) || \
    defined(AIXV3) || defined(hpux)
	date = mktime(&tm);
#else
	date = timelocal(&tm);
#endif

	if (xv_get(schedule_window.repeat, PANEL_VALUE) == 1)
		repeat_minutes = xv_get(schedule_window.repeat_minutes,
		    PANEL_VALUE);
	else
		repeat_minutes = 0;

	filename = tempnam("/var/tmp", "sched");
	if (filename == 0) {
		schedule_footer_message("Can't create temporary file.");
		goto out;
	}
	if (direction == RECV)
		ret = save_batch_list(schedule_window.receive_list, filename);
	else
		ret = save_batch_list(schedule_window.send_list, filename);
	if (ret)
		goto out;

	hl = new_hostlist();
	if (hl == 0)
		goto out;
	bzero((char *)hl, sizeof (struct hostlist));
	hl->aliasname = strdup((char *)(xv_get(host_window.advanced.alias,
		PANEL_VALUE)));
	if (hl->aliasname == 0) {
		goto out;
	}

	hl->proxy = strdup((char *)(xv_get(host_window.advanced.proxy,
		PANEL_VALUE)));
	if (hl->proxy == 0) {
		goto out;
	}

	hl->last_visited =
	    strdup((char *)(xv_get(host_window.advanced.last_visited,
		PANEL_LABEL_STRING)));
	if (hl->last_visited == 0) {
		goto out;
	}

	hl->host = strdup((char *)(xv_get(host_window.basic.host,
	    PANEL_VALUE)));
	if (hl->host == 0) {
		goto out;
	}

	hl->login = strdup((char *)(xv_get(host_window.basic.login,
	    PANEL_VALUE)));
	if (hl->login == 0) {
		goto out;
	}

	hl->password = strdup((char *)(xv_get(host_window.basic.password,
		PANEL_VALUE)));
	if (hl->password == 0) {
		goto out;
	}

	hl->transfer_mode = (int)xv_get(host_window.advanced.transfer_mode,
		PANEL_VALUE);

	hl->remote_directory = strdup((char *)
		(xv_get(host_window.advanced.remote_auto_cd, PANEL_VALUE)));
	if (hl->remote_directory == 0) {
		goto out;
	}

	hl->local_directory = strdup((char *)
		(xv_get(host_window.advanced.local_auto_cd, PANEL_VALUE)));
	if (hl->local_directory == 0) {
		goto out;
	}

	hl->dir_parse = strdup((char *)
		(xv_get(host_window.advanced.dir_parse, PANEL_VALUE)));
	if (hl->dir_parse == 0) {
		goto out;
	}

	hl->comment = strdup((char *)
		(xv_get(host_window.advanced.comment, PANEL_VALUE)));
	if (hl->comment == 0) {
		goto out;
	}

	if (add_schedule(&schedule_list, menu_name, direction, date,
		repeat_minutes, filename, hl)
		== 0) {
		xv_set(schedule_window.frame,
			FRAME_SHOW_FOOTER, TRUE,
			FRAME_LEFT_FOOTER, "Add failed.",
			0);
		goto out;
	}
	reload_schedule_menu(&schedule_list);
	return;
out:
	if (hl)
		free_hostlist(hl);
	return;
}

#ifdef USE_PROTOTYPES
void schedule_add_proc(Menu menu, Menu_item menu_item)
#else
void schedule_add_proc(menu, menu_item)
Menu	menu;
Menu_item menu_item;
#endif
{
	schedule_footer_message("");
	enter_schedule_info(1);
	xv_set(menu,
		MENU_NOTIFY_STATUS, XV_ERROR,
		0);
}

#ifdef USE_PROTOTYPES
void schedule_change_proc(Menu menu, Menu_item menu_item)
#else
void schedule_change_proc(menu, menu_item)
Menu	menu;
Menu_item menu_item;
#endif
{
	schedule_footer_message("");
	enter_schedule_info(0);
	xv_set(menu,
		MENU_NOTIFY_STATUS, XV_ERROR,
		0);
}

#ifdef USE_PROTOTYPES
void schedule_delete_proc(Menu menu, Menu_item menu_item)
#else
void schedule_delete_proc(menu, menu_item)
Menu	menu;
Menu_item menu_item;
#endif
{
	char	*menu_name;
	int		direction;

	schedule_footer_message("");
	menu_name = (char *)xv_get(schedule_window.menu_name, PANEL_VALUE);
	direction = (int)xv_get(schedule_window.direction, PANEL_VALUE);
	delete_schedule(&schedule_list, menu_name, direction);
	reload_schedule_menu(&schedule_list);
	xv_set(menu,
		MENU_NOTIFY_STATUS, XV_ERROR,
		0);
}

#ifdef USE_PROTOTYPES
void schedule_item_proc(Menu menu, Menu_item menu_item)
#else
void schedule_item_proc(menu, menu_item)
Menu	menu;
Menu_item menu_item;
#endif
{
	struct schedule *tmp = 0;
	struct tm *tm = 0;

	schedule_footer_message("");
	tmp = (struct schedule *)xv_get(menu_item, MENU_CLIENT_DATA);
	xv_set(schedule_window.menu_name,
		PANEL_VALUE, tmp->menu_name + 2,
		0);
	xv_set(schedule_window.direction,
		PANEL_VALUE, tmp->direction,
		0);
	update_date(0);
	tm = localtime(&tmp->date);
	xv_set(schedule_window.frame,
		XV_SHOW, TRUE,
		0);
	xv_set(schedule_window.hour,
		PANEL_VALUE, tm->tm_hour,
		0);
	xv_set(schedule_window.minute,
		PANEL_VALUE, tm->tm_min,
		0);
	xv_set(schedule_window.month,
		PANEL_VALUE, tm->tm_mon,
		0);
	xv_set(schedule_window.day,
		PANEL_VALUE, tm->tm_mday,
		0);
	xv_set(schedule_window.year,
		PANEL_VALUE, tm->tm_year + 1900,
		0);

	if (tmp->repeat_minutes > 0) {
		xv_set(schedule_window.repeat,
			PANEL_VALUE, 1,
			PANEL_CHOICE_STRING, 0, "in ",
			XV_SHOW, TRUE,
			0);
		xv_set(schedule_window.repeat_minutes,
			PANEL_VALUE, tmp->repeat_minutes,
			XV_SHOW, TRUE,
			0);
		if (tmp->repeat_minutes > 1) {
			xv_set(schedule_window.repeat_message,
				PANEL_LABEL_STRING, "minutes",
				PANEL_LABEL_BOLD, FALSE,
				XV_SHOW, TRUE,
				0);
		} else {
			xv_set(schedule_window.repeat_message,
				PANEL_LABEL_STRING, "minute",
				PANEL_LABEL_BOLD, FALSE,
				XV_SHOW, TRUE,
				0);
		}
	} else {
		xv_set(schedule_window.repeat,
			PANEL_VALUE, 0,
			PANEL_CHOICE_STRING, 0, "",
			XV_SHOW, TRUE,
			0);
		xv_set(schedule_window.repeat_minutes,
			PANEL_VALUE, 1,
			XV_SHOW, FALSE,
			0);
		xv_set(schedule_window.repeat_message,
			PANEL_LABEL_STRING, "minute",
			PANEL_LABEL_BOLD, FALSE,
			XV_SHOW, FALSE,
			0);
	}

	if (tmp->direction == RECV) {
		load_batch_list(schedule_window.receive_list, tmp->filename);
	} else {
		load_batch_list(schedule_window.send_list, tmp->filename);
	}

	xv_set(menu,
		MENU_NOTIFY_STATUS, XV_ERROR,
		0);
}

#ifdef USE_PROTOTYPES
void reload_schedule_menu(struct schedule *head)
#else
void reload_schedule_menu(head)
struct schedule *head;
#endif
{
	Menu	menu = xv_get(schedule_window.hosts, PANEL_ITEM_MENU);
	int	nitems = xv_get(menu, MENU_NITEMS);
	int	row;
	Menu_item	mi;
	Frame	frame;
	int		isshown;
	int		maxwidth = 0;
	int		width = 0;
	double	cols;
	struct schedule *tmp = 0;
	char	*currentname;

	frame = (Panel)xv_get(menu, MENU_PIN_WINDOW);
	isshown = FALSE;
	if (frame)
		isshown = xv_get(frame, XV_SHOW);

	currentname = (char *)xv_get(schedule_window.menu_name, PANEL_VALUE);

	if (isshown) {
		xv_set(frame,
#ifdef XVIEW3
			FRAME_CMD_PIN_STATE, FRAME_CMD_PIN_OUT,
#else
			FRAME_CMD_PUSHPIN_IN, FALSE,
#endif
			XV_SHOW, FALSE,
			0);
	}
	/* 2 items minimum, 1 for title */
	for (row = nitems; row > 1; row--)
		xv_set(menu,
			MENU_REMOVE, row,
			0);

	nitems = 0;
	for (tmp = head->next; tmp != 0; tmp = tmp->next) {
		width = strlen(tmp->menu_name);
		maxwidth = MAX(maxwidth, width);
		mi = (Menu_item)xv_create(XV_NULL, MENUITEM,
			MENU_STRING, tmp->menu_name,
			MENU_NOTIFY_PROC, schedule_item_proc,
			MENU_CLIENT_DATA, tmp,
			MENU_RELEASE,
			0);
		if (mi == XV_NULL) {
			fprintf(stderr, "Out of memory for menu item.\n");
			exit(1);
		}
		xv_set(menu,
			MENU_APPEND_ITEM, mi,
			PANEL_PAINT, PANEL_NONE,
			0);
		/* reload current information */
		if (!strcmp(currentname, &tmp->menu_name[2]))
			schedule_item_proc(menu, mi);
		nitems++;
	}
	if (nitems == 0) {
		mi = (Menu_item)xv_create(XV_NULL, MENUITEM,
			MENU_STRING, "No Hosts!",
			MENU_RELEASE,
			0);
		if (mi == XV_NULL) {
			fprintf(stderr, "Out of memory for menu item.\n");
			exit(1);
		}
		xv_set(menu,
			MENU_APPEND_ITEM, mi,
			0);
	}

	if (maxwidth == 0)
		maxwidth = 1;

	cols = 2.0 * nitems;
	cols /= maxwidth;
	cols = ceil(sqrt(cols));
	xv_set(menu,
		MENU_NCOLS, (int)cols,
		0);
	if (isshown) {
		xv_set(frame,
#ifdef XVIEW3
			FRAME_CMD_PIN_STATE, FRAME_CMD_PIN_IN,
#else
			FRAME_CMD_PUSHPIN_IN, TRUE,
#endif
			XV_SHOW, TRUE,
			0);
	}
}

#ifdef USE_PROTOTYPES
void action_choice_proc(Panel_item item, unsigned int value,
	Event *event)
#else
void action_choice_proc(item, value, event)
Panel_item	item;
unsigned int	value;
Event	*event;
#endif
{
	schedule_footer_message("");
	if (value == RECV) {
		xv_set(schedule_window.send_list,
			XV_SHOW, FALSE,
			0);
		xv_set(schedule_window.receive_list,
			XV_SHOW, TRUE,
			0);
	} else {
		xv_set(schedule_window.receive_list,
			XV_SHOW, FALSE,
			0);
		xv_set(schedule_window.send_list,
			XV_SHOW, TRUE,
			0);
	}
}

#ifdef USE_PROTOTYPES
Notify_value schedule_timer_proc(void)
#else
Notify_value schedule_timer_proc()
#endif
{
	dowhat = DOSCHEDULE;
	notify_stop();
	return (NOTIFY_DONE);
}

#ifdef USE_PROTOTYPES
void activate_schedule_timer(void)
#else
void activate_schedule_timer()
#endif
{
	time_t	t;
	struct itimerval itimer;

	if (schedule_list.next == 0) {
		xv_set(schedule_window.process,
			PANEL_LABEL_STRING, "Process Batch",
			XV_HELP_DATA, "ftptool:ProcessBatch",
			0);
		return;
	}
	if (!strcmp((char *)xv_get(schedule_window.process, PANEL_LABEL_STRING),
		"Process Batch"))
			return;	/* cancelled already */
	t = time((time_t *)0);
	itimer.it_value.tv_sec = schedule_list.next->date - t;
	itimer.it_value.tv_usec = 0;
	itimer.it_interval.tv_sec = 0;
	itimer.it_interval.tv_usec = 0;
	if (itimer.it_value.tv_sec < 0)
		itimer.it_value.tv_sec = 1;
	notify_set_itimer_func(schedule_window.frame, schedule_timer_proc,
		ITIMER_REAL, &itimer, (struct itimerval *)0);
}

#ifdef USE_PROTOTYPES
void batch_process_proc(Panel_item item, Event *event)
#else
void batch_process_proc(item, event)
Panel_item	item;
Event	*event;
#endif
{
	schedule_footer_message("");
	if (!strcmp("Process Batch",
	    (char *)xv_get(item, PANEL_LABEL_STRING))) {
		if (schedule_list.next == 0) {
			schedule_footer_message("Nothing to process.");
			xv_set(item,
				PANEL_NOTIFY_STATUS, XV_ERROR,
				0);
			return;
		}
		xv_set(item,
			PANEL_LABEL_STRING, "Abort Batch",
			XV_HELP_DATA, "ftptool:AbortBatch",
			0);
		xv_set(base_window.connect,
			PANEL_INACTIVE, TRUE,
			0);
		xv_set(host_window.basic.connect,
			PANEL_INACTIVE, TRUE,
			0);
		activate_schedule_timer();
		batch_mode = 1;
	} else {
		xv_set(item,
			PANEL_LABEL_STRING, "Process Batch",
			XV_HELP_DATA, "ftptool:ProcessBatch",
			0);
		xv_set(base_window.connect,
			PANEL_INACTIVE, FALSE,
			0);
		xv_set(host_window.basic.connect,
			PANEL_INACTIVE, FALSE,
			0);
		notify_set_itimer_func(schedule_window.frame,
		    NOTIFY_FUNC_NULL, ITIMER_REAL, (struct itimerval *)0,
		    (struct itimerval *)0);
		batch_mode = 0;
	}
	xv_set(item,
		PANEL_NOTIFY_STATUS, XV_ERROR,
		0);
}

#ifdef USE_PROTOTYPES
void doschedule(void)
#else
void doschedule()
#endif
{
	struct schedule *tmp = schedule_list.next;

	xv_set(host_window.advanced.alias,
		PANEL_VALUE, tmp->hl->aliasname,
		0);
	xv_set(host_window.advanced.last_visited,
		PANEL_LABEL_STRING, tmp->hl->last_visited,
		0);
	xv_set(host_window.advanced.proxy,
		PANEL_VALUE, tmp->hl->proxy,
		0);
	xv_set(host_window.basic.host,
		PANEL_VALUE, tmp->hl->host,
		0);
	xv_set(host_window.basic.login,
		PANEL_VALUE, tmp->hl->login,
		0);
	xv_set(host_window.basic.password,
		PANEL_VALUE, tmp->hl->password,
		0);
	xv_set(host_window.advanced.transfer_mode,
		PANEL_VALUE, tmp->hl->transfer_mode,
		0);
	xv_set(host_window.advanced.remote_auto_cd,
		PANEL_VALUE, tmp->hl->remote_directory,
		0);
	xv_set(host_window.advanced.local_auto_cd,
		PANEL_VALUE, tmp->hl->local_directory,
		0);
	xv_set(host_window.advanced.dir_parse,
		PANEL_VALUE, tmp->hl->dir_parse,
		0);
	xv_set(host_window.advanced.comment,
		PANEL_VALUE, tmp->hl->comment,
		0);

	if (try_proxy) {
		xv_set(host_window.advanced.proxy,
			XV_SHOW, TRUE,
			0);
	} else {
		xv_set(host_window.advanced.proxy,
			XV_SHOW, FALSE,
			0);
	}
	if (doconnect()) {
		/* failed. Try again later. */
		schedule_list.next->date += 300;
		reorder_list(&schedule_list);
		goto out;
	}
	if (tmp->direction == RECV) {
		load_batch_list(schedule_window.receive_list, tmp->filename);
		dobatchget();
	} else {
		load_batch_list(schedule_window.send_list, tmp->filename);
		dobatchput();
	}
	disconnect();
	if (tmp->repeat_minutes > 0) {
		tmp->date += (tmp->repeat_minutes * 60);
		reorder_list(&schedule_list);
	} else {
		delete_schedule(&schedule_list, &tmp->menu_name[2],
		    tmp->direction);
	}
out:
	reload_schedule_menu(&schedule_list);
	activate_schedule_timer();
}
