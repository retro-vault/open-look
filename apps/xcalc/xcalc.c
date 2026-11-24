/* Custom-drawn calculator keys in a native XView frame. Mouse activation is
 * committed on release inside the original key; the LCD keeps native text
 * selection, and keyboard navigation is independent of pointer input. */
#include "ui.h"
#include "app-icon.h"
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <xview/cms.h>
#include <xview/notify.h>
#include <xview/openmenu.h>
#include <xview/xv_xrect.h>

/* The application owns one UI and engine for its main-loop lifetime. */
static CalculatorUi ui;
static Menu menu;

static void
refresh(void)
{
    const char *value = calculator_display();
    char status[80];
    calculator_status(status, sizeof status);
    int width = XTextWidth(ui.font, value, (int)strlen(value)) + 6;
    if (width > CALC_WIDTH - 30)
        width = CALC_WIDTH - 30;
    xv_set(ui.display, XV_X, CALC_WIDTH - 28 - width,
        PANEL_VALUE_DISPLAY_WIDTH, width, PANEL_VALUE, value, NULL);
    xv_set(ui.angle, PANEL_LABEL_STRING, calculator_angle(), NULL);
    xv_set(ui.base, PANEL_LABEL_STRING, calculator_base(), NULL);
    char flags[32];
    snprintf(flags, sizeof flags, "%s%s%s%s", rpn ? "RPN " : "",
        strstr(status, "   M") ? "M " : "",
        strstr(status, "   INV") ? "INV " : "",
        strstr(status, "( )") ? "()" : "");
    xv_set(ui.flags, PANEL_LABEL_STRING, flags, NULL);
    panel_paint(ui.lcd_panel, PANEL_CLEAR);
    drawing_paint(&ui);
}

static void
press(int key)
{
    if (!calculator_press(key))
        XBell(ui.connection, 0);
    refresh();
}

static void
mode(int use_rpn)
{
    calculator_reset(use_rpn);
    ui.pressed = ui.tracking = -1;
    refresh();
}

static void
menu_action(Menu source, Menu_item item)
{
    (void)source;
    int action = (int)xv_get(item, MENU_VALUE);
    if (action < 2)
        mode(action);
    else
        press(kBKSP);
}

static Notify_value
keyboard(Xv_window window, Event *event, Notify_arg arg, Notify_event_type type)
{
    XEvent *xe = event_xevent(event);
    if (xe && xe->type == KeyRelease && ui.tracking < 0) {
        ui.pressed = -1;
        drawing_paint(&ui);
    }
    if (xe && xe->type == KeyPress) {
        KeySym symbol;
        char text[8];
        int length = XLookupString(&xe->xkey, text, sizeof text, &symbol, NULL);
        if ((xe->xkey.state & ControlMask) && (symbol == XK_q || symbol == XK_Q)) {
            xv_destroy_safe(ui.frame);
            return NOTIFY_DONE;
        }
        if (symbol == XK_F2 || ((xe->xkey.state & ControlMask) &&
            (symbol == XK_r || symbol == XK_R))) {
            mode(!rpn);
            return NOTIFY_DONE;
        }
        int step = 0;
        if (symbol == XK_Tab || symbol == XK_ISO_Left_Tab)
            step = xe->xkey.state & ShiftMask ? -1 : 1;
        else if (symbol == XK_Left) step = -1;
        else if (symbol == XK_Right) step = 1;
        else if (symbol == XK_Up) step = -5;
        else if (symbol == XK_Down) step = 5;
        if (step) {
            ui.focused = ui.focused < 0 ? (step < 0 ? CALC_KEY_COUNT - 1 : 0) :
                (ui.focused + step + CALC_KEY_COUNT) % CALC_KEY_COUNT;
            ui.show_focus = 1;
            drawing_paint(&ui);
            return NOTIFY_DONE;
        }
        int key = kNOP;
        if (!(xe->xkey.state & (ControlMask | Mod1Mask))) {
            if (symbol == XK_space && ui.show_focus && ui.focused >= 0)
                key = calculator_buttons[ui.focused].key;
            else if (symbol == XK_BackSpace || symbol == XK_Delete) key = kBKSP;
            else if (symbol == XK_Escape) key = kOFF;
            else if (symbol == XK_Return || symbol == XK_KP_Enter) key = kEQU;
            else if (length == 1) key = calculator_key((unsigned char)text[0]);
        }
        if (key != kNOP) {
            ui.pressed = drawing_button_for(key);
            press(key);
            return NOTIFY_DONE;
        }
    }
    return notify_next_event_func(window, (Notify_event)event, arg, type);
}

static void
mouse(Xv_window window, Event *event)
{
    int target = drawing_button_at(event_x(event), event_y(event));
    if (event_action(event) == ACTION_MENU && event_is_down(event)) {
        menu_show(menu, window, event, NULL);
    } else if (event_action(event) == ACTION_SELECT && event_is_down(event)) {
        ui.tracking = ui.pressed = target;
        ui.focused = target;
        ui.show_focus = 0;
        xv_set(window, WIN_SET_FOCUS, NULL);
        drawing_paint(&ui);
    } else if (event_action(event) == LOC_DRAG && ui.tracking >= 0) {
        ui.pressed = target == ui.tracking ? target : -1;
        drawing_paint(&ui);
    } else if (event_action(event) == ACTION_SELECT && event_is_up(event)) {
        int activate = target >= 0 && target == ui.tracking;
        ui.tracking = ui.pressed = -1;
        if (activate)
            press(calculator_buttons[target].key);
        else
            drawing_paint(&ui);
    }
}

