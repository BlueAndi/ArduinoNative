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
 * @brief  Test the cbuf class.
 * @author Andreas Merkle <web@blue-andi.de>
 */

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <unity.h>
#include <cbuf.h>

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
 * Test that size() is the capacity.
 */
static void testSize(void)
{
    cbuf buffer(8U);

    TEST_ASSERT_EQUAL_UINT(8U, buffer.size());
    TEST_ASSERT_EQUAL_UINT(0U, buffer.available());
    TEST_ASSERT_EQUAL_UINT(8U, buffer.room());

    TEST_ASSERT_EQUAL_UINT(3U, buffer.write("abc", 3U));
    TEST_ASSERT_EQUAL_UINT(8U, buffer.size());
    TEST_ASSERT_EQUAL_UINT(3U, buffer.available());
    TEST_ASSERT_EQUAL_UINT(5U, buffer.room());
}

/**
 * Test writing more than the room.
 */
static void testWriteOverflow(void)
{
    cbuf buffer(4U);
    char data[8U] = { 0 };

    TEST_ASSERT_EQUAL_UINT(4U, buffer.write("abcdef", 6U));
    TEST_ASSERT_EQUAL_UINT(0U, buffer.room());
    TEST_ASSERT_EQUAL_UINT(0U, buffer.write("x", 1U));
    TEST_ASSERT_EQUAL_UINT(0U, buffer.write(nullptr, 1U));

    TEST_ASSERT_EQUAL_UINT(4U, buffer.read(data, sizeof(data)));
    TEST_ASSERT_EQUAL_STRING("abcd", data);
}

/**
 * Test reading in order after the buffer wrapped.
 */
static void testReadAfterWrap(void)
{
    cbuf buffer(4U);
    char data[8U] = { 0 };

    TEST_ASSERT_EQUAL_UINT(4U, buffer.write("abcd", 4U));
    TEST_ASSERT_EQUAL_UINT(2U, buffer.read(data, 2U));
    TEST_ASSERT_EQUAL_MEMORY("ab", data, 2U);
    TEST_ASSERT_EQUAL_UINT(2U, buffer.available());

    /* Consume the rest, which restarts from the beginning. */
    TEST_ASSERT_EQUAL_UINT(2U, buffer.read(data, 2U));
    TEST_ASSERT_EQUAL_MEMORY("cd", data, 2U);
    TEST_ASSERT_EQUAL_UINT(4U, buffer.room());

    TEST_ASSERT_EQUAL_UINT(4U, buffer.write("efgh", 4U));
    memset(data, 0, sizeof(data));
    TEST_ASSERT_EQUAL_UINT(4U, buffer.read(data, sizeof(data)));
    TEST_ASSERT_EQUAL_STRING("efgh", data);
    TEST_ASSERT_EQUAL_UINT(0U, buffer.read(data, sizeof(data)));
}

/**
 * Test that resizeAdd keeps the pending content.
 */
static void testResizeAdd(void)
{
    cbuf buffer(4U);
    char data[16U] = { 0 };

    TEST_ASSERT_FALSE(buffer.resizeAdd(0U));

    TEST_ASSERT_EQUAL_UINT(4U, buffer.write("abcd", 4U));
    TEST_ASSERT_EQUAL_UINT(1U, buffer.read(data, 1U));

    TEST_ASSERT_TRUE(buffer.resizeAdd(4U));
    TEST_ASSERT_EQUAL_UINT(8U, buffer.size());
    TEST_ASSERT_EQUAL_UINT(3U, buffer.available());
    TEST_ASSERT_EQUAL_UINT(5U, buffer.room());

    TEST_ASSERT_EQUAL_UINT(5U, buffer.write("efghi", 5U));

    memset(data, 0, sizeof(data));
    TEST_ASSERT_EQUAL_UINT(8U, buffer.read(data, sizeof(data)));
    TEST_ASSERT_EQUAL_STRING("bcdefghi", data);
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

    RUN_TEST(testSize);
    RUN_TEST(testWriteOverflow);
    RUN_TEST(testReadAfterWrap);
    RUN_TEST(testResizeAdd);

    return UNITY_END();
}
