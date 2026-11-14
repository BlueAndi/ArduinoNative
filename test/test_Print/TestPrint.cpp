/* MIT License
 *
 * Copyright (c) 2023 - 2026 Andreas Merkle <web@blue-andi.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*******************************************************************************
    DESCRIPTION
*******************************************************************************/
/**
 * @brief  Test the Print class.
 * @author Andreas Merkle <web@blue-andi.de>
 */

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <unity.h>
#include <Print.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/******************************************************************************
 * Compiler Switches
 *****************************************************************************/

/******************************************************************************
 * Macros
 *****************************************************************************/

/******************************************************************************
 * Types and classes
 *****************************************************************************/

/**
 * Print implementation, which appends everything to a string.
 */
class StringPrint : public Print
{
public:

    StringPrint() :
        Print(),
        m_buffer()
    {
    }

    ~StringPrint() final
    {
    }

    size_t write(uint8_t data) final
    {
        m_buffer += static_cast<char>(data);
        return 1U;
    }

    using Print::write;

    const String& get() const
    {
        return m_buffer;
    }

private:

    String m_buffer; /**< Collected output. */
};

/******************************************************************************
 * Prototypes
 *****************************************************************************/

static void testPrintString(void);
static void testPrintNumber(void);
static void testPrintln(void);
static void testPrintLimits(void);
static void testPrintNullptr(void);
static void testPrintOverloads(void);
static void testPrintLongLong(void);
static void testPrintNegativeBases(void);
static void testPrintInvalidBase(void);
static void testPrintDouble(void);

/******************************************************************************
 * Local Variables
 *****************************************************************************/

/******************************************************************************
 * External Functions
 *****************************************************************************/

/** Application setup, required by the library. */
void setup()
{
}

/** Application loop, required by the library. */
void loop()
{
}

/**
 * Main entry point
 *
 * @param[in] argc  Number of command line arguments
 * @param[in] argv  Command line arguments
 */
extern int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    UNITY_BEGIN();

    RUN_TEST(testPrintString);
    RUN_TEST(testPrintNumber);
    RUN_TEST(testPrintln);
    RUN_TEST(testPrintLimits);
    RUN_TEST(testPrintNullptr);
    RUN_TEST(testPrintOverloads);
    RUN_TEST(testPrintLongLong);
    RUN_TEST(testPrintNegativeBases);
    RUN_TEST(testPrintInvalidBase);
    RUN_TEST(testPrintDouble);

    return UNITY_END();
}

/**
 * Setup a test. This function will be called before every test by unity.
 */
extern void setUp(void)
{
    /* Not used. */
}

/**
 * Clean up test. This function will be called after every test by unity.
 */
extern void tearDown(void)
{
    /* Not used. */
}

/******************************************************************************
 * Local Functions
 *****************************************************************************/

/**
 * Test printing strings.
 */
static void testPrintString(void)
{
    StringPrint out;

    TEST_ASSERT_EQUAL_UINT(3U, out.print("abc"));
    TEST_ASSERT_EQUAL_STRING("abc", out.get().c_str());
}

/**
 * Test printing numbers with different bases.
 */
static void testPrintNumber(void)
{
    StringPrint hex;
    hex.print(255, HEX);
    TEST_ASSERT_EQUAL_STRING("FF", hex.get().c_str());

    StringPrint bin;
    bin.print(5, BIN);
    TEST_ASSERT_EQUAL_STRING("101", bin.get().c_str());

    StringPrint neg;
    neg.print(-12);
    TEST_ASSERT_EQUAL_STRING("-12", neg.get().c_str());
}

/**
 * Test println.
 */
static void testPrintln(void)
{
    StringPrint out;

    out.println("a");
    TEST_ASSERT_EQUAL_STRING("a\r\n", out.get().c_str());
}

/**
 * Test the limits of the number range.
 */
