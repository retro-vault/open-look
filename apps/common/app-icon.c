/* Shared application artwork for staged and installed OpenLook desktops. */
#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 200809L
#include "app-icon.h"
#include <X11/xpm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct IconEntry {
    Display *display;
    Xv_opaque screen;
    Colormap colormap;
    unsigned depth;
    char name[64];
    AppIconImage art;
    struct IconEntry *next;
} IconEntry;
static IconEntry *images;

/* Keep assets relocatable, with the desktop's explicit OPENWINHOME taking priority. */
static int icon_path(char *path, size_t capacity, const char *name)
{
    char executable[4096];
    const char *root = getenv("OPENWINHOME");
    int n;
    if (root && *root) {
        n = snprintf(path, capacity, "%s/share/openlook/icons/bw-theme/%s.xpm", root, name);
    } else {
        ssize_t length = readlink("/proc/self/exe", executable, sizeof executable - 1);
        if (length < 0 || length >= (ssize_t)sizeof executable - 1) return 0;
        executable[length] = '\0';
        char *slash = strrchr(executable, '/');
        if (!slash) return 0;
        *slash = '\0';
        n = snprintf(path, capacity, "%s/../share/openlook/icons/bw-theme/%s.xpm", executable, name);
    }
    return n > 0 && (size_t)n < capacity;
}

/* Preserve both black and white pixels; only the separate mask is transparent. */
const AppIconImage *app_icon_image(Xv_window owner, const char *name)
{
    char path[4300];
    Display *display = (Display *)xv_get(owner, XV_DISPLAY);
    Window window = (Window)xv_get(owner, XV_XID);
    Xv_opaque screen = xv_get(owner, XV_SCREEN);
    XWindowAttributes wa;
    if (strlen(name) >= sizeof images->name || strchr(name, '/') ||
        !icon_path(path, sizeof path, name) || !XGetWindowAttributes(display, window, &wa))
        return NULL;
    for (IconEntry *entry = images; entry; entry = entry->next)
        if (entry->display == display && entry->screen == screen &&
            entry->colormap == wa.colormap && entry->depth == (unsigned)wa.depth &&
            !strcmp(entry->name, name)) return &entry->art;

    XpmAttributes attr = {0};
    attr.valuemask = XpmVisual | XpmColormap | XpmDepth | XpmSize;
    attr.visual = wa.visual; attr.colormap = wa.colormap; attr.depth = wa.depth;
    Pixmap pixels = None, mask = None;
    int result = XpmReadFileToPixmap(display, window, path, &pixels, &mask, &attr);
    int valid_size = attr.width == 64 && attr.height == 64;
    XpmFreeAttributes(&attr);
    if (result < 0 || !valid_size) {
        if (pixels) XFreePixmap(display, pixels);
        if (mask) XFreePixmap(display, mask);
        fprintf(stderr, "OpenLook: cannot load 64x64 icon %s\n", path);
        return NULL;
    }
    IconEntry *entry = calloc(1, sizeof *entry);
    if (!entry) {
        XFreePixmap(display, pixels);
        if (mask) XFreePixmap(display, mask);
        return NULL;
    }
    entry->display = display; entry->screen = screen;
    entry->colormap = wa.colormap; entry->depth = wa.depth;
    strcpy(entry->name, name);
    entry->art.image = xv_create(screen, SERVER_IMAGE, SERVER_IMAGE_PIXMAP, pixels, NULL);
    if (mask) entry->art.mask = xv_create(screen, SERVER_IMAGE, SERVER_IMAGE_PIXMAP, mask, NULL);
    entry->next = images; images = entry;
    return &entry->art;
}

/* Retain each application's label, callbacks, and WM icon window. */
int app_icon_apply(Icon icon, const char *name)
{
    if (!icon) return 0;
    const AppIconImage *art = app_icon_image(icon, name);
    if (!art) return 0;
    xv_set(icon, ICON_IMAGE, art->image, ICON_MASK_IMAGE, art->mask,
        XV_WIDTH, 64, XV_HEIGHT, 64, NULL);
    return 1;
}
