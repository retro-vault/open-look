#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 200809L
#include "mosaix.h"
#include "toolbar-icons.h"
#include "app-icon.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/panel.h>
#include <xview/scrollbar.h>
#include <xview/openmenu.h>
#include <xview/notice.h>
#include <xview/font.h>
#include <xview/svrimage.h>
#include <olgx/olgx.h>

MxPreferences mx_preferences;
static Frame frame, properties, script_frame;
static Panel toolbar;
static Xv_window viewport;
static Scrollbar vertical, horizontal;
static Panel_item location, back_button, forward_button, reload_button, home_button, settings_button;
static void (*location_default_event)(Panel_item, Event *);
static int tool_size, address_x, address_width;
static Server_image tool_label;
/* OLWM already leaves five pixels below the title. Balance that inset below
 * the controls instead of adding another margin above them. */
enum { TOOL_MARGIN = 3, TOOL_TOP = 0, TOOL_BOTTOM = 5,
       TOOL_GAP = 4, ADDRESS_INSET = 7,
       TOOL_LABEL_SIZE = MX_TOOL_ICON_SIZE - 2 };
static Panel_item home_field, zoom_field, privacy_field, content_field, prompt_field;
static Menu page_menu;
static int alive = 1, start_ok;
static unsigned revision;
static unsigned scroll_revision;
static int scrollbar_pressed;
static char shown_url[MX_URL_MAX];
static void layout(void);
static void command(int action);

/* Position the decorated popup inside the main browser's client area. */
static void center_dialog(Frame dialog)
{
    Display *dpy = (Display *)xv_get(frame, XV_DISPLAY);
    Window root = (Window)xv_get(xv_get(frame, XV_ROOT), XV_XID), child;
    int x, y;
    Rect popup;
    XWindowAttributes screen;
    frame_get_rect(dialog, &popup);
    XTranslateCoordinates(dpy, (Window)xv_get(frame, XV_XID), root,
                          0, 0, &x, &y, &child);
    popup.r_left = x + ((int)xv_get(frame, XV_WIDTH) - popup.r_width) / 2;
    popup.r_top = y + ((int)xv_get(frame, XV_HEIGHT) - popup.r_height) / 2;
    if (XGetWindowAttributes(dpy, root, &screen)) {
        if (popup.r_left + popup.r_width > screen.width)
            popup.r_left = screen.width - popup.r_width;
        if (popup.r_top + popup.r_height > screen.height)
            popup.r_top = screen.height - popup.r_height;
    }
    if (popup.r_left < 0) popup.r_left = 0;
    if (popup.r_top < 0) popup.r_top = 0;
    frame_set_rect(dialog, &popup);
}
static int confirm_message(const char *text, int confirm)
{
    int result = NOTICE_NO;
    /* Object notices center on their owner; legacy notice_prompt follows
     * the pointer and uses the old screen-locking placement instead. */
    Xv_notice notice = xv_create(frame, NOTICE, NOTICE_LOCK_SCREEN, FALSE,
        NOTICE_BLOCK_THREAD, TRUE, NOTICE_STATUS, &result,
        NOTICE_MESSAGE_STRINGS, text, NULL, NULL);
    if (confirm)
        xv_set(notice, NOTICE_BUTTON_YES, "Clear", NOTICE_BUTTON_NO, "Cancel", NULL);
    else
        xv_set(notice, NOTICE_BUTTON_YES, "OK", NULL);
    xv_set(notice, XV_SHOW, TRUE, NULL);
    xv_destroy(notice);
    return result;
}
static void message(const char *text)
{
    (void)confirm_message(text, 0);
}
static void close_properties(Frame f)
{
    xv_set(f, FRAME_CMD_PIN_STATE, FALSE, XV_SHOW, FALSE, NULL);
}
static void save_properties(Panel_item item, Event *event)
{
    MxPreferences p = mx_preferences;
    if (!mx_normalize_url((char *)xv_get(home_field, PANEL_VALUE), p.homepage, sizeof p.homepage)) {
        xv_set(item, PANEL_NOTIFY_STATUS, XV_ERROR, NULL);
        message("Enter a valid HTTP, HTTPS or file address, or about:blank."); return;
    }
    p.zoom = (int)xv_get(zoom_field, PANEL_VALUE);
    int flags = (int)xv_get(privacy_field, PANEL_VALUE);
    p.persistent = !!(flags & 1); p.cookies = !!(flags & 2); p.session_cookies = !!(flags & 4);
    flags = (int)xv_get(content_field, PANEL_VALUE);
    p.javascript = !!(flags & 1); p.images = !!(flags & 2);
    if (!mx_preferences_save(&p)) {
        xv_set(item, PANEL_NOTIFY_STATUS, XV_ERROR, NULL);
        message("Could not save MosaiX preferences."); return;
    }
    mx_preferences = p; mx_zoom(p.zoom);
    xv_set(properties, XV_SHOW, FALSE, NULL);
    xv_set(frame, FRAME_LEFT_FOOTER, "Preferences saved. Content and privacy changes apply after restarting MosaiX.", NULL);
}
static void cancel_properties(Panel_item item, Event *event) { close_properties(properties); }
static void clear_data(Panel_item item, Event *event)
{
    /* These actions must not dismiss an unpinned property sheet. */
    xv_set(item, PANEL_NOTIFY_STATUS, XV_ERROR, NULL);
    int cache = (int)xv_get(item, PANEL_CLIENT_DATA);
    int answer = confirm_message(cache ? "Clear the HTTP cache?" :
        "Clear all cookies? This signs you out of websites.", 1);
    if (answer == NOTICE_YES) mx_clear(cache);
}
/* Reset edits to the currently applied preferences, including keyboard zoom. */
static void reset_properties(Panel_item item, Event *event)
{
    if (item) xv_set(item, PANEL_NOTIFY_STATUS, XV_ERROR, NULL);
    xv_set(home_field, PANEL_VALUE, mx_preferences.homepage, NULL);
    xv_set(zoom_field, PANEL_VALUE, mx_preferences.zoom, NULL);
    xv_set(privacy_field, PANEL_VALUE, mx_preferences.persistent | mx_preferences.cookies << 1 | mx_preferences.session_cookies << 2, NULL);
    xv_set(content_field, PANEL_VALUE, mx_preferences.javascript | mx_preferences.images << 1, NULL);
}