static void testPrintLimits(void)
{
    char expected[32];

    StringPrint big;
    big.print(4294967295UL);
    TEST_ASSERT_EQUAL_STRING("4294967295", big.get().c_str());

    StringPrint minimum;
    (void)snprintf(expected, sizeof(expected), "%ld", LONG_MIN);
    minimum.print(static_cast<long>(LONG_MIN));
    TEST_ASSERT_EQUAL_STRING(expected, minimum.get().c_str());

    StringPrint maximum;
    (void)snprintf(expected, sizeof(expected), "%lu", ULONG_MAX);
    maximum.print(ULONG_MAX);
    TEST_ASSERT_EQUAL_STRING(expected, maximum.get().c_str());

    StringPrint uintMax;
    (void)snprintf(expected, sizeof(expected), "%u", UINT_MAX);
    uintMax.print(UINT_MAX);
    TEST_ASSERT_EQUAL_STRING(expected, uintMax.get().c_str());
}

/**
 * Test printing a null pointer string.
 */
static void testPrintNullptr(void)
{
    StringPrint out;

    TEST_ASSERT_EQUAL_UINT(0U, out.print(static_cast<const char*>(nullptr)));
    TEST_ASSERT_EQUAL_STRING("", out.get().c_str());
}

/**
 * Test that the common integer types are not ambiguous, independent of the host.
 */
static void testPrintOverloads(void)
{
    StringPrint sizeOfInt;
    TEST_ASSERT_EQUAL_UINT(1U, sizeOfInt.print(sizeof(int)));
    TEST_ASSERT_EQUAL_STRING("4", sizeOfInt.get().c_str());

    StringPrint size;
    TEST_ASSERT_EQUAL_UINT(3U, size.print(static_cast<size_t>(123U)));
    TEST_ASSERT_EQUAL_STRING("123", size.get().c_str());

    StringPrint i64;
    TEST_ASSERT_EQUAL_UINT(2U, i64.print(static_cast<int64_t>(-5)));
    TEST_ASSERT_EQUAL_STRING("-5", i64.get().c_str());

    StringPrint u64;
    TEST_ASSERT_EQUAL_UINT(2U, u64.print(static_cast<uint64_t>(42U)));
    TEST_ASSERT_EQUAL_STRING("42", u64.get().c_str());

    StringPrint u8;
    TEST_ASSERT_EQUAL_UINT(3U, u8.print(static_cast<uint8_t>(255U)));
    TEST_ASSERT_EQUAL_STRING("255", u8.get().c_str());

    StringPrint u8Hex;
    u8Hex.print(static_cast<uint8_t>(255U), HEX);
    TEST_ASSERT_EQUAL_STRING("FF", u8Hex.get().c_str());

    StringPrint i16;
    i16.print(static_cast<int16_t>(-3));
    TEST_ASSERT_EQUAL_STRING("-3", i16.get().c_str());

    StringPrint u16;
    u16.print(static_cast<uint16_t>(65535U));
    TEST_ASSERT_EQUAL_STRING("65535", u16.get().c_str());

    /* A single character is printed as character, not as number. */
    StringPrint chr;
    TEST_ASSERT_EQUAL_UINT(1U, chr.print('A'));
    TEST_ASSERT_EQUAL_STRING("A", chr.get().c_str());
}

/**
 * Test the 64-bit types, which are long or long long depending on the host.
 */
static void testPrintLongLong(void)
{
    StringPrint minimum;
    TEST_ASSERT_EQUAL_UINT(20U, minimum.print(LLONG_MIN));
    TEST_ASSERT_EQUAL_STRING("-9223372036854775808", minimum.get().c_str());

    StringPrint maximum;
    TEST_ASSERT_EQUAL_UINT(20U, maximum.print(ULLONG_MAX));
    TEST_ASSERT_EQUAL_STRING("18446744073709551615", maximum.get().c_str());

    StringPrint hex;
    hex.print(255LL, HEX);
    TEST_ASSERT_EQUAL_STRING("FF", hex.get().c_str());

    StringPrint hexUnsigned;
    hexUnsigned.print(0xDEADBEEFCAFEULL, HEX);
    TEST_ASSERT_EQUAL_STRING("DEADBEEFCAFE", hexUnsigned.get().c_str());

    StringPrint bin;
    bin.print(5LL, BIN);
    TEST_ASSERT_EQUAL_STRING("101", bin.get().c_str());

    StringPrint hexNeg;
    hexNeg.print(-1LL, HEX);
    TEST_ASSERT_EQUAL_STRING("FFFFFFFFFFFFFFFF", hexNeg.get().c_str());
}

/**
 * Test negative values with a base other than decimal: the two's complement of
 * the width of the argument's type is printed, like Arduino does it.
 */