static void
repaint(Canvas canvas, Xv_window paint, Display *display, Window window,
        Xv_xrectlist *rectangles)
{
    (void)canvas; (void)paint; (void)display; (void)window; (void)rectangles;
    drawing_paint(&ui);
}

int
main(int argc, char **argv)
{
    int use_rpn = 0;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-rpn")) {
            use_rpn = 1;
            memmove(&argv[i], &argv[i + 1], (size_t)(argc - i) * sizeof *argv);
            --argc;
            --i;
        } else if (!strcmp(argv[i], "-help") || !strcmp(argv[i], "--help")) {
            puts("Usage: xcalc [-rpn] [XView options]\n"
                 "Enter evaluates; Escape clears; Backspace edits.\n"
                 "F2 or Ctrl+R switches algebraic/RPN modes; Ctrl+Q closes.\n"
                 "Tab/arrows select a key, Space activates it. See man xcalc.");
            return 0;
        }
    }
    xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, NULL);
    calculator_reset(use_rpn);
    ui.pressed = ui.tracking = ui.focused = -1;
    ui.frame = xv_create(NULL, FRAME, FRAME_LABEL, "Calculator",
        FRAME_NO_CONFIRM, TRUE, FRAME_SHOW_RESIZE_CORNER, FALSE,
        FRAME_MIN_SIZE, CALC_WIDTH, CALC_HEIGHT,
        FRAME_MAX_SIZE, CALC_WIDTH, CALC_HEIGHT,
        XV_WIDTH, CALC_WIDTH, XV_HEIGHT, CALC_HEIGHT, NULL);
    Xv_Font font = xv_find(ui.frame, FONT, FONT_NAME,
        "-adobe-helvetica-medium-r-normal--12-*-*-*-*-*-iso8859-1", NULL);
    if (font)
        xv_set(ui.frame, WIN_FONT, font, NULL);
    xv_set(ui.frame, WIN_CONSUME_EVENTS, WIN_ASCII_EVENTS, WIN_UP_ASCII_EVENTS,
        NULL, NULL);
    notify_interpose_event_func(ui.frame, keyboard, NOTIFY_SAFE);
    app_icon_apply(xv_get(ui.frame, FRAME_ICON), "calculator");
    xv_set(xv_get(ui.frame, FRAME_ICON), ICON_LABEL, "Calculator", NULL);
    ui.canvas = xv_create(ui.frame, CANVAS, XV_X, 0, XV_Y, 0,
        XV_WIDTH, CALC_WIDTH, XV_HEIGHT, CALC_HEIGHT,
        OPENWIN_SHOW_BORDERS, FALSE, CANVAS_X_PAINT_WINDOW, TRUE,
        CANVAS_RETAINED, FALSE, CANVAS_REPAINT_PROC, repaint, NULL);
    ui.paint = canvas_paint_window(ui.canvas);
    xv_set(ui.paint, WIN_EVENT_PROC, mouse, WIN_CONSUME_EVENTS,
        WIN_MOUSE_BUTTONS, LOC_DRAG, WIN_ASCII_EVENTS, WIN_UP_ASCII_EVENTS,
        NULL, NULL);
    notify_interpose_event_func(ui.paint, keyboard, NOTIFY_SAFE);
    if (!drawing_open(&ui)) {
        fprintf(stderr, "xcalc: cannot initialize calculator drawing resources\n");
        drawing_close(&ui);
        return 1;
    }
    Cms lcd_colours = xv_create(NULL, CMS, CMS_SIZE, 2,
        CMS_NAMED_COLORS, "#a5b9a3", "#203623", NULL, NULL);
    ui.lcd_panel = xv_create(ui.frame, PANEL, XV_X, 12, XV_Y, 12,
        XV_WIDTH, CALC_WIDTH - 24, XV_HEIGHT, 40, WIN_CMS, lcd_colours,
        PANEL_ACCEPT_KEYSTROKE, TRUE, NULL);
    notify_interpose_event_func(ui.lcd_panel, keyboard, NOTIFY_SAFE);
    ui.display = xv_create(ui.lcd_panel, PANEL_TEXT, XV_X, 4, XV_Y, 0,
        PANEL_VALUE, "0", PANEL_VALUE_STORED_LENGTH, 32,
        PANEL_VALUE_DISPLAY_WIDTH, CALC_WIDTH - 32,
        PANEL_READ_ONLY, TRUE, PANEL_VALUE_UNDERLINED, FALSE, NULL);
    ui.angle = xv_create(ui.lcd_panel, PANEL_MESSAGE, XV_X, 24, XV_Y, 23,
        PANEL_LABEL_STRING, "DEG", PANEL_LABEL_BOLD, FALSE, NULL);
    ui.base = xv_create(ui.lcd_panel, PANEL_MESSAGE, XV_X, CALC_WIDTH - 72,
        XV_Y, 23, PANEL_LABEL_STRING, "DEC", PANEL_LABEL_BOLD, FALSE, NULL);
    ui.flags = xv_create(ui.lcd_panel, PANEL_MESSAGE, XV_X, 0, XV_Y, 0,
        PANEL_LABEL_STRING, "", PANEL_LABEL_BOLD, FALSE, NULL);
    menu = xv_create(NULL, MENU, MENU_TITLE_ITEM, "Calculator",
        MENU_STRINGS, "Algebraic", "RPN", "Backspace", NULL,
        MENU_NOTIFY_PROC, menu_action, NULL);
    /* MENU_STRINGS assigns values starting at one. Use explicit mode values. */
    for (int i = 0; i < 3; ++i)
        xv_set(xv_get(menu, MENU_NTH_ITEM, i + 2), MENU_VALUE, i, NULL);
    refresh();
    xv_main_loop(ui.frame);
    drawing_close(&ui);
    return 0;
}
