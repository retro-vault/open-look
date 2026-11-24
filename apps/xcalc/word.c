/* Checked programmer arithmetic. Unsigned 32-bit words remain exact in the
 * scientific engine's doubles; invalid conversions never reach C casts. */
#include "xcalc.h"
#include <stdint.h>

int
calculator_word(double value, uint32_t *word)
{
    if (!isfinite(value) || value < 0 || value > UINT32_MAX ||
        floor(value) != value) {
        errno = EDOM;
        return 0;
    }
    *word = (uint32_t)value;
    return 1;
}

double
calculator_binary(int key, double left, double right)
{
    switch (key) {
    case kADD: return left + right;
    case kSUB: return left - right;
    case kMUL: return left * right;
    case kDIV: return left / right;
    case kPOW: return pow(left, right);
    default: break;
    }
    uint32_t a, b;
    if (!calculator_word(left, &a) || !calculator_word(right, &b))
        return NAN;
    switch (key) {
    case kMOD: if (b) return a % b; break;
    case kAND: return a & b;
    case kOR: return a | b;
    case kXOR: return a ^ b;
    case kSHL: if (b < 32) return (uint32_t)(a << b); break;
    case kSHR: if (b < 32) return a >> b; break;
    default: break;
    }
    errno = EDOM;
    return NAN;
}
