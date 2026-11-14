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
 * @brief  Test the Preferences class.
 * @author Andreas Merkle <web@blue-andi.de>
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
static const char* TEST_DIR = "tmp_test_preferences";

/** Is the working directory changed into the temporary directory? */
static bool gIsInTestDir = false;

/** Namespace used by the tests. */
static const char* NAMESPACE = "test";

/** Second namespace, to check the isolation. */
static const char* NAMESPACE_OTHER = "other";

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
 * Called before every test.
 *
 * The preferences are stored in a file in the working directory, therefore the
 * working directory is changed to a temporary one. A leftover of an aborted
 * run is removed first.
 */
void setUp(void)
{
    FS rootFs;

    TEST_ASSERT_TRUE(rootFs.begin("."));
    (void)rootFs.remove(String(TEST_DIR) + "/preferences.txt");
    (void)rootFs.rmdir(TEST_DIR);

    TEST_ASSERT_TRUE(rootFs.mkdir(TEST_DIR));
    TEST_ASSERT_TRUE(changeDirectory(TEST_DIR));
    gIsInTestDir = true;
}

/**
 * Called after every test.
 *
 * The storage is shared by all instances and kept in memory for the whole
 * process, therefore the namespaces are cleared. Afterwards the storage file
 * and the temporary directory are removed.
 */
void tearDown(void)
{
    FS rootFs;

    /* Without the temporary directory nothing was used, and clearing would
     * write the storage file into the wrong directory.
     */
    if (true == gIsInTestDir)
    {
        Preferences pref;

        if (true == pref.begin(NAMESPACE, false))
        {
            (void)pref.clear();
            pref.end();
        }

        if (true == pref.begin(NAMESPACE_OTHER, false))
        {
            (void)pref.clear();
            pref.end();
        }

        (void)remove("preferences.txt");
        (void)changeDirectory("..");
        gIsInTestDir = false;
    }

    (void)rootFs.begin(".");
    (void)rootFs.rmdir(TEST_DIR);
}

/**
 * Test the typed values.
 */
static void testPutGet(void)
{
    Preferences pref;

    TEST_ASSERT_TRUE(pref.begin(NAMESPACE));

    /* Like the ESP32 the stored size is returned: the size of the type and
     * not the length of its text representation.
     */
    TEST_ASSERT_EQUAL_UINT32(1U, pref.putBool("bool", true));
    TEST_ASSERT_EQUAL_UINT32(4U, pref.putInt("int", -42));
    TEST_ASSERT_EQUAL_UINT32(4U, pref.putUInt("uint", 4000000000U));
    TEST_ASSERT_EQUAL_UINT32(1U, pref.putUChar("uchar", 255U));
    TEST_ASSERT_EQUAL_UINT32(5U, pref.putString("string", "Hello"));

    TEST_ASSERT_TRUE(pref.getBool("bool", false));
    TEST_ASSERT_EQUAL_INT32(-42, pref.getInt("int", 0));
    TEST_ASSERT_EQUAL_UINT32(4000000000U, pref.getUInt("uint", 0U));
    TEST_ASSERT_EQUAL_UINT8(255U, pref.getUChar("uchar", 0U));
    TEST_ASSERT_EQUAL_STRING("Hello", pref.getString("string", "").c_str());

    TEST_ASSERT_EQUAL_UINT32(1U, pref.putBool("bool", false));
    TEST_ASSERT_FALSE(pref.getBool("bool", true));

    pref.end();
}

/**
 * Test the default value of a missing key.
 */
static void testDefault(void)
{
    Preferences pref;

    TEST_ASSERT_TRUE(pref.begin(NAMESPACE));

    TEST_ASSERT_TRUE(pref.getBool("missing", true));
    TEST_ASSERT_EQUAL_INT32(-7, pref.getInt("missing", -7));
    TEST_ASSERT_EQUAL_UINT32(7U, pref.getUInt("missing", 7U));
    TEST_ASSERT_EQUAL_UINT8(9U, pref.getUChar("missing", 9U));
    TEST_ASSERT_EQUAL_STRING("def", pref.getString("missing", "def").c_str());
    TEST_ASSERT_EQUAL_STRING("", pref.getString("missing").c_str());

    pref.end();
}

/**
 * Test that a value is available in a new instance after end().
 *
 * The storage is shared by all instances of the process and loaded from the
 * file only once. This test therefore proves the sharing, not the reload from
 * the file; the write-through to the file is checked separately.
 */
static void testSurvivesEnd(void)
{
    Preferences pref;

    TEST_ASSERT_TRUE(pref.begin(NAMESPACE));
    (void)pref.putString("string", "kept");
    (void)pref.putInt("int", 1234);
    pref.end();

    /* Closed, therefore nothing is available. */
    TEST_ASSERT_EQUAL_INT32(-1, pref.getInt("int", -1));

    Preferences other;

    TEST_ASSERT_TRUE(other.begin(NAMESPACE, true));
    TEST_ASSERT_EQUAL_STRING("kept", other.getString("string", "").c_str());
    TEST_ASSERT_EQUAL_INT32(1234, other.getInt("int", 0));
    other.end();

    /* Reopen the same instance. */
    TEST_ASSERT_TRUE(pref.begin(NAMESPACE));
    TEST_ASSERT_EQUAL_INT32(1234, pref.getInt("int", 0));
    pref.end();
}

