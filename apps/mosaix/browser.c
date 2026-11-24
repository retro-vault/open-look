/* CEF C-API integration, informed by Gemscape's lifecycle and scroll metrics.
 * CEF owns a native child window; no framebuffer copies or input emulation.
 */
#define _POSIX_C_SOURCE 200809L
#include "mosaix.h"
#include "include/capi/cef_app_capi.h"
#include "include/capi/cef_client_capi.h"
#include "include/capi/cef_parser_capi.h"
#include "include/capi/cef_devtools_message_observer_capi.h"
#include "include/cef_api_hash.h"
#include <X11/Xlib.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>

MxBrowser mx_browser;
static cef_browser_t *browser;
static cef_registration_t *registration;
static cef_jsdialog_callback_t *dialog_callback;
static int initialized, profile_lock = -1, next_id, metrics_id, clear_id, scroll_id;
static unsigned scroll_generation, metrics_generation;
static unsigned ticks;
static Display *display;
static Window child;

/* Each process-lifetime handler retains its initial reference. The C API
 * transfers references for object parameters as well as returned objects. */
#define HANDLER(name, type) \
    static struct { type api; atomic_int refs; } name; \
    static void CEF_CALLBACK name##_add(cef_base_ref_counted_t *b) { (void)b; ++name.refs; } \
    static int CEF_CALLBACK name##_release(cef_base_ref_counted_t *b) { (void)b; --name.refs; return 0; } \
    static int CEF_CALLBACK name##_one(cef_base_ref_counted_t *b) { (void)b; return name.refs == 1; } \
    static int CEF_CALLBACK name##_any(cef_base_ref_counted_t *b) { (void)b; return name.refs > 0; }
#define INIT(name) do { name.refs = 1; name.api.base.size = sizeof(name.api); \
    name.api.base.add_ref = name##_add; name.api.base.release = name##_release; \
    name.api.base.has_one_ref = name##_one; name.api.base.has_at_least_one_ref = name##_any; } while (0)
#define DROP(p) do { if (p) (p)->base.release(&(p)->base); } while (0)
HANDLER(app, cef_app_t)
HANDLER(client, cef_client_t)
HANDLER(life, cef_life_span_handler_t)
HANDLER(load, cef_load_handler_t)
HANDLER(view, cef_display_handler_t)
HANDLER(observer, cef_dev_tools_message_observer_t)
HANDLER(dialog, cef_jsdialog_handler_t)
HANDLER(context_menu, cef_context_menu_handler_t)
HANDLER(keyboard, cef_keyboard_handler_t)

