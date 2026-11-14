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
 * @brief  Test the host compatibility functions.
 * @author Andreas Merkle <web@blue-andi.de>
 */

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <unity.h>
#include <string.h>
#include <time.h>
#include <Arduino.h>
#include <HostCompat.h>

/* On the other hosts the C library provides strptime(), declared in time.h. */

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

/******************************************************************************
 * Local Variables
 *****************************************************************************/

/******************************************************************************
 * Public Methods
 *****************************************************************************/

/******************************************************************************
 * Protected Methods
 *****************************************************************************/

/******************************************************************************
 * Private Methods
 *****************************************************************************/

/******************************************************************************
 * External Functions
 *****************************************************************************/

/** Called before every test. */
void setUp(void)
{
}

/** Called after every test. */
void tearDown(void)
{
}

/**
 * Test a successful conversion.
 */
static void testStrptime(void)
{
    const char* text = "2026-11-14 08:30:05";
    struct tm   timeInfo;
    char*       end = nullptr;

    memset(&timeInfo, 0, sizeof(timeInfo));

    end = strptime(text, "%Y-%m-%d %H:%M:%S", &timeInfo);

    TEST_ASSERT_NOT_NULL(end);
    TEST_ASSERT_EQUAL_PTR(text + strlen(text), end);
    TEST_ASSERT_EQUAL_INT(126, timeInfo.tm_year);
    TEST_ASSERT_EQUAL_INT(10, timeInfo.tm_mon);
    TEST_ASSERT_EQUAL_INT(14, timeInfo.tm_mday);
    TEST_ASSERT_EQUAL_INT(8, timeInfo.tm_hour);
    TEST_ASSERT_EQUAL_INT(30, timeInfo.tm_min);
    TEST_ASSERT_EQUAL_INT(5, timeInfo.tm_sec);
}

/**
 * Test that the pointer to the remaining text is returned.
 */
static void testStrptimeRemainder(void)
{
    const char* text = "14.11.2026 rest";
    struct tm   timeInfo;
    char*       end = nullptr;

    memset(&timeInfo, 0, sizeof(timeInfo));

    end = strptime(text, "%d.%m.%Y", &timeInfo);

    TEST_ASSERT_NOT_NULL(end);
    TEST_ASSERT_EQUAL_STRING(" rest", end);
    TEST_ASSERT_EQUAL_INT(14, timeInfo.tm_mday);
}

/**
 * Test a string which doesn't match the format.
 */
static void testStrptimeMismatch(void)
{
    struct tm timeInfo;

    memset(&timeInfo, 0, sizeof(timeInfo));

    TEST_ASSERT_NULL(strptime("2026/11/14", "%Y-%m-%d", &timeInfo));
    TEST_ASSERT_NULL(strptime("abcd-11-14", "%Y-%m-%d", &timeInfo));
    TEST_ASSERT_NULL(strptime("2026-13-14", "%Y-%m-%d", &timeInfo));
    TEST_ASSERT_NULL(strptime("", "%Y-%m-%d", &timeInfo));
}

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

    RUN_TEST(testStrptime);
    RUN_TEST(testStrptimeRemainder);
    RUN_TEST(testStrptimeMismatch);

    return UNITY_END();
}
