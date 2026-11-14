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
 * @brief  Test the Arduino core functions.
 * @author Andreas Merkle <web@blue-andi.de>
 */

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <unity.h>
#include <Arduino.h>

/******************************************************************************
 * Compiler Switches
 *****************************************************************************/

/******************************************************************************
 * Macros
 *****************************************************************************/

/******************************************************************************
 * Types and classes
 *****************************************************************************/

/******************************************************************************
 * Prototypes
 *****************************************************************************/

static void          testChronoTimeBase(void);
static void          testGpio(void);
static void          testRandom(void);
static void          testMacros(void);
static void          testInjectedTimeBase(void);

static unsigned long fakeTick(void);
static void          fakeDelay(unsigned long ms);

/******************************************************************************
 * Local Variables
 *****************************************************************************/

/** Last delay time, which was requested via the injected delay function. */
static unsigned long gLastDelay = 0UL;

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

    RUN_TEST(testChronoTimeBase);
    RUN_TEST(testGpio);
    RUN_TEST(testRandom);
    RUN_TEST(testMacros);
    RUN_TEST(testInjectedTimeBase);

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
    /* Restore the default time base, independent of the test order. */
    Arduino::setup(nullptr, nullptr);
}

/******************************************************************************
 * Local Functions
 *****************************************************************************/

/**
 * Without injected functions the std::chrono::steady_clock is used.
 */
static void testChronoTimeBase(void)
{
    unsigned long start = millis();

    delay(20U);

    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(20U, millis() - start);
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(millis() * 1000UL - 1000UL, micros());

    yield();
}

/**
 * The GPIO state is just remembered.
 */
static void testGpio(void)
{
    pinMode(5U, OUTPUT);
    digitalWrite(5U, HIGH);
    TEST_ASSERT_EQUAL_INT(HIGH, digitalRead(5U));

    digitalWrite(5U, LOW);
    TEST_ASSERT_EQUAL_INT(LOW, digitalRead(5U));

    /* Out of range */
    digitalWrite(200U, HIGH);
    TEST_ASSERT_EQUAL_INT(LOW, digitalRead(200U));

    TEST_ASSERT_EQUAL_UINT16(0U, analogRead(0U));
}

/**
 * Test the random number generator.
 */
static void testRandom(void)
{
    uint32_t idx = 0U;

    randomSeed(1UL);

    for (idx = 0U; idx < 100U; ++idx)
    {
        long value = random(3, 7);

        TEST_ASSERT_TRUE((3 <= value) && (7 > value));

        value = random(4);

        TEST_ASSERT_TRUE((0 <= value) && (4 > value));
    }

    /* Empty range returns the lower limit. */
    TEST_ASSERT_EQUAL_INT32(5, random(5, 5));
}

/**
 * Test the macros, which have no meaning on the host.
 */
static void testMacros(void)
{
    static const char TEXT[] PROGMEM = "abc";

    TEST_ASSERT_EQUAL_STRING("abc", F("abc"));
    TEST_ASSERT_EQUAL_STRING("abc", PSTR("abc"));
    TEST_ASSERT_EQUAL_STRING("abc", FPSTR(TEXT));
    TEST_ASSERT_EQUAL_UINT8('b', pgm_read_byte(&TEXT[1]));
    TEST_ASSERT_EQUAL_UINT32(3U, strlen_P(TEXT));
    TEST_ASSERT_EQUAL_INT(10, constrain(12, 0, 10));
    TEST_ASSERT_EQUAL_INT(0, constrain(-1, 0, 10));
    TEST_ASSERT_EQUAL_INT(7, constrain(7, 0, 10));

    boolean flag = true;
    TEST_ASSERT_TRUE(flag);
}

/**
 * Injected time functions are used by millis(), micros() and delay().
 */
static void testInjectedTimeBase(void)
{
    gLastDelay = 0UL;

    Arduino::setup(fakeTick, fakeDelay);

    TEST_ASSERT_EQUAL_UINT32(1234U, millis());
    TEST_ASSERT_EQUAL_UINT32(1234000U, micros());

    delay(42UL);
    TEST_ASSERT_EQUAL_UINT32(42U, gLastDelay);

    /* nullptr selects the default again. */
    Arduino::setup(nullptr, nullptr);
    TEST_ASSERT_NOT_EQUAL_UINT32(1234U, millis());
}

/**
 * Fake system tick.
 *
 * @return Always 1234.
 */
static unsigned long fakeTick(void)
{
    return 1234UL;
}

/**
 * Fake delay, which just remembers the requested time.
 *
 * @param[in] ms    Delay time in ms
 */
static void fakeDelay(unsigned long ms)
{
    gLastDelay = ms;
}
