/* Native interface to the imported xcalc engine. A calculator process owns
 * one engine instance; returned strings remain owned by that instance. */
#ifndef OPENLOOK_CALCULATOR_H
#define OPENLOOK_CALCULATOR_H

#include "xcalc.h"

/* Reset all registers and select algebraic (zero) or RPN (nonzero) operation. */
void calculator_reset(int use_rpn);

/* Apply a scientific or programmer calculator key. Returns zero if rejected; errors appear
 * in the display and can be cleared with CE/C or AC. No X11 calls are made. */
int calculator_press(int key);

/* Return borrowed static labels for the current angle units and number base. */
const char *calculator_angle(void);
const char *calculator_base(void);

/* Map an ASCII keyboard character to a key ID, or kNOP if unrecognized. */
int calculator_key(int character);

/* Return the current display text, borrowed until the next operation. */
const char *calculator_display(void);

/* Format mode, angle, memory and inverse indicators into a caller-owned
 * buffer. A zero capacity is allowed; otherwise the output is terminated. */
void calculator_status(char *text, size_t capacity);

#endif