static void testPrintNegativeBases(void)
{
    StringPrint hexInt;
    TEST_ASSERT_EQUAL_UINT(8U, hexInt.print(-1, HEX));
    TEST_ASSERT_EQUAL_STRING("FFFFFFFF", hexInt.get().c_str());

    StringPrint binInt;
    binInt.print(-2, BIN);
    TEST_ASSERT_EQUAL_STRING("11111111111111111111111111111110", binInt.get().c_str());

    StringPrint octInt;
    octInt.print(-1, OCT);
    TEST_ASSERT_EQUAL_STRING("37777777777", octInt.get().c_str());

    /* The width of long depends on the host: 4 or 8 byte. */
    char expected[32] = { 0 };

    memset(expected, 'F', sizeof(long) * 2U);

    StringPrint hexLong;
    hexLong.print(-1L, HEX);
    TEST_ASSERT_EQUAL_STRING(expected, hexLong.get().c_str());

    /* A short is promoted to int. */
    StringPrint hexShort;
    hexShort.print(static_cast<short>(-1), HEX);
    TEST_ASSERT_EQUAL_STRING("FFFFFFFF", hexShort.get().c_str());

    /* In decimal the sign is printed. */
    StringPrint dec;
    TEST_ASSERT_EQUAL_UINT(4U, dec.print(-255, DEC));
    TEST_ASSERT_EQUAL_STRING("-255", dec.get().c_str());
}

/**
 * Test the number bases, which are not supported: they fall back to decimal.
 */
static void testPrintInvalidBase(void)
{
    StringPrint base0;
    base0.print(42, 0);
    TEST_ASSERT_EQUAL_STRING("42", base0.get().c_str());

    StringPrint base1;
    base1.print(42U, 1);
    TEST_ASSERT_EQUAL_STRING("42", base1.get().c_str());

    StringPrint negative;
    negative.print(-42L, 1);
    TEST_ASSERT_EQUAL_STRING("-42", negative.get().c_str());

    StringPrint negativeBase0;
    negativeBase0.print(-42, 0);
    TEST_ASSERT_EQUAL_STRING("-42", negativeBase0.get().c_str());

    StringPrint tooLarge;
    tooLarge.print(255UL, 17);
    TEST_ASSERT_EQUAL_STRING("255", tooLarge.get().c_str());

    StringPrint negativeBase;
    negativeBase.print(255, -1);
    TEST_ASSERT_EQUAL_STRING("255", negativeBase.get().c_str());

    /* Bases between 2 and 16 are all supported. */
    StringPrint base7;
    base7.print(50, 7);
    TEST_ASSERT_EQUAL_STRING("101", base7.get().c_str());
}

/**
 * Test printing floating point numbers, like the ESP32 Arduino core does it.
 */
static void testPrintDouble(void)
{
    StringPrint def;
    TEST_ASSERT_EQUAL_UINT(4U, def.print(3.14159));
    TEST_ASSERT_EQUAL_STRING("3.14", def.get().c_str());

    StringPrint digits;
    digits.print(3.14159, 4);
    TEST_ASSERT_EQUAL_STRING("3.1416", digits.get().c_str());

    StringPrint rounded;
    TEST_ASSERT_EQUAL_UINT(1U, rounded.print(1.5, 0));
    TEST_ASSERT_EQUAL_STRING("2", rounded.get().c_str());

    StringPrint negative;
    negative.print(-1.26, 1);
    TEST_ASSERT_EQUAL_STRING("-1.3", negative.get().c_str());

    StringPrint single;
    single.print(2.5F, 1);
    TEST_ASSERT_EQUAL_STRING("2.5", single.get().c_str());

    StringPrint negativeDigits;
    negativeDigits.print(7.9, -3);
    TEST_ASSERT_EQUAL_STRING("8", negativeDigits.get().c_str());

    StringPrint notANumber;
    notANumber.print(NAN);
    TEST_ASSERT_EQUAL_STRING("nan", notANumber.get().c_str());

    StringPrint infinity;
    infinity.print(INFINITY);
    TEST_ASSERT_EQUAL_STRING("inf", infinity.get().c_str());

    StringPrint negInfinity;
    negInfinity.print(-INFINITY);
    TEST_ASSERT_EQUAL_STRING("-inf", negInfinity.get().c_str());
}
