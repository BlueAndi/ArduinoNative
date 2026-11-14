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
 * @brief  Test the load path of the Preferences class.
 * @author Andreas Merkle <web@blue-andi.de>
 *
 * The storage file is loaded once per process, on the first access. Therefore
 * this suite writes the storage file before the first access and checks
 * afterwards that its content is available.
 */

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <unity.h>
#include <stdio.h>
#include <string.h>
#include <Arduino.h>
#include <FS.h>
#include <Preferences.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

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

/** Temporary directory, relative to the working directory. */
static const char* TEST_DIR = "tmp_test_preferences_load";

/** Storage file content, in the format the implementation writes. */
static const char* STORAGE_CONTENT =
    "first\tname\tHello\n"
    "first\tcount\t-42\n"
    "first\tspecial\ta\\tb\\nc\\r\\nd\\\\e\\\\n\n"
    "second\tname\tOther\n"
    "second\tflag\t1\n";

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
 * Change the working directory.
 *
 * @param[in] path  New working directory.
 *
 * @return If successful, it will return true otherwise false.
 */
static bool changeDirectory(const char* path)
{
#ifdef _WIN32
    return (0 == _chdir(path));
#else
    return (0 == chdir(path));
#endif
}

/**
 * Create the temporary directory, change into it and write the storage file.
 * Must be called before the first access to any Preferences instance.
 */
static void prepare(void)
{
    FS    rootFs;
    FILE* fd = nullptr;

    (void)rootFs.begin(".");
    (void)rootFs.mkdir(TEST_DIR);
    (void)changeDirectory(TEST_DIR);

    fd = fopen("preferences.txt", "wb");

    if (nullptr != fd)
    {
        (void)fwrite(STORAGE_CONTENT, 1U, strlen(STORAGE_CONTENT), fd);
        (void)fclose(fd);
    }
}

/**
 * Remove the storage file and the temporary directory.
 */
static void cleanup(void)
{
    FS rootFs;

    (void)remove("preferences.txt");
    (void)changeDirectory("..");
    (void)rootFs.begin(".");
    (void)rootFs.rmdir(TEST_DIR);
}

/**
 * Test the values of the first namespace, including the escaped characters.
 */
static void testLoadFirst(void)
{
    Preferences pref;

    TEST_ASSERT_TRUE(pref.begin("first", true));
    TEST_ASSERT_EQUAL_STRING("Hello", pref.getString("name", "").c_str());
    TEST_ASSERT_EQUAL_INT32(-42, pref.getInt("count", 0));
    TEST_ASSERT_EQUAL_STRING("a\tb\nc\r\nd\\e\\n", pref.getString("special", "").c_str());
    pref.end();
}

/**
 * Test the values of the second namespace and the isolation.
 */
static void testLoadSecond(void)
{
    Preferences pref;

    TEST_ASSERT_TRUE(pref.begin("second", true));
    TEST_ASSERT_EQUAL_STRING("Other", pref.getString("name", "").c_str());
    TEST_ASSERT_TRUE(pref.getBool("flag", false));
    TEST_ASSERT_EQUAL_INT32(-1, pref.getInt("count", -1));
    pref.end();
}

/**
 * Test that a new value and the loaded ones coexist.
 */
static void testCoexist(void)
{
    Preferences pref;

    TEST_ASSERT_TRUE(pref.begin("first"));
    TEST_ASSERT_EQUAL_UINT32(4U, pref.putString("added", "x\ty\n"));
    pref.end();

    TEST_ASSERT_TRUE(pref.begin("first", true));
    TEST_ASSERT_EQUAL_STRING("x\ty\n", pref.getString("added", "").c_str());
    TEST_ASSERT_EQUAL_STRING("Hello", pref.getString("name", "").c_str());
    TEST_ASSERT_EQUAL_STRING("a\tb\nc\r\nd\\e\\n", pref.getString("special", "").c_str());
    pref.end();

    TEST_ASSERT_TRUE(pref.begin("second", true));
    TEST_ASSERT_EQUAL_STRING("Other", pref.getString("name", "").c_str());
    pref.end();
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
    int result = 0;

    (void)argc;
    (void)argv;

    /* Before the first access, otherwise the storage is loaded without it. */
    prepare();

    UNITY_BEGIN();

    RUN_TEST(testLoadFirst);
    RUN_TEST(testLoadSecond);
    RUN_TEST(testCoexist);

    result = UNITY_END();

    cleanup();

    return result;
}
