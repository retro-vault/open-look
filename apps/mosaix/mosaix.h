/* MosaiX: native OpenLook browser chrome and windowed CEF integration. */
#ifndef MOSAIX_H
#define MOSAIX_H
#include <stddef.h>
#define MX_URL_MAX 2048
typedef struct {
    char homepage[MX_URL_MAX];
    int zoom, persistent, cookies, session_cookies, javascript, images;
} MxPreferences;
typedef struct {
    char url[MX_URL_MAX], title[256], status[512];
    int loading, back, forward, closing, closed;
    int x, y, width, height, content_width, content_height;
    unsigned revision, scroll_revision;
} MxBrowser;
extern MxPreferences mx_preferences;
extern MxBrowser mx_browser;
int mx_normalize_url(const char *input, char *output, size_t capacity);
void mx_preferences_load(MxPreferences *prefs);
int mx_preferences_save(const MxPreferences *prefs);
int mx_path(char *out, size_t size, const char *xdg, const char *fallback);
/* Run and exit CEF child processes; return -1 only in the browser process. */
int mx_execute(int argc, char **argv);
int mx_start(int argc, char **argv, unsigned long parent, int width, int height, const char *url);
void mx_pump(void);
void mx_shutdown(void);
void mx_close(void);
void mx_resize(int width, int height);
void mx_navigate(const char *url);
void mx_action(int action);
void mx_zoom(int percent);
void mx_scroll(int x, int y);
void mx_clear(int cache);
void mx_dialog_reply(int accept, const char *text);
void mx_ui_dialog(const char *origin, const char *message, const char *text, int prompt);
void mx_ui_dialog_dismiss(void);
void mx_ui_command(int action);
void mx_ui_location(void);
void mx_ui_context(int x, int y);
enum { MX_BACK=1, MX_FORWARD, MX_RELOAD, MX_STOP, MX_HOME, MX_COPY, MX_CUT, MX_PASTE, MX_SELECT_ALL, MX_FIND, MX_SETTINGS, MX_QUIT, MX_ABOUT, MX_ZOOM_IN, MX_ZOOM_OUT, MX_ZOOM_RESET };
#endif