/**
 * Test that every modification is written through to the storage file.
 */
static void testWriteThrough(void)
{
    Preferences pref;
    char        buffer[64U] = {0};

    TEST_ASSERT_TRUE(pref.begin(NAMESPACE));
    (void)pref.putString("key", "value");
    pref.end();

    FILE* fd = fopen("preferences.txt", "rb");

    TEST_ASSERT_NOT_NULL(fd);
    (void)fread(buffer, 1U, sizeof(buffer) - 1U, fd);
    (void)fclose(fd);

    TEST_ASSERT_EQUAL_STRING("test\tkey\tvalue\n", buffer);
}

/**
 * Test that tabs and line breaks come back unchanged.
 */
static void testSpecialCharacters(void)
{
    Preferences pref;
    const char* value = "a\tb\nc\r\nd\\e\\n";

    TEST_ASSERT_TRUE(pref.begin(NAMESPACE));
    TEST_ASSERT_EQUAL_UINT32(strlen(value), pref.putString("special", value));
    TEST_ASSERT_EQUAL_STRING(value, pref.getString("special", "").c_str());
    pref.end();

    TEST_ASSERT_TRUE(pref.begin(NAMESPACE));
    TEST_ASSERT_EQUAL_STRING(value, pref.getString("special", "").c_str());
    pref.end();

    /* The file keeps one entry per line, with the value escaped. */
    char  buffer[64U] = {0};
    FILE* fd          = fopen("preferences.txt", "rb");

    TEST_ASSERT_NOT_NULL(fd);
    (void)fread(buffer, 1U, sizeof(buffer) - 1U, fd);
    (void)fclose(fd);

    TEST_ASSERT_EQUAL_STRING("test\tspecial\ta\\tb\\nc\\r\\nd\\\\e\\\\n\n", buffer);
}

/**
 * Test a read-only namespace.
 */
static void testReadOnly(void)
{
    Preferences pref;

    TEST_ASSERT_TRUE(pref.begin(NAMESPACE));
    (void)pref.putInt("int", 5);
    pref.end();

    TEST_ASSERT_TRUE(pref.begin(NAMESPACE, true));
    TEST_ASSERT_EQUAL_UINT32(0U, pref.putBool("bool", true));
    TEST_ASSERT_EQUAL_UINT32(0U, pref.putInt("int", 6));
    TEST_ASSERT_EQUAL_UINT32(0U, pref.putUInt("uint", 6U));
    TEST_ASSERT_EQUAL_UINT32(0U, pref.putUChar("uchar", 6U));
    TEST_ASSERT_EQUAL_UINT32(0U, pref.putString("string", "x"));
    TEST_ASSERT_FALSE(pref.remove("int"));
    TEST_ASSERT_FALSE(pref.clear());

    /* Reading works and the value is unchanged. */
    TEST_ASSERT_EQUAL_INT32(5, pref.getInt("int", 0));
    TEST_ASSERT_FALSE(pref.getBool("bool", false));
    pref.end();
}

/**
 * Test a namespace which is not opened.
 */
static void testNotOpen(void)
{
    Preferences pref;

    TEST_ASSERT_EQUAL_UINT32(0U, pref.putInt("int", 6));
    TEST_ASSERT_FALSE(pref.remove("int"));
    TEST_ASSERT_FALSE(pref.clear());
    TEST_ASSERT_FALSE(pref.begin(nullptr));

    TEST_ASSERT_TRUE(pref.begin(NAMESPACE));
    TEST_ASSERT_FALSE(pref.begin(NAMESPACE_OTHER));
    TEST_ASSERT_EQUAL_UINT32(0U, pref.putInt(nullptr, 6));
    pref.end();
}

/**
 * Test clear() and remove().
 */
static void testClearRemove(void)
{
    Preferences pref;
    Preferences other;

    TEST_ASSERT_TRUE(pref.begin(NAMESPACE));
    TEST_ASSERT_TRUE(other.begin(NAMESPACE_OTHER));

    (void)pref.putInt("a", 1);
    (void)pref.putInt("b", 2);
    (void)other.putInt("a", 3);

    TEST_ASSERT_TRUE(pref.remove("a"));
    TEST_ASSERT_FALSE(pref.remove("a"));
    TEST_ASSERT_EQUAL_INT32(-1, pref.getInt("a", -1));
    TEST_ASSERT_EQUAL_INT32(2, pref.getInt("b", -1));

    TEST_ASSERT_TRUE(pref.clear());
    TEST_ASSERT_EQUAL_INT32(-1, pref.getInt("b", -1));

    /* Another namespace is not affected. */
    TEST_ASSERT_EQUAL_INT32(3, other.getInt("a", -1));

    pref.end();
    other.end();
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

    RUN_TEST(testPutGet);
    RUN_TEST(testDefault);
    RUN_TEST(testSurvivesEnd);
    RUN_TEST(testWriteThrough);
    RUN_TEST(testSpecialCharacters);
    RUN_TEST(testReadOnly);
    RUN_TEST(testNotOpen);
    RUN_TEST(testClearRemove);

    return UNITY_END();
}
