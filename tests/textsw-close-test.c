/* A cancelled close check must not detach a dialog from its text owner. */
#include <stdio.h>
#include <stdlib.h>
#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/textsw.h>
#include <xview_private/txt_impl.h>
extern void textsw_create_popup_frame(Textsw_view_handle, int);
extern int LOAD_FILE_POPUP_KEY, SEARCH_POPUP_KEY;
static void expect(int condition, const char *message)
{
    if (!condition) { fprintf(stderr, "%s\n", message); exit(1); }
}
int main(int argc, char **argv)
{
    xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, NULL);
    for (int i = 0; i < 12; ++i) {
        Frame f = xv_create(NULL, FRAME, FRAME_NO_CONFIRM, TRUE, XV_SHOW, FALSE, NULL);
        Textsw t = xv_create(f, TEXTSW, NULL);
        int type = i % 2 ? TEXTSW_MENU_FIND_AND_REPLACE : TEXTSW_MENU_LOAD;
        textsw_create_popup_frame(VIEW_PRIVATE(textsw_first(t)), type);
        Attr_attribute key = i % 2 ? SEARCH_POPUP_KEY : LOAD_FILE_POPUP_KEY;
        Frame popup = xv_get(f, XV_KEY_DATA, key);
        expect(popup != XV_NULL, "popup created");
        /* Simulate the checking pass followed by a veto elsewhere in the
         * owner hierarchy. No cleanup is delivered to this popup yet. */
        expect(notify_post_destroy(popup, DESTROY_CHECKING, NOTIFY_IMMEDIATE) == NOTIFY_OK,
               "popup permits checking pass");
        expect(xv_get(f, XV_KEY_DATA, key) == popup,
               "cancelled close retains popup ownership");
        if (i % 3) {
            expect(xv_destroy(t) == XV_OK, "destroy text before frame");
            expect(xv_get(f, XV_KEY_DATA, key) == XV_NULL, "text destruction closes its popup");
        }
        expect(xv_destroy(f) == XV_OK, "destroy frame without stale text access");
    }
    puts("Textsw cancelled-close and popup cleanup: PASS");
    return 0;
}
