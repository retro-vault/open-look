/* Rounded, beveled calculator keys drawn into an X11 back buffer. Drawing is
 * independent of input and arithmetic; it never sends synthetic events. */
#include "ui.h"
#include <xview/cms.h>

const CalculatorButton calculator_buttons[CALC_KEY_COUNT] = {
    {kRECIP,"1/x"}, {kSQR,"x2"}, {kSQRT,"sqrt"}, {kCLR,"CE/C"}, {kOFF,"AC"},
    {kINV,"INV"}, {kSIN,"sin"}, {kCOS,"cos"}, {kTAN,"tan"}, {kDRG,"DRG"},
    {kE,"e"}, {kEE,"EE"}, {kLOG,"log"}, {kLN,"ln"}, {kPOW,"yx"},
    {kNOT,"not"}, {kAND,"and"}, {kOR,"or"}, {kXOR,"xor"}, {kTRUNC,"trunc"},
    {kPI,"pi"}, {kFACT,"x!"}, {kLPAR,"("}, {kRPAR,")"}, {kBASE,"base"},
    {kSHL,"shl"}, {kxD,"D"}, {kxE,"E"}, {kxF,"F"}, {kSHR,"shr"},
    {kMOD,"mod"}, {kxA,"A"}, {kxB,"B"}, {kxC,"C"}, {kDIV,"/"},
    {kSTO,"STO"}, {kSEVEN,"7"}, {kEIGHT,"8"}, {kNINE,"9"}, {kMUL,"*"},
    {kRCL,"RCL"}, {kFOUR,"4"}, {kFIVE,"5"}, {kSIX,"6"}, {kSUB,"-"},
    {kSUM,"SUM"}, {kONE,"1"}, {kTWO,"2"}, {kTHREE,"3"}, {kADD,"+"},
    {kEXC,"EXC"}, {kZERO,"0"}, {kDEC,"."}, {kNEG,"+/-"}, {kEQU,"="}
};

static void
colour(CalculatorUi *ui, int index)
{
    XSetForeground(ui->connection, ui->gc, ui->colours[index]);
}

static void
line(CalculatorUi *ui, int x1, int y1, int x2, int y2)
{
    XDrawLine(ui->connection, ui->buffer, ui->gc, x1, y1, x2, y2);
}

/* Fill a rounded rectangle with integer-pixel arcs, retaining the crisp
 * low-resolution character of the original calculator. */
static void
rounded(CalculatorUi *ui, int x, int y, int w, int h, int radius)
{
    Display *dpy = ui->connection;
    XFillRectangle(dpy, ui->buffer, ui->gc, x + radius, y, w - 2 * radius, h);
    XFillRectangle(dpy, ui->buffer, ui->gc, x, y + radius, w, h - 2 * radius);
    int diameter = 2 * radius;
    XFillArc(dpy, ui->buffer, ui->gc, x, y, diameter, diameter, 0, 360 * 64);
    XFillArc(dpy, ui->buffer, ui->gc, x + w - diameter, y,
             diameter, diameter, 0, 360 * 64);
    XFillArc(dpy, ui->buffer, ui->gc, x, y + h - diameter,
             diameter, diameter, 0, 360 * 64);
    XFillArc(dpy, ui->buffer, ui->gc, x + w - diameter, y + h - diameter,
             diameter, diameter, 0, 360 * 64);
}

static int
key_colour(int key)
{
    switch (key) {
    case kZERO: case kONE: case kTWO: case kTHREE: case kFOUR:
    case kFIVE: case kSIX: case kSEVEN: case kEIGHT: case kNINE: case kDEC:
        return WHITE;
    case kDIV: case kMUL: case kBASE: case kTRUNC: case kSHR:
        return SAND;
    default: return BLUE;
    }
}

