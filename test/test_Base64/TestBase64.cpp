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
 * @brief  Test the base64 encoder.
 * @author Andreas Merkle <web@blue-andi.de>
 */

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <unity.h>
#include <string.h>
#include <libb64/cencode.h>


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
 * Encode a string in one step.
 *
 * @param[in]  text     Text
 * @param[out] output   Encoded text, must be big enough.
 */
static void encode(const char* text, char* output)
{
    int count = base64_encode_chars(text, static_cast<int>(strlen(text)), output);

    TEST_ASSERT_EQUAL_INT(static_cast<int>(strlen(output)), count);
}

/**
 * Test the reference vectors of RFC 4648.
 */
static void testReferenceVectors(void)
{
    char output[32U];

    encode("", output);
    TEST_ASSERT_EQUAL_STRING("", output);

    encode("f", output);
    TEST_ASSERT_EQUAL_STRING("Zg==", output);

    encode("fo", output);
    TEST_ASSERT_EQUAL_STRING("Zm8=", output);

    encode("foo", output);
    TEST_ASSERT_EQUAL_STRING("Zm9v", output);

    encode("foob", output);
    TEST_ASSERT_EQUAL_STRING("Zm9vYg==", output);

    encode("fooba", output);
    TEST_ASSERT_EQUAL_STRING("Zm9vYmE=", output);

    encode("foobar", output);
    TEST_ASSERT_EQUAL_STRING("Zm9vYmFy", output);
}

/**
 * Test a long input, which must not be broken into lines.
 */
static void testNoLineBreak(void)
{
    const char* text = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/ABCDEFGH";
    char        output[base64_encode_expected_len(72U) + 1U];

    TEST_ASSERT_EQUAL_UINT(72U, strlen(text));

    encode(text, output);

    TEST_ASSERT_EQUAL_UINT(96U, strlen(output));
    TEST_ASSERT_NULL(strchr(output, '\n'));
    TEST_ASSERT_NULL(strchr(output, '\r'));
    TEST_ASSERT_EQUAL_STRING("QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVphYmNkZWZnaGlqa2xtbm9wcXJzdHV2d3h5ejAxMjM0NTY3ODkrL0FCQ0RFRkdI", output);
}

/**
 * Test the streaming interface in several blocks.
 */
static void testStreaming(void)
{
    base64_encodestate state;
    char               output[16U] = { 0 };
    int                count       = 0;

    base64_init_encodestate(&state);
    count += base64_encode_block("fo", 2, &output[count], &state);
    count += base64_encode_block("obar", 4, &output[count], &state);
    count += base64_encode_blockend(&output[count], &state);

    TEST_ASSERT_EQUAL_INT(8, count);
    TEST_ASSERT_EQUAL_STRING("Zm9vYmFy", output);
}

/**
 * Test the buffer size macro.
 */
static void testExpectedLen(void)
{
    TEST_ASSERT_EQUAL_UINT(0U, base64_encode_expected_len(0U));
    TEST_ASSERT_EQUAL_UINT(4U, base64_encode_expected_len(1U));
    TEST_ASSERT_EQUAL_UINT(4U, base64_encode_expected_len(3U));
    TEST_ASSERT_EQUAL_UINT(8U, base64_encode_expected_len(4U));
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

    RUN_TEST(testReferenceVectors);
    RUN_TEST(testNoLineBreak);
    RUN_TEST(testStreaming);
    RUN_TEST(testExpectedLen);

    return UNITY_END();
}
