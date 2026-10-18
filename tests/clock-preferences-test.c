/* Regression checks for preferences with long HOME paths and container UIDs. */
#define main clock_application_main
#include "../apps/clock/clock.c"
#undef main
#include <assert.h>
#include <sys/stat.h>

int main(void)
{
    char temporary[] = "/tmp/openlook-clock.XXXXXX";
    char home_path[512], rc_path[540], component[151];
    const char *previous = getenv("HOME");
    char *saved_home = previous ? strdup(previous) : NULL;
    ClockOptions options = {0};
    FILE *fp;

    assert(mkdtemp(temporary) != NULL);
    memset(component, 'h', sizeof(component) - 1);
    component[sizeof(component) - 1] = '\0';
    snprintf(home_path, sizeof(home_path), "%s/%s", temporary, component);
    assert(mkdir(home_path, 0700) == 0);
    snprintf(rc_path, sizeof(rc_path), "%s/.clockrc", home_path);
    assert(setenv("HOME", home_path, 1) == 0);
    readrc(&options);
    assert(options.face == analog && options.seconds == 0 && options.date == 0);
    for (int face = digital; face <= analog; ++face) {
        options.face = (Face)face;
        options.seconds = 1;
        options.date = 1;
        writerc(&options);
        memset(&options, 0, sizeof(options));
        readrc(&options);
        assert(options.face == (Face)face);
        assert(options.seconds == 1 && options.date == 1);
        assert(options.faceBAK == options.face && options.secondsBAK == 1);
    }
    fp = fopen(rc_path, "w");
    assert(fp != NULL);
    fputs("99 broken preferences\n", fp);
    fclose(fp);
    readrc(&options);
    assert(options.face == analog && options.seconds == 0 && options.date == 0);
    assert(unsetenv("HOME") == 0);
    readrc(&options); /* Missing both HOME and passwd entry must not exit. */
    if (!getpwuid(getuid())) assert(options.face == analog);
    if (saved_home) {
        assert(setenv("HOME", saved_home, 1) == 0);
        free(saved_home);
    } else {
        assert(unsetenv("HOME") == 0);
    }
    assert(unlink(rc_path) == 0);
    assert(rmdir(home_path) == 0);
    assert(rmdir(temporary) == 0);
    puts("Clock preferences: long HOME, round trips, malformed/missing config passed.");
    return 0;
}
