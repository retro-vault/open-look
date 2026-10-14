/* Verify public header order, long attribute lists, and selection linkage.
 * No X server is needed: a local variadic function captures macro arguments.
 */
#include <xview/base.h>
#include <xview/attr.h>
#include <xview/notify.h>
#include <xview/sel_pkg.h>
#include <xview/macros.h>
#include <assert.h>
#include <stdarg.h>

static int token;

static Xv_opaque
capture(Xv_opaque object, ...)
{
    va_list args;
    unsigned long value;

    assert(object == 1234);
    va_start(args, object);
    assert(va_arg(args, Attr_attribute) == (Attr_attribute)&token);
    for (value = 1; value <= 80; ++value)
        assert(va_arg(args, Attr_attribute) == value);
    assert(va_arg(args, Attr_attribute) == 0);
    va_end(args);
    return object;
}

int
main(void)
{
    void (*volatile post_request)(Selection_requestor) = sel_post_request;
    Xv_opaque result;

    assert(post_request != 0);
    result = MACRO_DEF1(capture, Xv_opaque, 1234, &token,
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
        11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
        31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
        51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
        61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
        71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 0);
    assert(result == 1234);
    return 0;
}
