/* Custom calculator surface inside a native XView frame. The application owns
 * the drawing resources; the LCD remains a native selectable text control. */
#ifndef OPENLOOK_CALCULATOR_UI_H
#define OPENLOOK_CALCULATOR_UI_H

#include "calculator.h"
#include <X11/Xlib.h>
#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/canvas.h>
#include <xview/panel.h>
#include <xview/font.h>

enum {
    CALC_WIDTH = 222, CALC_HEIGHT = 380, CALC_KEY_COUNT = 55,
    CALC_GRID_X = 8, CALC_GRID_Y = 68,
    CALC_KEY_WIDTH = 38, CALC_KEY_HEIGHT = 24,
    CALC_STEP_X = 42, CALC_STEP_Y = 28
};

enum {
    SURFACE, INK, EDGE, LIGHT, SHADOW, BLUE, WHITE, SAND, PRESSED,
    LCD, LCD_INK, COLOUR_COUNT
};

typedef struct {
    int key;
    const char *label;
} CalculatorButton;

typedef struct {
    Frame frame;
    Canvas canvas;
    Xv_window paint;
    Panel lcd_panel;
    Panel_item display, angle, base, flags;
    Display *connection;
    GC gc;
    Pixmap buffer;
    XFontStruct *font, *small_font;
    unsigned long colours[COLOUR_COUNT];
    int pressed, tracking, focused, show_focus;
} CalculatorUi;

/* Immutable row-major layout, matching the scientific/programmer reference. */
extern const CalculatorButton calculator_buttons[CALC_KEY_COUNT];

/* Allocate screen-specific colors, fonts, GC and back buffer owned by ui.
 * Returns zero on failure; drawing_close releases even partial allocation. */
int drawing_open(CalculatorUi *ui);

/* Release this UI's drawing resources while its display remains connected. */
void drawing_close(CalculatorUi *ui);

/* Paint the recessed LCD surround and keys from the model and input state. */
void drawing_paint(CalculatorUi *ui);

/* Return a button index for canvas coordinates, or -1 for gaps/outside. */
int drawing_button_at(int x, int y);

/* Return the first button index for an engine key, or -1 if keyboard-only. */
int drawing_button_for(int key);

#endif
