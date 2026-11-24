/* Native, consistently sized color glyphs for the MosaiX navigation toolbar. */
#ifndef MOSAIX_TOOLBAR_ICONS_H
#define MOSAIX_TOOLBAR_ICONS_H
#include <xview/xview.h>
#define MX_TOOL_ICON_SIZE 13

/* Composite a transparent glyph onto owner's window at x,y. Inactive glyphs
 * retain their color with reduced opacity. Owns its cached XRender pictures. */
void mx_toolbar_icon_paint(Xv_window owner, int action, int x, int y, int inactive);
/* Release cached server resources after the toolbar has been destroyed. */
void mx_toolbar_icons_destroy(void);
#endif
