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
#include <stdio.h>

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
