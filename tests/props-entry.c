/* Exercise the real application startup without entering its event loop. */
#define main props_entry
#define xv_main_loop props_test_loop
#include "../apps/props/props.c"
