/* Headless regression checks for the imported scientific calculator engine.
 * Real key sequences cover precedence, modes, memory and error recovery. */
#include "calculator.h"
#include <assert.h>

static void
type(const char *text)
{
    for (; *text; ++text)
        calculator_press(calculator_key((unsigned char)*text));
}

static void
expect(double result)
{
    char *end;
    double value = strtod(calculator_display(), &end);
    assert(*end == '\0');
    if (fabs(value - result) > 1e-7 * fmax(1.0, fabs(result))) {
        fprintf(stderr, "Expected %.12g, got %s\n", result,
                calculator_display());
        abort();
    }
}

static void
expression(const char *text, double result)
{
    calculator_reset(0);
    type(text);
    expect(result);
}

static void
error(const char *text)
{
    calculator_reset(0);
    type(text);
    assert(!strcmp(calculator_display(), "Error"));
    calculator_press(kCLR);
    type("2+3=");
    expect(5);
}

int
main(void)
{
    expression("2+3*4=", 14);
    expression("100-20*2-5=", 55);
    expression("100/2*5=", 250);
    expression("(2+3)*4=", 20);
    expression("2*(3+(4*5))=", 46);
    expression("2^3=", 8);
    expression("5!", 120);
    expression("30s", 0.5);
    expression("0.5is", 30);
    expression("9r", 3);
    expression("1E3rn", -sqrt(1000));
    expression("2", 2);
    calculator_press(kLOG);
    expect(log10(2));
    calculator_reset(0);
    type("2i");
    calculator_press(kLOG);
    expect(100);
    expression("p", 3.141592653589793);
    expression("e", 2.718281828459045);
    expression("1E3+2=", 1002);
    expression("1E3n", 0.001);
    expression("5nn", 5);
    expression("12\b3", 13);
    expression("1E2\b\b3", 13);
    expression("1.2+3=.5", 0.5);
    error("1/0=");
    error("1nr");
    error("0l");
    error("171!");
    error("999^999=");
    calculator_reset(0);
    for (int i = 0; i < 40; ++i)
        calculator_press(kLPAR);
    assert(!strcmp(calculator_display(), "Error"));
    calculator_press(kOFF);
    type("4+5=");
    expect(9);

    calculator_reset(0);
    type("42");
    calculator_press(kSTO);
    calculator_press(kCLR);
    calculator_press(kRCL);
    expect(42);
    calculator_press(kSUM);
    calculator_press(kRCL);
    expect(84);
    calculator_press(kDRG);
    type("p");
    calculator_press(kCOS);
    expect(-1);
    calculator_press(kDRG);
    calculator_press(kCLR);
    type("100s");
    expect(1);

    calculator_reset(1);
    type("2\r3+");
    expect(5);
    type("4*");
    expect(20);
    calculator_reset(1);
    type("7");
    calculator_press(kSTO);
    type("2");
    calculator_press(kCLR);
    calculator_press(kRCL);
    type("2");
    expect(7);
    calculator_reset(1);
    type("2\r3");
    calculator_press(kXXY);
    expect(2);
    calculator_press(kXXY);
    expect(3);
    calculator_reset(0);
    calculator_press(kRCL);
    expect(0);
    expression("5&3=", 1);
    expression("5|2=", 7);
    expression("5x3=", 6);
    expression("1<4=", 16);
    expression("16>2=", 4);
    expression("7%3=", 1);
    error("1%0=");
    error("1<32=");
    error("1n&2=");
    error("1.5|1=");
    calculator_reset(0);
    type("255b");
    assert(!strcmp(calculator_display(), "FF"));
    type("&F=");
    assert(!strcmp(calculator_display(), "F"));
    type("b");
    assert(!strcmp(calculator_display(), "17"));
    type("b");
    expect(15);
    calculator_reset(0);
    type("4294967295b");
    assert(!strcmp(calculator_display(), "FFFFFFFF"));
    type("<1=");
    assert(!strcmp(calculator_display(), "FFFFFFFE"));
    type("~");
    assert(!strcmp(calculator_display(), "1"));
    calculator_reset(1);
    type("7\r3&");
    expect(3);
    /* Exercise malformed/long edit sequences under the sanitizers as well. */
    unsigned int random_state = 12345;
    for (int i = 0; i < 100000; ++i) {
        if (i % 128 == 0)
            calculator_reset((i / 128) % 2);
        random_state = random_state * 1664525U + 1013904223U;
        calculator_press((int)(random_state % (kxF + 1)));
        assert(strlen(calculator_display()) < LCD_STR_LEN);
    }
    puts("Scientific, programmer, algebraic, RPN, memory, editing and error checks passed.");
    return 0;
}
