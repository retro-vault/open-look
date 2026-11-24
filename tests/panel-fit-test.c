/* Exercise compact menu fitting without changing ordinary panel geometry. */
#include <stdio.h>
#include <stdlib.h>
#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/panel.h>
#include <xview/openmenu.h>
#include <xview/scrollbar.h>
#include <xview/font.h>

static void
expect(int condition, const char *message)
{
    if (!condition) {
        fprintf(stderr, "%s\n", message);
        exit(EXIT_FAILURE);
    }
}

/* Record the native item extents before invoking any fit method. */
static int
extent(Panel panel, int width)
{
    int result = 0;
    Panel_item item;
    for (item = xv_get(panel, PANEL_FIRST_ITEM); item;
         item = xv_get(item, PANEL_NEXT_ITEM)) {
        const Rect *rect = (const Rect *)xv_get(item, PANEL_ITEM_RECT);
        int end = width ? rect->r_left + rect->r_width
                        : rect->r_top + rect->r_height;
        if (end > result)
            result = end;
    }
    return result;
}

static Panel
make_panel(Frame frame)
{
    return xv_create(frame, PANEL, XV_WIDTH, 800, XV_HEIGHT, 180,
                     PANEL_LAYOUT, PANEL_HORIZONTAL, NULL);
}

static Panel_item
add_button(Panel panel, Menu menu)
{
    Panel_item button = xv_create(panel, PANEL_BUTTON,
                                 PANEL_LABEL_STRING, "File", NULL);
    if (menu)
        xv_set(button, PANEL_ITEM_MENU, menu, NULL);
    return button;
}

int
main(int argc, char **argv)
{
    Frame frame;
    Panel panel, content, ordinary, mixed, vertical, empty;
    Panel_item second;
    Menu menu;
    Scrollbar scrollbar;
    int bottom, margin, extra, width, fitted;

    xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, NULL);
    frame = xv_create(NULL, FRAME, XV_SHOW, FALSE,
                      XV_WIDTH, 900, XV_HEIGHT, 600, NULL);
    expect(frame != XV_NULL, "create frame");
    menu = xv_create(NULL, MENU_COMMAND_MENU,
                     MENU_STRINGS, "Open", "Save", NULL, NULL);

    ordinary = make_panel(frame);
    add_button(ordinary, XV_NULL);
    bottom = extent(ordinary, 0);
    extra = (int)xv_get(ordinary, PANEL_EXTRA_PAINT_HEIGHT);
    margin = (int)xv_get(ordinary, WIN_FIT_HEIGHT, 0) - bottom - extra;
    expect(margin >= 8, "ordinary command panel retains its font margin");
    window_fit_height(ordinary);
    expect((int)xv_get(ordinary, XV_HEIGHT) == bottom + margin + extra,
           "ordinary command panel fit is unchanged");

    panel = make_panel(frame);
    add_button(panel, menu);
    second = add_button(panel, menu);
    bottom = extent(panel, 0);
    width = extent(panel, 1);
    window_fit_height(panel);
    fitted = (int)xv_get(panel, XV_HEIGHT);
    printf("menu: bottom=%d height=%d bottom-inset=%d; form-margin=%d\n",
           bottom, fitted, fitted - bottom, margin);
    expect(fitted == bottom + 2, "menu panel has two-pixel bottom inset");
    expect((int)xv_get(panel, XV_WIDTH) == 800,
           "height fitting leaves width unchanged");
    content = xv_create(frame, PANEL, WIN_BELOW, panel, XV_HEIGHT, 20, NULL);
    expect((int)xv_get(content, XV_Y) ==
           (int)xv_get(panel, XV_Y) + fitted,
           "content starts immediately below fitted menu");
    window_fit(panel);
    expect((int)xv_get(panel, XV_HEIGHT) == bottom + 2,
           "window_fit also uses compact menu height");
    expect((int)xv_get(panel, XV_WIDTH) == width + margin + 1,
           "menu width keeps the ordinary font margin");

    xv_set(panel, WIN_FIT_HEIGHT, 7, NULL);
    expect((int)xv_get(panel, XV_HEIGHT) == bottom + 7 + 1 + 7,
           "explicit nonzero fit margin preserves existing semantics");
    xv_set(panel, PANEL_EXTRA_PAINT_HEIGHT, 9, NULL);
    window_fit_height(panel);
    expect((int)xv_get(panel, XV_HEIGHT) == bottom + 1 + 9,
           "extra paint height remains additive");
    xv_set(panel, PANEL_EXTRA_PAINT_HEIGHT, 1, NULL);

    /* A second row must remain inside the fitted panel. */
    xv_set(second, PANEL_ITEM_X, 4, PANEL_ITEM_Y, bottom + 13, NULL);
    bottom = extent(panel, 0);
    window_fit_height(panel);
    expect((int)xv_get(panel, XV_HEIGHT) == bottom + 2,
           "menu fit includes lower rows");
    panel = xv_create(frame, SCROLLABLE_PANEL,
                      XV_WIDTH, 800, XV_HEIGHT, 180, NULL);
    add_button(panel, menu);
    bottom = extent(panel, 0);
    scrollbar = xv_create(panel, SCROLLBAR, SCROLLBAR_DIRECTION,
                          SCROLLBAR_HORIZONTAL, NULL);
    xv_set(panel, WIN_HORIZONTAL_SCROLLBAR, scrollbar, NULL);
    window_fit_height(panel);
    expect((int)xv_get(panel, XV_HEIGHT) == bottom + 2 +
           (int)xv_get(scrollbar, XV_HEIGHT), "horizontal scrollbar retained");

    mixed = make_panel(frame);
    add_button(mixed, menu);
    xv_create(mixed, PANEL_TEXT, PANEL_LABEL_STRING, "Name", NULL);
    window_fit_height(mixed);
    expect((int)xv_get(mixed, XV_HEIGHT) == extent(mixed, 0) + margin + 1,
           "mixed form retains its font margin");
    vertical = make_panel(frame);
    xv_set(vertical, PANEL_LAYOUT, PANEL_VERTICAL, NULL);
    add_button(vertical, menu);
    window_fit_height(vertical);
    expect((int)xv_get(vertical, XV_HEIGHT) ==
           extent(vertical, 0) + margin + 1,
           "vertical menu panel retains its font margin");
    empty = make_panel(frame);
    window_fit_height(empty);
    expect((int)xv_get(empty, XV_HEIGHT) == margin + 1,
           "empty panel retains its font margin");

    xv_destroy(frame);
    xv_destroy(menu);
    puts("Panel fitting checks passed.");
    return EXIT_SUCCESS;
}
