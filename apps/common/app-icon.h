#ifndef OPENLOOK_APP_ICON_H
#define OPENLOOK_APP_ICON_H
#include <xview/xview.h>
#include <xview/icon.h>
#include <xview/svrimage.h>

typedef struct {
    Server_image image, mask;
} AppIconImage;

/* Images are cached per screen/colormap for the lifetime of the application. */
const AppIconImage *app_icon_image(Xv_window owner, const char *name);
int app_icon_apply(Icon icon, const char *name);
#endif