static void string(cef_string_t *s, const char *v) { cef_string_utf8_to_utf16(v, strlen(v), s); }
static void text(char *out, size_t size, const cef_string_t *s)
{
    cef_string_utf8_t v = {0};
    if (s) cef_string_utf16_to_utf8(s->str, s->length, &v);
    snprintf(out, size, "%s", v.str ? v.str : "");
    cef_string_utf8_clear(&v);
}
static void status(const char *s) { snprintf(mx_browser.status, sizeof mx_browser.status, "%s", s); ++mx_browser.revision; }
static void CEF_CALLBACK command_line(cef_app_t *self, const cef_string_t *type, cef_command_line_t *cmd)
{
    cef_string_t key = {0}, value = {0};
    string(&key, "ozone-platform"); string(&value, "x11");
    cmd->append_switch_with_value(cmd, &key, &value);
    cef_string_clear(&key); cef_string_clear(&value);
    DROP(cmd);
}
int mx_execute(int argc, char **argv)
{
    INIT(app); app.api.on_before_command_line_processing = command_line;
    cef_main_args_t args = {argc, argv};
    (void)cef_api_hash(CEF_API_VERSION, 0);
    int result = cef_execute_process(&args, &app.api, NULL);
    /* Chromium changes the stack canary in forked children. Exit here instead
     * of returning through frames that saved the parent's canary. */
    if (result >= 0) exit(result);
    return result;
}
static void execute(const char *script)
{
    if (!browser || mx_browser.closed) return;
    cef_frame_t *frame = browser->get_main_frame(browser);
    if (!frame) return;
    cef_string_t code = {0}, origin = {0};
    string(&code, script); string(&origin, "mosaix://chrome");
    frame->execute_java_script(frame, &code, &origin, 1);
    cef_string_clear(&code); cef_string_clear(&origin); DROP(frame);
}
static int devtools(const char *method, cef_dictionary_value_t *params)
{
    if (!browser || mx_browser.closed) return 0;
    cef_browser_host_t *host = browser->get_host(browser);
    cef_string_t name = {0}; string(&name, method);
    /* CEF consumes a reference to object arguments; the caller retains its
     * dictionary until it releases it after this call. */
    if (params) params->base.add_ref(&params->base);
    int id = host->execute_dev_tools_method(host, ++next_id, &name, params);
    cef_string_clear(&name); DROP(host);
    return id;
}
static cef_dictionary_value_t *dictionary(cef_dictionary_value_t *d, const char *name)
{
    cef_string_t key = {0}; string(&key, name);
    cef_dictionary_value_t *v = d->get_dictionary(d, &key);
    cef_string_clear(&key); return v;
}
static int number(cef_dictionary_value_t *d, const char *name)
{
    cef_string_t key = {0}; string(&key, name);
    double v = d->get_type(d, &key) == VTYPE_INT ? d->get_int(d, &key) : d->get_double(d, &key);
    cef_string_clear(&key);
    return isfinite(v) ? (int)fmax(0, fmin(v, 100000000)) : 0;
}
static void CEF_CALLBACK result(cef_dev_tools_message_observer_t *self, cef_browser_t *b,
                               int id, int success, const void *data, size_t size)
{
    DROP(b);
    if (id == clear_id) { clear_id = 0; status(success ? "Browsing data cleared." : "Could not clear browsing data."); }
    if (id == scroll_id) { scroll_id = 0; ticks = 9; return; }
    if (id != metrics_id) return;
    metrics_id = 0;
    /* A snapshot requested before the latest drag must not move its thumb
     * backward. Query again after the renderer acknowledges the scroll. */
    if (scroll_id || metrics_generation != scroll_generation) return;
    if (!success || !data || size > 65536) return;
    cef_value_t *v = cef_parse_json_buffer(data, size, JSON_PARSER_RFC);
    if (!v) return;
    cef_dictionary_value_t *d = v->get_dictionary(v);
    if (d) {
        cef_dictionary_value_t *viewport = dictionary(d, "cssLayoutViewport"), *content = dictionary(d, "cssContentSize");
        if (viewport && content) {
            mx_browser.x = number(viewport, "pageX"); mx_browser.y = number(viewport, "pageY");
            mx_browser.width = number(viewport, "clientWidth"); mx_browser.height = number(viewport, "clientHeight");
            mx_browser.content_width = number(content, "width"); mx_browser.content_height = number(content, "height");
            ++mx_browser.scroll_revision;
        }
        DROP(viewport); DROP(content); DROP(d);
    }
    DROP(v);
}
static void CEF_CALLBACK load_end(cef_load_handler_t *self, cef_browser_t *b, cef_frame_t *frame, int code)
{
    if (frame->is_main(frame)) {
        execute("(()=>{const s=document.createElement('style');s.textContent='html::-webkit-scrollbar,body::-webkit-scrollbar{width:0!important;height:0!important}html,body{scrollbar-width:none!important}';(document.head||document.documentElement).appendChild(s)})()");
        mx_zoom(mx_preferences.zoom);
    }
    DROP(frame); DROP(b);
}
static void CEF_CALLBACK loading(cef_load_handler_t *self, cef_browser_t *b, int busy, int back, int forward)
{
    mx_browser.loading = busy; mx_browser.back = back; mx_browser.forward = forward;
    status(busy ? "Loading..." : "Ready"); DROP(b);
}
static void CEF_CALLBACK load_error(cef_load_handler_t *self, cef_browser_t *b, cef_frame_t *frame,
                                    cef_errorcode_t code, const cef_string_t *message, const cef_string_t *url)
{
    if (frame->is_main(frame) && code != ERR_ABORTED) {
        text(mx_browser.status, sizeof mx_browser.status, message); ++mx_browser.revision;
    }
    DROP(frame); DROP(b);
}
static void CEF_CALLBACK address(cef_display_handler_t *self, cef_browser_t *b, cef_frame_t *frame, const cef_string_t *url)
{
    if (frame->is_main(frame)) { text(mx_browser.url, sizeof mx_browser.url, url); ++mx_browser.revision; }
    DROP(frame); DROP(b);
}
static void CEF_CALLBACK title(cef_display_handler_t *self, cef_browser_t *b, const cef_string_t *value)
{
    text(mx_browser.title, sizeof mx_browser.title, value); ++mx_browser.revision; DROP(b);
}
static int CEF_CALLBACK do_close(cef_life_span_handler_t *self, cef_browser_t *b)
{ mx_browser.closing = 1; DROP(b); return 0; }
static void CEF_CALLBACK before_close(cef_life_span_handler_t *self, cef_browser_t *b)
{
    mx_browser.closed = 1; child = 0; DROP(b);
}
static int CEF_CALLBACK popup(cef_life_span_handler_t *self, cef_browser_t *b, cef_frame_t *f,
    int id, const cef_string_t *url, const cef_string_t *name, cef_window_open_disposition_t disposition,
    int gesture, const cef_popup_features_t *features, cef_window_info_t *window,
    cef_client_t **c, cef_browser_settings_t *settings, cef_dictionary_value_t **extra, int *no_access)
{
    char target[MX_URL_MAX]; text(target, sizeof target, url);
    if (gesture) mx_navigate(target); else status("Popup blocked.");
    DROP(b); DROP(f);
    if (c) { DROP(*c); *c = NULL; }
    if (extra) { DROP(*extra); *extra = NULL; }
    return 1;
}
static int CEF_CALLBACK script_dialog(cef_jsdialog_handler_t *self, cef_browser_t *b,
    const cef_string_t *origin, cef_jsdialog_type_t type, const cef_string_t *message,
    const cef_string_t *default_prompt, cef_jsdialog_callback_t *callback, int *suppress)
{
    char from[MX_URL_MAX], msg[2048], initial[1024];
    text(from, sizeof from, origin); text(msg, sizeof msg, message); text(initial, sizeof initial, default_prompt);
    if (dialog_callback) { *suppress = 1; DROP(callback); DROP(b); return 0; }
    dialog_callback = callback;
    mx_ui_dialog(from, msg, initial, type == JSDIALOGTYPE_PROMPT);
    DROP(b); return 1;
}
static int CEF_CALLBACK unload_dialog(cef_jsdialog_handler_t *self, cef_browser_t *b,
    const cef_string_t *message, int reload, cef_jsdialog_callback_t *callback)
{
    char msg[2048]; text(msg, sizeof msg, message);
    if (dialog_callback) mx_dialog_reply(0, "");
    dialog_callback = callback;
    mx_ui_dialog("Leave this page?", msg, "", 0);
    DROP(b); return 1;
}
static void CEF_CALLBACK reset_dialog(cef_jsdialog_handler_t *self, cef_browser_t *b)
{
    DROP(dialog_callback); dialog_callback = NULL; mx_ui_dialog_dismiss(); DROP(b);
}
void mx_dialog_reply(int accept, const char *value)
{
    cef_jsdialog_callback_t *callback = dialog_callback;
    if (!callback) return;
    dialog_callback = NULL;
    cef_string_t s = {0}; string(&s, value);
    callback->cont(callback, accept, &s); cef_string_clear(&s); DROP(callback);
}
static int CEF_CALLBACK run_context(cef_context_menu_handler_t *self, cef_browser_t *b,
    cef_frame_t *f, cef_context_menu_params_t *params, cef_menu_model_t *model, cef_run_context_menu_callback_t *cb)
{
    int x = params->get_xcoord(params), y = params->get_ycoord(params);
    cb->cancel(cb); DROP(cb); DROP(model); DROP(params); DROP(f); DROP(b);
    mx_ui_context(x, y); return 1;
}
static int CEF_CALLBACK key_event(cef_keyboard_handler_t *self, cef_browser_t *b,
                                 const cef_key_event_t *event, cef_event_handle_t os_event, int *shortcut)
{
    int action = 0;
    if (event->type == KEYEVENT_RAWKEYDOWN) {
        if (event->modifiers & EVENTFLAG_CONTROL_DOWN) {
            if (event->windows_key_code == 'L') { mx_ui_location(); DROP(b); return 1; }
            if (event->windows_key_code == 188) action = MX_SETTINGS;
            if (event->windows_key_code == 'R') action = MX_RELOAD;
            if (event->windows_key_code == 'Q') action = MX_QUIT;
        }
        if (event->modifiers & EVENTFLAG_ALT_DOWN) {
            if (event->windows_key_code == 37) action = MX_BACK;
            if (event->windows_key_code == 39) action = MX_FORWARD;
            if (event->windows_key_code == 36) action = MX_HOME;
        }
        if (event->windows_key_code == 116) action = MX_RELOAD;
    }
    if (action) mx_ui_command(action);
    DROP(b); return action != 0;
}
#define GETTER(fn, field, type) static type *CEF_CALLBACK fn(cef_client_t *self) { field##_add(&field.api.base); return &field.api; }
GETTER(get_life, life, cef_life_span_handler_t)
GETTER(get_load, load, cef_load_handler_t)
GETTER(get_view, view, cef_display_handler_t)
GETTER(get_dialog, dialog, cef_jsdialog_handler_t)
GETTER(get_context, context_menu, cef_context_menu_handler_t)
GETTER(get_keyboard, keyboard, cef_keyboard_handler_t)