/* Use the native recessed rule to separate sections of the property sheet. */
static void properties_repaint(Panel panel, Xv_window window, Rectlist *damage)
{
    Graphics_info *ginfo = (Graphics_info *)xv_get(panel, PANEL_GINFO);
    int width = (int)xv_get(panel, XV_WIDTH);
    const int rules[] = {108, 238, 318};
    for (unsigned i = 0; i < sizeof rules / sizeof rules[0]; ++i)
        olgx_draw_box(ginfo, (Window)xv_get(window, XV_XID), 18, rules[i],
                      width - 36, 2, OLGX_INVOKED, FALSE);
}

static void show_properties(void)
{
    if (!properties) {
        properties = xv_create(frame, FRAME_CMD, FRAME_LABEL, "MosaiX: Properties",
            FRAME_CMD_DEFAULT_PIN_STATE, FALSE, FRAME_DONE_PROC, close_properties,
            XV_WIDTH, 540, XV_HEIGHT, 374, XV_SHOW, FALSE, NULL);
        Panel p = xv_get(properties, FRAME_CMD_PANEL);
        xv_set(p, PANEL_LAYOUT, PANEL_HORIZONTAL, PANEL_REPAINT_PROC, properties_repaint, NULL);
        xv_create(p, PANEL_MESSAGE, XV_X, 18, XV_Y, 18,
            PANEL_LABEL_STRING, "General", PANEL_LABEL_BOLD, TRUE, NULL);
        home_field = xv_create(p, PANEL_TEXT, XV_X, 42, XV_Y, 44,
            PANEL_LABEL_STRING, "Home page:", PANEL_VALUE_X, 132,
            PANEL_VALUE_DISPLAY_WIDTH, 390, PANEL_VALUE_STORED_LENGTH, MX_URL_MAX - 1, NULL);
        zoom_field = xv_create(p, PANEL_NUMERIC_TEXT, XV_X, 42, XV_Y, 76,
            PANEL_LABEL_STRING, "Zoom (%):", PANEL_VALUE_X, 132,
            PANEL_MIN_VALUE, 50, PANEL_MAX_VALUE, 200, PANEL_VALUE_DISPLAY_LENGTH, 4, NULL);
        xv_create(p, PANEL_MESSAGE, XV_X, 226, XV_Y, 76,
            PANEL_LABEL_STRING, "50 to 200%", NULL);
        xv_create(p, PANEL_MESSAGE, XV_X, 18, XV_Y, 126,
            PANEL_LABEL_STRING, "Privacy", PANEL_LABEL_BOLD, TRUE, NULL);
        xv_create(p, PANEL_MESSAGE, XV_X, 318, XV_Y, 126,
            PANEL_LABEL_STRING, "Content", PANEL_LABEL_BOLD, TRUE, NULL);
        privacy_field = xv_create(p, PANEL_CHECK_BOX, XV_X, 18, XV_Y, 152,
            PANEL_LAYOUT, PANEL_VERTICAL,
            PANEL_CHOICE_STRINGS, "Keep profile between sessions", "Allow cookies", "Restore session cookies", NULL, NULL);
        content_field = xv_create(p, PANEL_CHECK_BOX, XV_X, 318, XV_Y, 152,
            PANEL_LAYOUT, PANEL_VERTICAL,
            PANEL_CHOICE_STRINGS, "Enable JavaScript", "Load images", NULL, NULL);
        xv_create(p, PANEL_MESSAGE, XV_X, 18, XV_Y, 258,
            PANEL_LABEL_STRING, "Browsing data:", PANEL_LABEL_BOLD, TRUE, NULL);
        Panel_item cookies = xv_create(p, PANEL_BUTTON, XV_X, 150, XV_Y, 254,
            PANEL_LABEL_STRING, "Clear Cookies...", PANEL_NOTIFY_PROC, clear_data, PANEL_CLIENT_DATA, 0, NULL);
        xv_create(p, PANEL_BUTTON, XV_X, 150 + (int)xv_get(cookies, XV_WIDTH) + 12, XV_Y, 254,
            PANEL_LABEL_STRING, "Clear Cache...", PANEL_NOTIFY_PROC, clear_data, PANEL_CLIENT_DATA, 1, NULL);
        xv_create(p, PANEL_MESSAGE, XV_X, 18, XV_Y, 290,
            PANEL_LABEL_STRING, "Restart MosaiX to apply privacy and content changes.", NULL);
        Panel_item apply = xv_create(p, PANEL_BUTTON, XV_Y, 336,
            PANEL_LABEL_STRING, "Apply", PANEL_NOTIFY_PROC, save_properties, NULL);
        Panel_item reset = xv_create(p, PANEL_BUTTON, XV_Y, 336,
            PANEL_LABEL_STRING, "Reset", PANEL_NOTIFY_PROC, reset_properties, NULL);
        Panel_item cancel = xv_create(p, PANEL_BUTTON, XV_Y, 336,
            PANEL_LABEL_STRING, "Cancel", PANEL_NOTIFY_PROC, cancel_properties, NULL);
        Panel_item actions[] = {apply, reset, cancel};
        int button_width = (int)xv_get(cancel, XV_WIDTH) + 12;
        int x = (540 - 3 * button_width - 24) / 2;
        for (unsigned i = 0; i < sizeof actions / sizeof actions[0]; ++i) {
            int extra = button_width - (int)xv_get(actions[i], XV_WIDTH);
            xv_set(actions[i], PANEL_LABEL_WIDTH, (int)xv_get(actions[i], PANEL_LABEL_WIDTH) + extra,
                XV_X, x, XV_Y, 336, NULL);
            x += button_width + 12;
        }
        xv_set(p, PANEL_DEFAULT_ITEM, apply, XV_WIDTH, 540, XV_HEIGHT, 374, NULL);
    }
    reset_properties(0, NULL);
    center_dialog(properties);
    xv_set(properties, XV_SHOW, TRUE, WIN_FRONT, NULL);
}
static void script_answer(Panel_item item, Event *event)
{
    int accept = (int)xv_get(item, PANEL_CLIENT_DATA);
    char text[1024]; snprintf(text, sizeof text, "%s", (char *)xv_get(prompt_field, PANEL_VALUE));
    xv_set(script_frame, XV_SHOW, FALSE, NULL); mx_dialog_reply(accept, text);
}
static void script_done(Frame f) { xv_set(f, XV_SHOW, FALSE, NULL); mx_dialog_reply(0, ""); }
void mx_ui_dialog_dismiss(void) { if (script_frame) xv_set(script_frame, XV_SHOW, FALSE, NULL); }
void mx_ui_dialog(const char *origin, const char *msg, const char *initial, int prompt)
{
    if (script_frame) xv_destroy(script_frame);
    script_frame = xv_create(frame, FRAME_CMD, FRAME_LABEL, "MosaiX: Website Message",
        FRAME_CMD_PUSHPIN_IN, TRUE, FRAME_DONE_PROC, script_done, XV_SHOW, FALSE, NULL);
    Panel p = xv_get(script_frame, FRAME_CMD_PANEL);
    xv_set(p, PANEL_LAYOUT, PANEL_VERTICAL, NULL);
    char from[96]; snprintf(from, sizeof from, "%.90s", origin);
    xv_create(p, PANEL_MESSAGE, PANEL_LABEL_STRING, from, NULL);
    /* Bound each label to keep untrusted website messages within the dialog. */
    const char *at = msg;
    for (int row = 0; *at && row < 12; ++row) {
        char line[81]; size_t len = strcspn(at, "\n"); if (len > 80) len = 80;
        memcpy(line, at, len); line[len] = 0; at += len; if (*at == '\n') ++at;
        xv_create(p, PANEL_MESSAGE, PANEL_LABEL_STRING, line, NULL);
    }
    prompt_field = xv_create(p, PANEL_TEXT, PANEL_LABEL_STRING, "Response:", PANEL_VALUE, initial,
        PANEL_VALUE_STORED_LENGTH, 1023, PANEL_VALUE_DISPLAY_LENGTH, 50, XV_SHOW, prompt, NULL);
    xv_create(p, PANEL_BUTTON, PANEL_LABEL_STRING, "OK", PANEL_NOTIFY_PROC, script_answer, PANEL_CLIENT_DATA, 1, NULL);
    xv_create(p, PANEL_BUTTON, PANEL_LABEL_STRING, "Cancel", PANEL_NOTIFY_PROC, script_answer, PANEL_CLIENT_DATA, 0, NULL);
    window_fit(p); window_fit(script_frame);
    center_dialog(script_frame);
    xv_set(script_frame, XV_SHOW, TRUE, NULL);
}
static void command(int action)
{
    if (action == MX_SETTINGS) { show_properties(); return; }
    if (action == MX_ABOUT) { message("MosaiX\nAn OpenLook web browser, in tribute to Mosaic.\nPowered by the Chromium Embedded Framework."); return; }
    if (action == MX_ZOOM_IN || action == MX_ZOOM_OUT || action == MX_ZOOM_RESET) {
        int z = action == MX_ZOOM_RESET ? 100 : mx_preferences.zoom + (action == MX_ZOOM_IN ? 10 : -10);
        if (z >= 50 && z <= 200) { mx_preferences.zoom = z; mx_zoom(z); }
        return;
    }
    mx_action(action);
}
void mx_ui_command(int action) { command(action); }
void mx_ui_location(void)
{
    xv_set(toolbar, PANEL_CARET_ITEM, location, WIN_SET_FOCUS, NULL);
    xv_set(location, PANEL_VALUE, mx_browser.url, PANEL_TEXT_SELECT_LINE, NULL);
}
static void button(Panel_item item, Event *event)
{
    int action = (int)xv_get(item, PANEL_CLIENT_DATA);
    if (action == MX_RELOAD && mx_browser.loading) action = MX_STOP;
    command(action);
}
static Xv_opaque menu_command(Menu menu, Menu_item item) { command((int)xv_get(item, MENU_VALUE)); return item; }
static Menu make_menu(const char *title)
{
    return xv_create(NULL, MENU_COMMAND_MENU, MENU_GEN_PIN_WINDOW, frame, title, MENU_NOTIFY_PROC, menu_command, NULL);
}
static void add_menu_item(Menu menu, const char *label, int action)
{
    Menu_item item = xv_create(NULL, MENUITEM, MENU_STRING, label, MENU_VALUE, action, NULL);
    xv_set(menu, MENU_APPEND_ITEM, item, NULL);
}
static void paint_tool(Panel_item item)
{
    if (!item) return;
    panel_paint(item, PANEL_CLEAR);
    int inactive = (int)xv_get(item, PANEL_INACTIVE);
    int x = (int)xv_get(item, XV_X), y = (int)xv_get(item, XV_Y);
    if (inactive) {
        /* Keep the item genuinely inactive, but replace XView's stippled
         * outline with the normal border. Only the RGBA glyph is faded. */
        Graphics_info *ginfo = (Graphics_info *)xv_get(toolbar, PANEL_GINFO);
        olgx_draw_button(ginfo, (Window)xv_get(toolbar, XV_XID), x, y,
            tool_size, tool_size, (Xv_opaque)"", OLGX_NORMAL);
    }
    int action = (int)xv_get(item, PANEL_CLIENT_DATA);
    if (action == MX_RELOAD && mx_browser.loading) action = MX_STOP;
    int inset = (tool_size - MX_TOOL_ICON_SIZE) / 2;
    mx_toolbar_icon_paint(toolbar, action,
        x + inset, y + inset, inactive);
}
static void paint_tools(void)
{
    paint_tool(back_button); paint_tool(forward_button); paint_tool(reload_button);
    paint_tool(home_button); paint_tool(settings_button);
}
static void paint_toolbar(void)
{
    if (!location || address_width <= 0) return;
    /* XView adds a two-pixel separator to panels at y=0. This toolbar owns
     * that strip; clear it before repainting the button and address outlines. */
    XClearArea((Display *)xv_get(toolbar, XV_DISPLAY),
        (Window)xv_get(toolbar, XV_XID), 0, 0, 0, 2, False);
    Graphics_info *ginfo = (Graphics_info *)xv_get(toolbar, PANEL_GINFO);
    olgx_draw_button(ginfo, (Window)xv_get(toolbar, XV_XID), address_x,
        TOOL_TOP, address_width, tool_size, (Xv_opaque)"", OLGX_NORMAL);
    panel_paint(location, PANEL_CLEAR);
    paint_tools();
}
static Notify_value toolbar_event(Xv_window win, Event *event, Notify_arg arg, Notify_event_type type)
{
    Notify_value result = notify_next_event_func(win, (Notify_event)event, arg, type);
    if (event_action(event) == WIN_REPAINT) paint_toolbar();
    return result;
}
static void tool_event(Panel_item item, Event *event)
{
    panel_default_handle_event(item, event);
    paint_tool(item);
}
static Panel_item tool_button(int action, int x)
{
    /* Use XView's image-button outline and padding. A blank bitmap lets the
     * native control paint its normal/pressed background before the RGBA icon. */
    Panel_item item = xv_create(toolbar, PANEL_BUTTON, PANEL_LABEL_IMAGE, tool_label,
        XV_X, x, XV_Y, TOOL_TOP, PANEL_CLIENT_DATA, action,
        PANEL_EVENT_PROC, tool_event, PANEL_NOTIFY_PROC, button, NULL);
    return item;
}
static Panel_setting navigate(Panel_item item, Event *event)
{
    char url[MX_URL_MAX];
    if (mx_normalize_url((char *)xv_get(item, PANEL_VALUE), url, sizeof url)) mx_navigate(url);
    else message("Enter a valid HTTP, HTTPS or file address, or about:blank.");
    return PANEL_NONE;
}

