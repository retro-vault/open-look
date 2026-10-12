/* Keep regression checks active in Release builds as well. */
#undef NDEBUG
#include <assert.h>
#include "../apps/props/color_props.c"
void props_entry(int argc, char **argv);
void props_test_loop(Xv_Window window) { (void)window; }
static int errors;
static void check_image(Server_image image, const char *indices, int width, int height)
{
    unsigned long *pixels = (unsigned long *)xv_get(cms, CMS_INDEX_TABLE);
    XImage *rendered = XGetImage(dsp,
        (Drawable)xv_get(image, SERVER_IMAGE_PIXMAP), 0, 0,
        width, height, AllPlanes, ZPixmap);
    assert(rendered);
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            assert(XGetPixel(rendered, x, y) ==
                   pixels[(unsigned char)indices[y * width + x]]);
    XDestroyImage(rendered);
}
static int xerror(Display *d, XErrorEvent *e) {
    char text[200]; XGetErrorText(d, e->error_code, text, sizeof(text));
    fprintf(stderr, "X error: %s request %d\n", text, e->request_code);
    errors++; return 0;
}
int main(int argc, char **argv) {
    unsigned long oldpixel, newpixel;
    XColor actual;
    XSetErrorHandler(xerror);
    props_entry(argc, argv);
    XSync(dsp, False);
    assert(!errors);
    if ((int)xv_get(frame, XV_VISUAL_CLASS) == StaticGray ||
        (int)xv_get(frame, XV_VISUAL_CLASS) == GrayScale) {
        assert(!color);
        puts("Verified grayscale omits the colour palette");
        xv_destroy(frame);
        return 0;
    }
    assert(color);
    assert((int)xv_get(flavor_choice, PANEL_NCHOICES) == TOTAL_PANELS + 1);
    XSync(dsp, False);
    assert(!errors);
    assert((int)xv_get(preview_image, SERVER_IMAGE_DEPTH) == (int)xv_get(frame, XV_DEPTH));
    oldpixel = (unsigned long)xv_get(cms, CMS_PIXEL, CMS_CONTROL_COLORS + COLOR_CHOICES + WorkspaceIndex);
    xact_work.h=0; xact_work.s=MAXSV; xact_work.v=MAXSV;
    update_colors();
    newpixel = (unsigned long)xv_get(cms, CMS_PIXEL, CMS_CONTROL_COLORS + COLOR_CHOICES + WorkspaceIndex);
    actual.pixel=newpixel;
    XQueryColor(dsp, (Colormap)xv_get(cms, CMS_CMAP_ID), &actual);
    assert(actual.red > 64000 && actual.green < 512 && actual.blue < 512);
    if ((int)xv_get(frame, XV_VISUAL_CLASS) == TrueColor) {
        assert(oldpixel != newpixel);
        check_image(preview_image, image_data, IMAGE_WIDTH, IMAGE_HEIGHT);
        check_image(hue_image, hue_data, SIMAGE_WIDTH, SIMAGE_HEIGHT);
        check_image(sat_image, sat_data, SIMAGE_WIDTH, SIMAGE_HEIGHT);
        check_image(val_image, val_data, SIMAGE_WIDTH, SIMAGE_HEIGHT);
    }
    reset_colors();
    assert(xact_work.h == backup_xact_work.h &&
           xact_work.s == backup_xact_work.s &&
           xact_work.v == backup_xact_work.v);
    apply_colors();
    assert(!strcmp(defaults_get_string(WORKSPACE_RES, WORKSPACE_CLASS_RES, ""),
                   hsv_to_string(&xact_work)));
    for(int i=0;i<100;i++) {
        xact_work.h=i*3; xact_work.s=MAXSV; xact_work.v=MAXSV;
        update_colors();
        store_custom_colors();
    }
    XSync(dsp,False); assert(!errors);
    printf("Verified palette, previews, updates, Apply/Reset at depth %d, visual %d\n", (int)xv_get(frame,XV_DEPTH),(int)xv_get(frame,XV_VISUAL_CLASS));
    xv_destroy(frame);
    XSync(dsp,False); assert(!errors);
    return 0;
}
