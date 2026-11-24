/* Native scientific-key dispatch adapted from X.Org xcalc actions.c.
 * See COPYING for the retained X Consortium license. Xt callbacks are
 * replaced by a small interface usable by both XView and engine tests. */
/*

Copyright (c) 1989  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.

*/

#include "calculator.h"

/* The upstream engine owns singleton arithmetic state for this process. */
int rpn;
char dispstr[LCD_STR_LEN];
static int indicators[9];
static int rejected;

void
draw(char *string)
{
    /* The engine already formatted dispstr; the UI repaints after dispatch. */
    (void)string;
}

void
setflag(int indicator, int on)
{
    if (indicator >= 0 && indicator < 9)
        indicators[indicator] = !!on;
}

void
ringbell(void)
{
    rejected = 1;
}

void
calculator_reset(int use_rpn)
{
    rpn = !!use_rpn;
    rejected = 0;
    offf();
}

int
calculator_press(int key)
{
    rejected = 0;
    if (key < kRECIP || key > kxF || key == kNOP || pre_op(key))
        return 0;
    switch (key) {
    case kZERO: case kONE: case kTWO: case kTHREE: case kFOUR:
    case kFIVE: case kSIX: case kSEVEN: case kEIGHT: case kNINE:
    case kxA: case kxB: case kxC: case kxD: case kxE: case kxF:
        numeric(key); break;
    case kADD: case kSUB: case kMUL: case kDIV: case kPOW:
    case kMOD: case kAND: case kOR: case kXOR: case kSHL: case kSHR:
        if (rpn) twof(key); else twoop(key);
        break;
    case kBASE: change_base(); break;
    case kDEC: decf(); break;
    case kEE: eef(); break;
    case kCLR: clearf(); break;
    case kOFF: offf(); break;
    case kNEG: negf(); break;
    case kINV: invf(); break;
    case kDRG: drgf(); break;
    case kBKSP: bkspf(); break;
    case kEQU: case kENTR:
        if (rpn) entrf(); else equf();
        break;
    case kLPAR:
        if (rpn) rollf(); else lparf();
        break;
    case kRPAR: case kXXY:
        if (rpn) twof(kXXY); else rparf();
        break;
    case kROLL: rollf(); break;
    case kSTO: case kRCL: case kSUM:
        if (rpn) memf(key); else oneop(key);
        break;
    default: oneop(key); break;
    }
    post_op();
    return !rejected;
}

int
calculator_key(int character)
{
    static const int digits[] = {kZERO, kONE, kTWO, kTHREE, kFOUR,
        kFIVE, kSIX, kSEVEN, kEIGHT, kNINE};
    if (character >= '0' && character <= '9')
        return digits[character - '0'];
    switch (character) {
    case '+': return kADD;
    case '-': return kSUB;
    case '*': return kMUL;
    case '/': return kDIV;
    case '^': return kPOW;
    case '.': return kDEC;
    case '=': case '\r': case '\n': return kEQU;
    case '(': return kLPAR;
    case ')': return kRPAR;
    case '!': return kFACT;
    case 'p': return kPI;
    case 'e': return kE;
    case 'E': return indicators[XCalc_HEX] ? kxE : kEE;
    case 'A': return kxA;
    case 'B': return kxB;
    case 'C': return kxC;
    case 'D': return kxD;
    case 'F': return kxF;
    case '%': return kMOD;
    case '&': return kAND;
    case '|': return kOR;
    case '~': return kNOT;
    case '<': return kSHL;
    case '>': return kSHR;
    case 'x': return kXOR;
    case 'b': return kBASE;
    case 's': return kSIN;
    case 'c': return kCOS;
    case 't': return kTAN;
    case 'l': return kLN;
    case 'r': return kSQRT;
    case 'i': return kINV;
    case 'd': return kDRG;
    case 'n': return kNEG;
    case '\b': case 127: return kBKSP;
    case 27: return kOFF;
    default: return kNOP;
    }
}

const char *
calculator_display(void)
{
    return dispstr;
}

void
calculator_status(char *text, size_t capacity)
{
    snprintf(text, capacity, "%s   %s   %s%s%s%s", rpn ? "RPN" : "Algebraic",
        indicators[XCalc_RADIAN] ? "RAD" :
        indicators[XCalc_GRADAM] ? "GRAD" : "DEG",
        calculator_base(),
        indicators[XCalc_MEMORY] ? "   M" : "",
        indicators[XCalc_INVERSE] ? "   INV" : "",
        indicators[XCalc_PAREN] ? "   ( )" : "");
}

const char *
calculator_base(void)
{
    return indicators[XCalc_HEX] ? "HEX" :
           indicators[XCalc_OCT] ? "OCT" : "DEC";
}

const char *
calculator_angle(void)
{
    return indicators[XCalc_RADIAN] ? "RAD" :
           indicators[XCalc_GRADAM] ? "GRAD" : "DEG";
}
