/* Run with:
 * cc -std=gnu99 -Iinclude tests/environ-test.c -o /tmp/environ-test
 * /tmp/environ-test
 */
#include <assert.h>
#include <stdlib.h>
#define NOSVENV
#include "../src/olwm/environ.c"

void *MemAlloc(unsigned int size) { return malloc(size); }
void MemFree(void *ptr) { free(ptr); }

int main(void)
{
    const char *names[] = { ":2", ":2.1", "localhost:10", "host:0.3" };
    const char *expected[] = { "DISPLAY=:2.0", "DISPLAY=:2.0",
                               "DISPLAY=localhost:10.0", "DISPLAY=host:0.0" };
    char *original[] = { "DISPLAY_EXTRA=keep", "DISPLAY=:99", NULL };
    char **saved = environ;
    _XPrivDisplay display = calloc(1, sizeof(*display));
    unsigned int i;

    environ = original;
    for (i = 0; i < sizeof(names) / sizeof(names[0]); i++) {
        char **result;
        int j;
        display->display_name = (char *)names[i];
        result = MakeEnviron((Display *)display, 0);
        assert(!strcmp(result[0], "DISPLAY_EXTRA=keep"));
        assert(!strcmp(result[1], expected[i]));
        assert(result[2] == NULL);
        for (j = 0; result[j]; j++) free(result[j]);
        free(result);
    }
    environ = saved;
    free(display);
    puts("Menu child DISPLAY tests passed");
    return 0;
}