static void
label(CalculatorUi *ui, int index, int x, int y)
{
    int key = calculator_buttons[index].key;
    const char *text = calculator_buttons[index].label;
    int cx = x + CALC_KEY_WIDTH / 2;
    int cy = y + CALC_KEY_HEIGHT / 2;
    colour(ui, INK);
    if (key == kSQRT) {
        line(ui, cx - 7, cy, cx - 4, cy + 5);
        line(ui, cx - 4, cy + 5, cx - 1, cy - 6);
        line(ui, cx - 1, cy - 6, cx + 7, cy - 6);
        return;
    }
    if (key == kPI) {
        line(ui, cx - 7, cy - 4, cx + 7, cy - 4);
        line(ui, cx - 4, cy - 3, cx - 5, cy + 5);
        line(ui, cx + 3, cy - 3, cx + 3, cy + 5);
        line(ui, cx + 3, cy + 5, cx + 6, cy + 4);
        return;
    }
    if (key == kDIV || key == kNEG) {
        line(ui, cx - 5, cy, cx + 5, cy);
        if (key == kDIV) {
            XFillRectangle(ui->connection, ui->buffer, ui->gc, cx - 1, cy - 5, 2, 2);
            XFillRectangle(ui->connection, ui->buffer, ui->gc, cx - 1, cy + 4, 2, 2);
        } else {
            line(ui, cx, cy - 4, cx, cy + 4);
            line(ui, cx - 5, cy + 6, cx + 5, cy + 6);
        }
        return;
    }
    if (key == kSQR || key == kPOW) {
        const char *base = key == kSQR ? "x" : "y";
        const char *exponent = key == kSQR ? "2" : "x";
        XDrawString(ui->connection, ui->buffer, ui->gc, cx - 6, cy + 4, base, 1);
        XSetFont(ui->connection, ui->gc, ui->small_font->fid);
        XDrawString(ui->connection, ui->buffer, ui->gc, cx + 1, cy - 1, exponent, 1);
        XSetFont(ui->connection, ui->gc, ui->font->fid);
        return;
    }
    if (rpn) {
        if (key == kLPAR) text = "Roll";
        if (key == kRPAR) text = "X:Y";
        if (key == kEQU) text = "Ent";
    }
    int width = XTextWidth(ui->font, text, (int)strlen(text));
    int baseline = cy + (ui->font->ascent - ui->font->descent) / 2;
    XDrawString(ui->connection, ui->buffer, ui->gc,
                 cx - width / 2, baseline, text, (int)strlen(text));
}

int
drawing_open(CalculatorUi *ui)
{
    ui->connection = (Display *)xv_get(ui->frame, XV_DISPLAY);
    Window window = (Window)xv_get(ui->paint, XV_XID);
    ui->gc = XCreateGC(ui->connection, window, 0, NULL);
    ui->buffer = XCreatePixmap(ui->connection, window, CALC_WIDTH, CALC_HEIGHT,
                                (unsigned int)xv_get(ui->paint, WIN_DEPTH));
    Xv_Font font = xv_get(ui->paint, WIN_FONT);
    ui->font = XQueryFont(ui->connection, (Font)xv_get(font, XV_XID));
    ui->small_font = XLoadQueryFont(ui->connection,
        "-adobe-helvetica-medium-r-normal--10-*-*-*-*-*-iso8859-1");
    if (!ui->small_font)
        ui->small_font = XLoadQueryFont(ui->connection, "fixed");
    if (!ui->gc || !ui->buffer || !ui->font || !ui->small_font)
        return 0;
    Cms cms = xv_get(ui->paint, WIN_CMS);
    Colormap map = (Colormap)xv_get(cms, CMS_CMAP_ID);
    static const char *names[] = {
        "#cccccc", "#26343a", "#8298a4", "#f3f7fa", "#a6afb3",
        "#d3e4ef", "#f2f2ef", "#eddbc5", "#b2c7d4",
        "#a5b9a3", "#203623"
    };
    for (int i = 0; i < COLOUR_COUNT; ++i) {
        XColor color, exact;
        ui->colours[i] = (unsigned long)xv_get(cms, CMS_BACKGROUND_PIXEL);
        if (XAllocNamedColor(ui->connection, map, names[i], &color, &exact))
            ui->colours[i] = color.pixel;
    }
    return 1;
}

void
drawing_close(CalculatorUi *ui)
{
    if (ui->buffer) XFreePixmap(ui->connection, ui->buffer);
    if (ui->gc) XFreeGC(ui->connection, ui->gc);
    if (ui->font) XFreeFontInfo(NULL, ui->font, 1);
    if (ui->small_font) XFreeFont(ui->connection, ui->small_font);
}

