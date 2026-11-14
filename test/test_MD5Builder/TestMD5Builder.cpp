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
 * @brief  Test the MD5Builder class.
 * @author Andreas Merkle <web@blue-andi.de>
 */

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <unity.h>
#include <string.h>
#include <Arduino.h>
#include <MD5Builder.h>


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
 * Calculate the digest of a single string.
 *
 * @param[in] text  Text
 *
 * @return Digest as hexadecimal string
 */
static String md5Of(const char* text)
{
    MD5Builder builder;

    builder.begin();
    builder.add(text);
    builder.calculate();

    return builder.toString();
}

/**
 * Test the reference vectors of RFC 1321.
 */
static void testReferenceVectors(void)
{
    TEST_ASSERT_EQUAL_STRING("d41d8cd98f00b204e9800998ecf8427e", md5Of("").c_str());
    TEST_ASSERT_EQUAL_STRING("900150983cd24fb0d6963f7d28e17f72", md5Of("abc").c_str());
    TEST_ASSERT_EQUAL_STRING("f96b697d7cb7938d525a2f31aaf161d0", md5Of("message digest").c_str());
}

/**
 * Test an input over more than one block, added in two parts.
 */
static void testMultiBlock(void)
{
    /* 80 characters, see the RFC 1321 test suite. */
    const char* text   = "12345678901234567890123456789012345678901234567890123456789012345678901234567890";
    String      first  = String(text).substring(0, 37);
    String      second = String(text).substring(37);
    MD5Builder  builder;

    TEST_ASSERT_EQUAL_UINT(80U, strlen(text));

    builder.begin();
    builder.add(first);
    builder.add(second);
    builder.calculate();

    TEST_ASSERT_EQUAL_STRING("57edf4a22be3c955ac49da2e2107b67a", builder.toString().c_str());
    TEST_ASSERT_EQUAL_STRING("57edf4a22be3c955ac49da2e2107b67a", md5Of(text).c_str());
}

/**
 * Test getChars() and getBytes().
 */
static void testOutputs(void)
{
    MD5Builder builder;
    char       chars[33U];
    uint8_t    bytes[16U];

    builder.begin();
    builder.add(reinterpret_cast<const uint8_t*>("abc"), 3U);
    builder.calculate();
    builder.getChars(chars);
    builder.getBytes(bytes);

    TEST_ASSERT_EQUAL_STRING("900150983cd24fb0d6963f7d28e17f72", chars);
    TEST_ASSERT_EQUAL_UINT8(0x90U, bytes[0U]);
    TEST_ASSERT_EQUAL_UINT8(0x72U, bytes[15U]);

    /* A new calculation starts from scratch. */
    builder.begin();
    builder.calculate();
    TEST_ASSERT_EQUAL_STRING("d41d8cd98f00b204e9800998ecf8427e", builder.toString().c_str());
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
    RUN_TEST(testMultiBlock);
    RUN_TEST(testOutputs);

    return UNITY_END();
}
