/* Composite 13x13 Tango RGBA icons inside native button borders.
 * Alpha is preserved on normal, pressed, and inactive controls. */
#include "toolbar-icons.h"
#include "mosaix.h"
#include "icons/toolbar-rgba.h"
#include <X11/Xutil.h>
#include <X11/extensions/Xrender.h>
#include <xview/cms.h>
#include <stdlib.h>

static Display *display;
static Pixmap pixmaps[5][2];
static Picture pictures[5][2];

static Picture
icon_picture(Xv_window owner, int index, int inactive)
{
    if (pictures[index][inactive]) return pictures[index][inactive];
    XRenderPictFormat *format = XRenderFindStandardFormat(display, PictStandardARGB32);
    if (!format) return None;
    Pixmap pixmap = XCreatePixmap(display, (Drawable)xv_get(owner, XV_XID),
                                 MX_TOOL_ICON_SIZE, MX_TOOL_ICON_SIZE, 32);
    XImage *image = XCreateImage(display, (Visual *)xv_get(owner, XV_VISUAL),
        32, ZPixmap, 0, NULL, MX_TOOL_ICON_SIZE, MX_TOOL_ICON_SIZE, 32, 0);
    if (!image) { XFreePixmap(display, pixmap); return None; }
    image->data = calloc(MX_TOOL_ICON_SIZE, image->bytes_per_line);
    if (!image->data) {
        XDestroyImage(image); XFreePixmap(display, pixmap); return None;
    }
    for (int y = 0; y < MX_TOOL_ICON_SIZE; ++y) {
        for (int x = 0; x < MX_TOOL_ICON_SIZE; ++x) {
            const unsigned char *p = toolbar_rgba[index] + 4 * (y * MX_TOOL_ICON_SIZE + x);
            unsigned long alpha = inactive ? p[3] * 2 / 5 : p[3];
            unsigned long pixel = (alpha << 24) | (p[0] * alpha / 255 << 16) |
                                  (p[1] * alpha / 255 << 8) | (p[2] * alpha / 255);
            XPutPixel(image, x, y, pixel);
        }
    }
    GC gc = XCreateGC(display, pixmap, 0, NULL);
    if (!gc) { XDestroyImage(image); XFreePixmap(display, pixmap); return None; }
    XPutImage(display, pixmap, gc, image, 0, 0, 0, 0, MX_TOOL_ICON_SIZE, MX_TOOL_ICON_SIZE);
    XFreeGC(display, gc);
    XDestroyImage(image);
    pixmaps[index][inactive] = pixmap;
    pictures[index][inactive] = XRenderCreatePicture(display, pixmap, format, 0, NULL);
    return pictures[index][inactive];
}

void
mx_toolbar_icon_paint(Xv_window owner, int action, int x, int y, int inactive)
{
    display = (Display *)xv_get(owner, XV_DISPLAY);
    Drawable window = (Drawable)xv_get(owner, XV_XID);
    if (action == MX_SETTINGS) {
        GC gc = XCreateGC(display, window, 0, NULL);
        if (!gc) return;
        Cms cms = xv_get(owner, WIN_CMS);
        XSetForeground(display, gc, (unsigned long)xv_get(cms, CMS_FOREGROUND_PIXEL));
        for (int row = 0; row < 3; ++row)
            XFillRectangle(display, window, gc, x + 2, y + 2 + 4 * row, 9, 1);
        XFreeGC(display, gc);
        return;
    }
    int index = action - MX_BACK;
    if (index < 0 || index >= 5) return;
    Picture source = icon_picture(owner, index, !!inactive);
    XRenderPictFormat *format = XRenderFindVisualFormat(display, (Visual *)xv_get(owner, XV_VISUAL));
    if (!source || !format) return;
    Picture destination = XRenderCreatePicture(display, window, format, 0, NULL);
    XRenderComposite(display, PictOpOver, source, None, destination, 0, 0, 0, 0,
                     x, y, MX_TOOL_ICON_SIZE, MX_TOOL_ICON_SIZE);
    XRenderFreePicture(display, destination);
}

void
mx_toolbar_icons_destroy(void)
{
    if (!display) return;
    for (int i = 0; i < 5; ++i) {
        for (int state = 0; state < 2; ++state) {
            if (pictures[i][state]) XRenderFreePicture(display, pictures[i][state]);
            if (pixmaps[i][state]) XFreePixmap(display, pixmaps[i][state]);
            pictures[i][state] = None;
            pixmaps[i][state] = None;
        }
    }
    display = NULL;
}