/* A plain click selects the address for replacement; dragging and Shift-click
 * retain XView's normal selection behavior for editing part of an address. */
static void location_event(Panel_item item, Event *event)
{
    static int pressed, dragged, press_x, press_y;
    int select = event_action(event) == ACTION_SELECT;
    if (select && event_is_down(event)) {
        pressed = 1;
        dragged = event_shift_is_down(event);
        press_x = event_x(event);
        press_y = event_y(event);
    } else if (pressed && event_id(event) == LOC_DRAG &&
               (abs(event_x(event) - press_x) > 3 || abs(event_y(event) - press_y) > 3)) {
        dragged = 1;
    }
    location_default_event(item, event);
    if (select && event_is_up(event)) {
        if (pressed && !dragged) xv_set(item, PANEL_TEXT_SELECT_LINE, NULL);
        pressed = 0;
    }
}
void mx_ui_context(int x, int y)
{
    Event event; event_init(&event); event_set_id(&event, MS_RIGHT); event_set_down(&event);
    event_set_x(&event, x); event_set_y(&event, y); event_set_window(&event, viewport);
    menu_show(page_menu, viewport, &event, NULL);
}
static Notify_value viewport_event(Xv_window win, Event *event, Notify_arg arg, Notify_event_type type)
{
    if (event_id(event) == SCROLLBAR_REQUEST) {
        Scrollbar source = (Scrollbar)arg;
        int x = mx_browser.x, y = mx_browser.y;
        if (source == horizontal) x = (int)xv_get(source, SCROLLBAR_VIEW_START);
        if (source == vertical) y = (int)xv_get(source, SCROLLBAR_VIEW_START);
        /* XView also posts this event when we synchronize VIEW_START from
         * Chromium. Do not send those acknowledgements back as new scrolls. */
        if (x == mx_browser.x && y == mx_browser.y) return NOTIFY_DONE;
        mx_scroll(x, y);
        /* Do not apply an already-published snapshot after this request. */
        scroll_revision = mx_browser.scroll_revision;
        return NOTIFY_DONE;
    }
    return notify_next_event_func(win, (Notify_event)event, arg, type);
}
static Notify_value scrollbar_event(Xv_window win, Event *event, Notify_arg arg, Notify_event_type type)
{
    if (event_action(event) == ACTION_SELECT || event_action(event) == ACTION_ADJUST)
        scrollbar_pressed = event_is_down(event);
    return notify_next_event_func(win, (Notify_event)event, arg, type);
}
static Notify_value frame_event(Xv_window win, Event *event, Notify_arg arg, Notify_event_type type)
{
    Notify_value result = notify_next_event_func(win, (Notify_event)event, arg, type);
    if (event_action(event) == WIN_RESIZE && viewport) layout();
    return result;
}
static Notify_value destroy(Notify_client object, Destroy_status state)
{
    if (state == DESTROY_CHECKING && start_ok && !mx_browser.closing && !mx_browser.closed) {
        mx_close(); notify_veto_destroy(object); return NOTIFY_DONE;
    }
    if (state == DESTROY_CLEANUP) { alive = 0; script_frame = 0; }
    return notify_next_destroy_func(object, state);
}
static void layout(void)
{
    int width = (int)xv_get(frame, XV_WIDTH), height = (int)xv_get(frame, XV_HEIGHT);
    int bar = tool_size + TOOL_TOP + TOOL_BOTTOM;
    int sw = (int)xv_get(vertical, XV_WIDTH), sh = (int)xv_get(horizontal, XV_HEIGHT);
    int y = bar, h = height - y - sh, w = width - sw;
    if (w < 1) w = 1;
    if (h < 1) h = 1;
    xv_set(toolbar, XV_X, 0, XV_Y, 0, XV_WIDTH, width, XV_HEIGHT, bar, NULL);
    int settings_x = width - TOOL_MARGIN - tool_size;
    address_width = settings_x - TOOL_GAP - address_x;
    if (address_width < 2 * ADDRESS_INSET + 24) address_width = 2 * ADDRESS_INSET + 24;
    xv_set(settings_button, XV_X, settings_x, NULL);
    xv_set(location, XV_X, address_x + ADDRESS_INSET,
        XV_Y, TOOL_TOP + (tool_size - (int)xv_get(location, XV_HEIGHT)) / 2,
        PANEL_VALUE_DISPLAY_WIDTH, address_width - 2 * ADDRESS_INSET, NULL);
    xv_set(viewport, XV_X, 0, XV_Y, y, XV_WIDTH, w, XV_HEIGHT, h, NULL);
    xv_set(vertical, XV_X, w, XV_Y, y, XV_HEIGHT, h, NULL);
    xv_set(horizontal, XV_X, 0, XV_Y, y + h, XV_WIDTH, w, NULL);
    mx_resize(w, h);
    panel_paint(toolbar, PANEL_CLEAR);
    paint_toolbar();
}
static Notify_value tick(Notify_client object, int which)
{
    mx_pump();
    if (!alive) return NOTIFY_DONE;
    if (mx_browser.closing || mx_browser.closed) { xv_destroy_safe(frame); return NOTIFY_DONE; }
    if (revision != mx_browser.revision) {
        revision = mx_browser.revision;
        char title[300]; snprintf(title, sizeof title, "%.255s%sMosaiX", mx_browser.title, mx_browser.title[0] ? " - " : "");
        xv_set(frame, FRAME_LABEL, title, FRAME_LEFT_FOOTER, mx_browser.status, NULL);
        if (strcmp(shown_url, mx_browser.url)) {
            snprintf(shown_url, sizeof shown_url, "%s", mx_browser.url);
            xv_set(location, PANEL_VALUE, shown_url, NULL);
        }
        xv_set(back_button, PANEL_INACTIVE, !mx_browser.back, NULL);
        xv_set(forward_button, PANEL_INACTIVE, !mx_browser.forward, NULL);
        panel_paint(reload_button, PANEL_CLEAR);
        paint_tools();
    }
    if (!scrollbar_pressed && scroll_revision != mx_browser.scroll_revision &&
        mx_browser.width > 0 && mx_browser.height > 0) {
        scroll_revision = mx_browser.scroll_revision;
        xv_set(vertical, SCROLLBAR_OBJECT_LENGTH, mx_browser.content_height,
            SCROLLBAR_VIEW_LENGTH, mx_browser.height, SCROLLBAR_VIEW_START, mx_browser.y, NULL);
        xv_set(horizontal, SCROLLBAR_OBJECT_LENGTH, mx_browser.content_width,
            SCROLLBAR_VIEW_LENGTH, mx_browser.width, SCROLLBAR_VIEW_START, mx_browser.x, NULL);
    }
    return NOTIFY_DONE;
}
int main(int argc, char **argv)
{
    /* CEF initializes Xlib threading; do this before XView opens a Display. */
    XInitThreads();
    int result = mx_execute(argc, argv);
    if (result >= 0) return result;
    mx_preferences_load(&mx_preferences);
    char initial[MX_URL_MAX]; strcpy(initial, mx_preferences.homepage);
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--help")) { puts("Usage: mosaix [URL] [--no-sandbox]\nNative OpenLook browser for X11/XWayland."); return 0; }
        if (argv[i][0] != '-' && !mx_normalize_url(argv[i], initial, sizeof initial)) {
            fprintf(stderr, "MosaiX: invalid address: %s\n", argv[i]); return 2;
        }
    }
    /* CEF receives its original command line; XView gets no Chromium switches. */
    xv_init(NULL);
    frame = xv_create(NULL, FRAME, FRAME_LABEL, "MosaiX", FRAME_SHOW_FOOTER, TRUE,
        FRAME_NO_CONFIRM, TRUE, XV_WIDTH, 1024, XV_HEIGHT, 720, XV_SHOW, FALSE, NULL);
    app_icon_apply(xv_get(frame, FRAME_ICON), "browser");
    xv_set(xv_get(frame, FRAME_ICON), ICON_LABEL, "MosaiX", NULL);
    toolbar = xv_create(frame, PANEL, PANEL_LAYOUT, PANEL_HORIZONTAL, NULL);
    static const unsigned short blank_label[TOOL_LABEL_SIZE] = {0};
    tool_label = xv_create(xv_get(frame, XV_SCREEN), SERVER_IMAGE,
        XV_WIDTH, TOOL_LABEL_SIZE, XV_HEIGHT, TOOL_LABEL_SIZE,
        SERVER_IMAGE_DEPTH, 1, SERVER_IMAGE_BITS, blank_label, NULL);
    tool_size = TOOL_LABEL_SIZE + OLGX_VAR_HEIGHT_BTN_MARGIN;
    back_button = tool_button(MX_BACK, TOOL_MARGIN);
    forward_button = tool_button(MX_FORWARD, TOOL_MARGIN + tool_size + TOOL_GAP);
    reload_button = tool_button(MX_RELOAD, TOOL_MARGIN + 2 * (tool_size + TOOL_GAP));
    home_button = tool_button(MX_HOME, TOOL_MARGIN + 3 * (tool_size + TOOL_GAP));
    address_x = TOOL_MARGIN + 4 * (tool_size + TOOL_GAP);
    location = xv_create(toolbar, PANEL_TEXT, PANEL_LABEL_STRING, "", PANEL_VALUE_UNDERLINED, FALSE,
        XV_X, address_x + ADDRESS_INSET, XV_Y, TOOL_TOP,
        PANEL_VALUE_DISPLAY_LENGTH, 55,
        PANEL_VALUE_STORED_LENGTH, MX_URL_MAX - 1, PANEL_NOTIFY_PROC, navigate, PANEL_NOTIFY_LEVEL, PANEL_SPECIFIED,
        PANEL_NOTIFY_STRING, "\n\r", PANEL_VALUE, initial, NULL);
    location_default_event = (void (*)(Panel_item, Event *))xv_get(location, PANEL_EVENT_PROC);
    xv_set(location, PANEL_EVENT_PROC, location_event, NULL);
    settings_button = tool_button(MX_SETTINGS, 1024 - TOOL_MARGIN - tool_size);
    xv_set(frame, FRAME_MIN_SIZE, address_x + 100 + tool_size, 160, NULL);
    xv_set(toolbar, XV_HEIGHT, tool_size + TOOL_TOP + TOOL_BOTTOM, NULL);
    viewport = xv_create(frame, WINDOW, WIN_BELOW, toolbar, WIN_BORDER, FALSE, NULL);
    vertical = xv_create(frame, SCROLLBAR, SCROLLBAR_DIRECTION, SCROLLBAR_VERTICAL, SCROLLBAR_NOTIFY_CLIENT, viewport,
        SCROLLBAR_PERCENT_OF_DRAG_REPAINTS, 100,
        SCROLLBAR_PIXELS_PER_UNIT, 1, SCROLLBAR_OVERSCROLL, 0, XV_SHOW, TRUE, NULL);
    horizontal = xv_create(frame, SCROLLBAR, SCROLLBAR_DIRECTION, SCROLLBAR_HORIZONTAL, SCROLLBAR_NOTIFY_CLIENT, viewport,
        SCROLLBAR_PERCENT_OF_DRAG_REPAINTS, 100,
        SCROLLBAR_PIXELS_PER_UNIT, 1, SCROLLBAR_OVERSCROLL, 0, XV_SHOW, TRUE, NULL);
    page_menu = make_menu("Page"); add_menu_item(page_menu, "Back", MX_BACK); add_menu_item(page_menu, "Reload", MX_RELOAD);
    add_menu_item(page_menu, "Copy", MX_COPY); add_menu_item(page_menu, "Paste", MX_PASTE); add_menu_item(page_menu, "Select All", MX_SELECT_ALL);
    notify_interpose_event_func(viewport, viewport_event, NOTIFY_SAFE);
    notify_interpose_event_func(toolbar, toolbar_event, NOTIFY_SAFE);
    notify_interpose_event_func(vertical, scrollbar_event, NOTIFY_SAFE);
    notify_interpose_event_func(horizontal, scrollbar_event, NOTIFY_SAFE);
    notify_interpose_event_func(frame, frame_event, NOTIFY_SAFE);
    notify_interpose_destroy_func(frame, destroy);
    layout(); xv_set(frame, XV_SHOW, TRUE, NULL);
    XSync((Display *)xv_get(frame, XV_DISPLAY), False);
    start_ok = mx_start(argc, argv, (unsigned long)xv_get(viewport, XV_XID),
        (int)xv_get(viewport, XV_WIDTH), (int)xv_get(viewport, XV_HEIGHT), initial);
    if (!start_ok) { fprintf(stderr, "MosaiX: %s\n", mx_browser.status); message(mx_browser.status); xv_destroy(frame); mx_shutdown(); return 1; }
    struct itimerval timer = {{0, 10000}, {0, 10000}};
    notify_set_itimer_func(frame, tick, ITIMER_REAL, &timer, NULL);
    xv_main_loop(frame);
    /* Parent destruction is asynchronous in CEF. Keep servicing it before
     * releasing browser references and shutting down Chromium. */
    while (!mx_browser.closed) { struct timespec delay = {0, 10000000}; mx_pump(); nanosleep(&delay, NULL); }
    mx_shutdown();
    mx_toolbar_icons_destroy();
    xv_destroy(tool_label);
    return 0;
}