int mx_start(int argc, char **argv, unsigned long parent, int width, int height, const char *url)
{
    char profile[4096], path[8192], executable[4096], runtime[4200];
    if (!mx_path(profile, sizeof profile, "XDG_DATA_HOME", ".local/share")) { status("Cannot create browser profile."); return 0; }
    snprintf(path, sizeof path, "%s/mosaix.lock", profile);
    profile_lock = open(path, O_CREAT|O_RDWR|O_CLOEXEC|O_NOFOLLOW, 0600);
    if (profile_lock < 0 || flock(profile_lock, LOCK_EX|LOCK_NB)) { status("Browser profile is already in use or cannot be locked."); return 0; }
    ssize_t n = readlink("/proc/self/exe", executable, sizeof executable - 1);
    if (n < 0) { status("Cannot locate the CEF runtime."); return 0; }
    executable[n] = 0; *strrchr(executable, '/') = 0;
    snprintf(runtime, sizeof runtime, "%s/../lib/mosaix/cef", executable);
    cef_settings_t settings = {0}; settings.size = sizeof settings;
    /* The Docker debug launcher opts out because its seccomp policy blocks
     * Chromium's namespaces. Native launches keep the sandbox enabled. */
    const char *disable_sandbox = getenv("MOSAIX_DISABLE_SANDBOX");
    if (disable_sandbox && !strcmp(disable_sandbox, "1")) settings.no_sandbox = 1;
    for (int i = 1; i < argc; ++i) if (!strcmp(argv[i], "--no-sandbox")) settings.no_sandbox = 1;
    settings.log_severity = LOGSEVERITY_WARNING;
    string(&settings.root_cache_path, profile);
    if (mx_preferences.persistent) {
        snprintf(path, sizeof path, "%s/profile", profile); string(&settings.cache_path, path);
    }
    settings.persist_session_cookies = mx_preferences.session_cookies;
    settings.cookieable_schemes_exclude_defaults = !mx_preferences.cookies;
    string(&settings.resources_dir_path, runtime);
    snprintf(path, sizeof path, "%s/locales", runtime); string(&settings.locales_dir_path, path);
    string(&settings.locale, "en-US");
    if (mx_path(path, sizeof path, "XDG_STATE_HOME", ".local/state")) {
        char log[8208]; snprintf(log, sizeof log, "%s/cef.log", path); string(&settings.log_file, log);
    }
    cef_main_args_t args = {argc, argv};
    initialized = cef_initialize(&args, &settings, &app.api, NULL);
    cef_string_clear(&settings.root_cache_path); cef_string_clear(&settings.cache_path);
    cef_string_clear(&settings.resources_dir_path); cef_string_clear(&settings.locales_dir_path);
    cef_string_clear(&settings.locale); cef_string_clear(&settings.log_file);
    if (!initialized) { status("CEF initialization failed; see the MosaiX state log."); return 0; }
    INIT(client); INIT(life); INIT(load); INIT(view); INIT(observer); INIT(dialog); INIT(context_menu); INIT(keyboard);
    life.api.do_close = do_close; life.api.on_before_close = before_close; life.api.on_before_popup = popup;
    load.api.on_load_end = load_end; load.api.on_loading_state_change = loading; load.api.on_load_error = load_error;
    view.api.on_address_change = address; view.api.on_title_change = title;
    observer.api.on_dev_tools_method_result = result;
    dialog.api.on_jsdialog = script_dialog; dialog.api.on_before_unload_dialog = unload_dialog; dialog.api.on_reset_dialog_state = reset_dialog;
    context_menu.api.run_context_menu = run_context;
    keyboard.api.on_pre_key_event = key_event;
    client.api.get_life_span_handler = get_life; client.api.get_load_handler = get_load;
    client.api.get_display_handler = get_view; client.api.get_jsdialog_handler = get_dialog;
    client.api.get_context_menu_handler = get_context; client.api.get_keyboard_handler = get_keyboard;
    cef_window_info_t window = {0}; window.size = sizeof window;
    window.parent_window = parent; window.bounds.width = width; window.bounds.height = height;
    window.runtime_style = CEF_RUNTIME_STYLE_ALLOY;
    cef_browser_settings_t prefs = {0}; prefs.size = sizeof prefs;
    prefs.javascript = mx_preferences.javascript ? STATE_ENABLED : STATE_DISABLED;
    prefs.image_loading = mx_preferences.images ? STATE_ENABLED : STATE_DISABLED;
    cef_string_t initial = {0}; string(&initial, url);
    browser = cef_browser_host_create_browser_sync(&window, &client.api, &initial, &prefs, NULL, NULL);
    cef_string_clear(&initial);
    if (!browser) { status("Could not create the CEF browser window."); return 0; }
    cef_browser_host_t *host = browser->get_host(browser);
    child = host->get_window_handle(host);
    registration = host->add_dev_tools_message_observer(host, &observer.api);
    DROP(host);
    display = XOpenDisplay(NULL);
    mx_zoom(mx_preferences.zoom);
    return 1;
}
void mx_navigate(const char *url)
{
    if (!browser || mx_browser.closed) return;
    cef_frame_t *frame = browser->get_main_frame(browser);
    if (!frame) return;
    cef_string_t value = {0}; string(&value, url); frame->load_url(frame, &value); cef_string_clear(&value); DROP(frame);
}
void mx_action(int action)
{
    if (!browser || mx_browser.closed) return;
    switch (action) {
    case MX_BACK: browser->go_back(browser); break;
    case MX_FORWARD: browser->go_forward(browser); break;
    case MX_RELOAD: browser->reload(browser); break;
    case MX_STOP: browser->stop_load(browser); break;
    case MX_HOME: mx_navigate(mx_preferences.homepage); break;
    case MX_QUIT: mx_close(); break;
    default: {
        cef_frame_t *frame = browser->get_focused_frame(browser);
        if (!frame) break;
        if (action == MX_COPY) frame->copy(frame);
        if (action == MX_CUT) frame->cut(frame);
        if (action == MX_PASTE) frame->paste(frame);
        if (action == MX_SELECT_ALL) frame->select_all(frame);
        DROP(frame);
    } break;
    }
}
void mx_resize(int width, int height)
{
    if (display && child && width > 0 && height > 0) { XResizeWindow(display, child, width, height); XFlush(display); }
}
void mx_zoom(int percent)
{
    if (!browser || mx_browser.closed) return;
    cef_browser_host_t *host = browser->get_host(browser);
    host->set_zoom_level(host, log(percent / 100.0) / log(1.2)); DROP(host);
}
void mx_scroll(int x, int y)
{
    if (!browser || mx_browser.closed) return;
    cef_dictionary_value_t *params = cef_dictionary_value_create();
    if (!params) return;
    cef_string_t key = {0}, value = {0};
    char code[128];
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    snprintf(code, sizeof code,
        "window.scrollTo({left:%d,top:%d,behavior:'instant'})", x, y);
    string(&key, "expression"); string(&value, code);
    params->set_string(params, &key, &value);
    cef_string_clear(&key); cef_string_clear(&value);
    ++scroll_generation;
    scroll_id = devtools("Runtime.evaluate", params);
    DROP(params);
    /* Preserve the other axis when successive horizontal/vertical requests
     * arrive before Chromium has returned fresh layout metrics. */
    mx_browser.x = x; mx_browser.y = y;
}
void mx_clear(int cache)
{
    if (clear_id) return;
    clear_id = devtools(cache ? "Network.clearBrowserCache" : "Network.clearBrowserCookies", NULL);
    status(clear_id ? "Clearing browsing data..." : "Could not request data clearing.");
}
void mx_close(void)
{
    if (!browser || mx_browser.closed) return;
    cef_browser_host_t *host = browser->get_host(browser);
    host->close_browser(host, 0); DROP(host);
}
void mx_pump(void)
{
    if (!initialized) return;
    cef_do_message_loop_work();
    if (browser && !mx_browser.closed && ++ticks % 10 == 0 && !metrics_id && !scroll_id) {
        metrics_generation = scroll_generation;
        metrics_id = devtools("Page.getLayoutMetrics", NULL);
    }
}
void mx_shutdown(void)
{
    DROP(registration); registration = NULL;
    DROP(browser); browser = NULL;
    DROP(dialog_callback); dialog_callback = NULL;
    if (initialized) { cef_shutdown(); initialized = 0; }
    if (display) { XCloseDisplay(display); display = NULL; }
    if (profile_lock >= 0) { close(profile_lock); profile_lock = -1; }
}
