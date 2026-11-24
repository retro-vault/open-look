#define _POSIX_C_SOURCE 200809L
#include "mosaix.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

int mx_normalize_url(const char *input, char *output, size_t capacity)
{
    size_t n;
    const char *prefix = "";
    if (!input || !(n = strlen(input))) return 0;
    for (size_t i = 0; i < n; ++i)
        if ((unsigned char)input[i] <= 32 || (unsigned char)input[i] == 127) return 0;
    if (strcasecmp(input, "about:blank") && strncasecmp(input, "http://", 7) &&
        strncasecmp(input, "https://", 8) && strncasecmp(input, "file:///", 8)) {
        size_t host = strcspn(input, "/:");
        int local = (host == 9 && !strncasecmp(input, "localhost", 9)) ||
                    !strncmp(input, "127.", 4) || !strncmp(input, "[::1]", 5);
        const char *colon = strchr(input, ':');
        /* A port is allowed; arbitrary executable URL schemes are not. */
        if (strstr(input, "://") || (colon && !local && !isdigit((unsigned char)colon[1]))) return 0;
        prefix = local ? "http://" : "https://";
    }
    if (strlen(prefix) + n >= capacity) return 0;
    snprintf(output, capacity, "%s%s", prefix, input);
    return 1;
}

static int mkdirs(char *path)
{
    for (char *p = path + 1; ; ++p) {
        if (*p == '/' || !*p) {
            char saved = *p;
            struct stat st;
            *p = 0;
            if (mkdir(path, 0700) && errno != EEXIST) { *p = saved; return 0; }
            if (stat(path, &st) || !S_ISDIR(st.st_mode)) { *p = saved; return 0; }
            *p = saved;
            if (!saved) break;
        }
    }
    return 1;
}
int mx_path(char *out, size_t size, const char *xdg, const char *fallback)
{
    const char *root = getenv(xdg), *home = getenv("HOME");
    int n;
    if (root && root[0] == '/') n = snprintf(out, size, "%s/openlook/mosaix", root);
    else if (home && home[0] == '/') n = snprintf(out, size, "%s/%s/openlook/mosaix", home, fallback);
    else return 0;
    return n > 0 && (size_t)n < size && mkdirs(out);
}
static int config_path(char *out, size_t size)
{
    char dir[4096];
    if (!mx_path(dir, sizeof dir, "XDG_CONFIG_HOME", ".config")) return 0;
    int n = snprintf(out, size, "%s/preferences", dir);
    return n > 0 && (size_t)n < size;
}
void mx_preferences_load(MxPreferences *p)
{
    char path[4096], line[4096], url[MX_URL_MAX];
    *p = (MxPreferences){.homepage="https://github.com/retro-vault/open-look/", .zoom=100,
        .persistent=1, .cookies=1, .javascript=1, .images=1};
    if (!config_path(path, sizeof path)) return;
    FILE *f = fopen(path, "r");
    if (!f) return;
    while (fgets(line, sizeof line, f)) {
        line[strcspn(line, "\r\n")] = 0;
        char *v = strchr(line, '=');
        if (!v) continue;
        *v++ = 0;
        if (!strcmp(line, "homepage")) {
            if (mx_normalize_url(v, url, sizeof url)) strcpy(p->homepage, url);
            continue;
        }
        char *end;
        long n = strtol(v, &end, 10);
        if (!*v || *end) continue;
        if (!strcmp(line, "zoom") && n >= 50 && n <= 200) p->zoom = (int)n;
        if (n != 0 && n != 1) continue;
#define READ_BOOL(name) if (!strcmp(line, #name)) p->name = (int)n
        READ_BOOL(persistent); READ_BOOL(cookies); READ_BOOL(session_cookies);
        READ_BOOL(javascript); READ_BOOL(images);
#undef READ_BOOL
    }
    fclose(f);
}
int mx_preferences_save(const MxPreferences *p)
{
    char path[4096], temp[4110], normalized[MX_URL_MAX];
    if (!config_path(path, sizeof path) || !mx_normalize_url(p->homepage, normalized, sizeof normalized) ||
        p->zoom < 50 || p->zoom > 200) return 0;
    snprintf(temp, sizeof temp, "%s.XXXXXX", path);
    int fd = mkstemp(temp);
    if (fd < 0) return 0;
    FILE *f = fdopen(fd, "w");
    if (!f) { close(fd); unlink(temp); return 0; }
    int ok = fprintf(f, "homepage=%s\nzoom=%d\npersistent=%d\ncookies=%d\nsession_cookies=%d\njavascript=%d\nimages=%d\n",
        normalized, p->zoom, !!p->persistent, !!p->cookies, !!p->session_cookies, !!p->javascript, !!p->images) > 0;
    if (fflush(f) || fsync(fd)) ok = 0;
    if (fclose(f)) ok = 0;
    if (ok && rename(temp, path) == 0) return 1;
    unlink(temp);
    return 0;
}
