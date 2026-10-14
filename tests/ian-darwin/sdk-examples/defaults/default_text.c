/*
 * default_text.c -- use the defaults package to get a font name from
 * the resource database to set the textsw's font.
 */
#include <xview/xview.h>
#include <xview/font.h>
#include <xview/defaults.h>
#include <xview/textsw.h>

int main(int argc, char * argv[])
{
    Frame       frame;
    Xv_Font     font;
    char        *name;

    xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);

    frame = (Frame)xv_create(0, FRAME, 0);

    name = defaults_get_string("textsw.font","Textsw.Font", "fixed"),
    font = xv_find(frame, FONT,
        FONT_NAME,      name,
        0);

    xv_create(frame, TEXTSW,
        XV_FONT,        font,
        WIN_COLUMNS,    80,
        WIN_ROWS,       10,
        0);

    window_fit(frame);
    xv_main_loop(frame);
}