void
drawing_paint(CalculatorUi *ui)
{
    if (!ui->gc || !ui->buffer)
        return;
    XSetFont(ui->connection, ui->gc, ui->font->fid);
    colour(ui, SURFACE);
    XFillRectangle(ui->connection, ui->buffer, ui->gc,
                    0, 0, CALC_WIDTH, CALC_HEIGHT);
    /* Recessed screen: dark top/left and light bottom/right edges. */
    colour(ui, SHADOW);
    XDrawRectangle(ui->connection, ui->buffer, ui->gc, 8, 8, CALC_WIDTH - 17, 48);
    colour(ui, LIGHT);
    line(ui, 9, 57, CALC_WIDTH - 8, 57);
    line(ui, CALC_WIDTH - 8, 9, CALC_WIDTH - 8, 57);
    colour(ui, LCD);
    XFillRectangle(ui->connection, ui->buffer, ui->gc, 11, 11, CALC_WIDTH - 22, 43);
    for (int i = 0; i < CALC_KEY_COUNT; ++i) {
        int x = CALC_GRID_X + i % 5 * CALC_STEP_X;
        int y = CALC_GRID_Y + i / 5 * CALC_STEP_Y;
        int down = i == ui->pressed;
        colour(ui, SHADOW);
        rounded(ui, x, y + 1, CALC_KEY_WIDTH, CALC_KEY_HEIGHT, 5);
        colour(ui, EDGE);
        rounded(ui, x, y, CALC_KEY_WIDTH, CALC_KEY_HEIGHT - 1, 5);
        colour(ui, down ? PRESSED : key_colour(calculator_buttons[i].key));
        rounded(ui, x + 1, y + 1, CALC_KEY_WIDTH - 2, CALC_KEY_HEIGHT - 3, 4);
        colour(ui, down ? SHADOW : LIGHT);
        line(ui, x + 5, y + 1, x + CALC_KEY_WIDTH - 6, y + 1);
        line(ui, x + 1, y + 5, x + 1, y + CALC_KEY_HEIGHT - 6);
        colour(ui, down ? LIGHT : SHADOW);
        line(ui, x + 5, y + CALC_KEY_HEIGHT - 3,
             x + CALC_KEY_WIDTH - 6, y + CALC_KEY_HEIGHT - 3);
        line(ui, x + CALC_KEY_WIDTH - 2, y + 5,
             x + CALC_KEY_WIDTH - 2, y + CALC_KEY_HEIGHT - 6);
        label(ui, i, x + down, y + down);
        if (ui->show_focus && ui->focused == i) {
            colour(ui, INK);
            const char dots[] = {1, 2};
            XSetDashes(ui->connection, ui->gc, 0, dots, 2);
            XSetLineAttributes(ui->connection, ui->gc, 0, LineOnOffDash,
                               CapButt, JoinMiter);
            XDrawRectangle(ui->connection, ui->buffer, ui->gc,
                x + 3, y + 3, CALC_KEY_WIDTH - 7, CALC_KEY_HEIGHT - 8);
            XSetLineAttributes(ui->connection, ui->gc, 0, LineSolid,
                               CapButt, JoinMiter);
        }
    }
    XCopyArea(ui->connection, ui->buffer, (Window)xv_get(ui->paint, XV_XID),
               ui->gc, 0, 0, CALC_WIDTH, CALC_HEIGHT, 0, 0);
    XFlush(ui->connection);
}

int
drawing_button_at(int x, int y)
{
    x -= CALC_GRID_X;
    y -= CALC_GRID_Y;
    if (x < 0 || y < 0 || x / CALC_STEP_X >= 5 || y / CALC_STEP_Y >= 11 ||
        x % CALC_STEP_X >= CALC_KEY_WIDTH || y % CALC_STEP_Y >= CALC_KEY_HEIGHT)
        return -1;
    return y / CALC_STEP_Y * 5 + x / CALC_STEP_X;
}

int
drawing_button_for(int key)
{
    for (int i = 0; i < CALC_KEY_COUNT; ++i)
        if (calculator_buttons[i].key == key)
            return i;
    return -1;
}
