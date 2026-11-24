/* Desktop/pager wire constants. EWMH indices are zero-based; all-desktops is
 * the 32-bit sentinel. The private grid request carries columns and rows. */
#ifndef OLWM_DESKTOP_PROTOCOL_H
#define OLWM_DESKTOP_PROTOCOL_H

#define DESKTOP_ALL 0xffffffffUL
#define DESKTOP_MAX_AXIS 4
#define DESKTOP_MAX_COUNT 16
#define DESKTOP_GRID "_OPENLOOK_DESKTOP_GRID"
#define DESKTOP_VERSION "_OPENLOOK_DESKTOP_VERSION"
#define DESKTOP_ACTIVATE "_OPENLOOK_DESKTOP_ACTIVATE"

#endif
