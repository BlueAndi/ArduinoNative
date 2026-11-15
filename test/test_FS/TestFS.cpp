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
 * @brief  Test the filesystem.
 * @author Andreas Merkle <web@blue-andi.de>
 */

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <unity.h>
#include <string.h>
#include <Arduino.h>
#include <FS.h>

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

static void removeTestFiles(void);

/******************************************************************************
 * Local Variables
 *****************************************************************************/

/** Temporary directory, relative to the working directory. */
static const char* TEST_DIR = "tmp_test_fs";

/** Filesystem under test, mounted in the temporary directory. */
static FS gFs;

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
    FS rootFs;

    removeTestFiles();

    TEST_ASSERT_TRUE(rootFs.begin("."));
    TEST_ASSERT_TRUE(rootFs.mkdir(TEST_DIR));
    TEST_ASSERT_TRUE(gFs.begin(TEST_DIR));
}

/** Called after every test. */
void tearDown(void)
{
    gFs.end();
    removeTestFiles();
}

/**
 * Write a text into a file.
 *
 * @param[in] path  Path of the file.
 * @param[in] text  Text which to write.
 */
static void writeFile(const char* path, const char* text)
{
    File file = gFs.open(path, FILE_WRITE);

    TEST_ASSERT_TRUE(file);
    TEST_ASSERT_EQUAL_UINT32(strlen(text), file.write(reinterpret_cast<const uint8_t*>(text), strlen(text)));
    file.close();
}

/**
 * Remove everything the tests may have created.
 */
static void removeTestFiles(void)
{
    FS rootFs;

    (void)rootFs.begin(".");
    (void)rootFs.remove("tmp_test_fs/a.txt");
    (void)rootFs.remove("tmp_test_fs/b.txt");
    (void)rootFs.remove("tmp_test_fs/sub/c.txt");
    (void)rootFs.rmdir("tmp_test_fs/sub");
    (void)rootFs.rmdir(TEST_DIR);
}

/**
 * Test writing, closing and reading a file.
 */
static void testWriteRead(void)
{
    TEST_ASSERT_FALSE(gFs.exists("/a.txt"));

    writeFile("/a.txt", "Hello");

    TEST_ASSERT_TRUE(gFs.exists("/a.txt"));
    TEST_ASSERT_TRUE(gFs.exists(String("/a.txt")));

    File file = gFs.open("/a.txt", FILE_READ);

    TEST_ASSERT_TRUE(file);
    TEST_ASSERT_FALSE(file.isDirectory());
    TEST_ASSERT_EQUAL_STRING("a.txt", file.name());
    TEST_ASSERT_EQUAL_STRING("/a.txt", file.path());
    TEST_ASSERT_EQUAL_UINT32(5U, file.size());
    TEST_ASSERT_EQUAL_INT(5, file.available());

    TEST_ASSERT_EQUAL_INT('H', file.read());
    TEST_ASSERT_EQUAL_INT(4, file.available());
    TEST_ASSERT_EQUAL_UINT32(1U, file.position());

    /* size() must not move the read position. */
    TEST_ASSERT_EQUAL_UINT32(5U, file.size());
    TEST_ASSERT_EQUAL_UINT32(1U, file.position());

    TEST_ASSERT_EQUAL_INT('e', file.peek());
    TEST_ASSERT_EQUAL_INT('e', file.read());

    TEST_ASSERT_TRUE(file.seek(4U));
    TEST_ASSERT_EQUAL_INT('o', file.read());
    TEST_ASSERT_EQUAL_INT(0, file.available());
    TEST_ASSERT_EQUAL_INT(-1, file.read());

    TEST_ASSERT_TRUE(file.seek(0U, fs::SeekSet));
    TEST_ASSERT_TRUE(file.seek(1U, fs::SeekCur));
    TEST_ASSERT_EQUAL_INT('e', file.read());
    TEST_ASSERT_TRUE(file.seek(0U, fs::SeekEnd));
    TEST_ASSERT_EQUAL_INT(0, file.available());

    char    buffer[6U] = {0};
    uint8_t bytes[5U]  = {0};

    TEST_ASSERT_TRUE(file.seek(0U));
    TEST_ASSERT_EQUAL_UINT32(5U, file.readBytes(buffer, 5U));
    TEST_ASSERT_EQUAL_STRING("Hello", buffer);

    TEST_ASSERT_TRUE(file.seek(1U));
    TEST_ASSERT_EQUAL_UINT32(4U, file.read(bytes, sizeof(bytes)));
    TEST_ASSERT_EQUAL_UINT8('e', bytes[0U]);

    file.close();
    TEST_ASSERT_FALSE(file);
}

/**
 * Test appending to a file.
 */
static void testAppend(void)
{
    writeFile("/a.txt", "He");

    File file = gFs.open("/a.txt", FILE_APPEND);

    TEST_ASSERT_TRUE(file);
    TEST_ASSERT_EQUAL_UINT32(1U, file.write('l'));
    file.close();

    file = gFs.open("/a.txt");
    TEST_ASSERT_EQUAL_UINT32(3U, file.size());
    file.close();
}

/**
 * Test that opening a file in text mode doesn't translate anything.
 */
static void testBinaryMode(void)
{
    const uint8_t data[] = {'a', '\r', '\n', 0x1AU, 'b'};
    File          file   = gFs.open("/a.txt", "w");

    TEST_ASSERT_TRUE(file);
    TEST_ASSERT_EQUAL_UINT32(sizeof(data), file.write(data, sizeof(data)));
    file.close();

    file = gFs.open("/a.txt", "r");
    TEST_ASSERT_TRUE(file);
    TEST_ASSERT_EQUAL_UINT32(sizeof(data), file.size());
    file.close();

    TEST_ASSERT_EQUAL_STRING("rb", fs::toBinaryMode("r").c_str());
    TEST_ASSERT_EQUAL_STRING("wb", fs::toBinaryMode("wb").c_str());
    TEST_ASSERT_EQUAL_STRING("r+b", fs::toBinaryMode("rt+").c_str());
    TEST_ASSERT_EQUAL_STRING("rb", fs::toBinaryMode(nullptr).c_str());
}

/**
 * Test renaming and removing a file.
 */
static void testRenameRemove(void)
{
    writeFile("/a.txt", "x");

    TEST_ASSERT_TRUE(gFs.rename("/a.txt", "/b.txt"));
    TEST_ASSERT_FALSE(gFs.exists("/a.txt"));
    TEST_ASSERT_TRUE(gFs.exists("/b.txt"));

    TEST_ASSERT_TRUE(gFs.remove("/b.txt"));
    TEST_ASSERT_FALSE(gFs.exists("/b.txt"));

    /* Removing or renaming something missing fails. */
    TEST_ASSERT_FALSE(gFs.remove("/b.txt"));
    TEST_ASSERT_FALSE(gFs.rename("/a.txt", "/b.txt"));
}

/**
 * Test creating a directory, iterating over it and removing it.
 */
static void testDirectory(void)
{
    TEST_ASSERT_TRUE(gFs.mkdir("/sub"));
    TEST_ASSERT_FALSE(gFs.mkdir("/sub"));

    /* exists() is true for a directory too, independent of the host. */
    TEST_ASSERT_TRUE(gFs.exists("/sub"));
    TEST_ASSERT_TRUE(gFs.exists(String("/sub")));

    writeFile("/sub/c.txt", "abc");

    File dir = gFs.open("/sub");

    TEST_ASSERT_TRUE(dir);
    TEST_ASSERT_TRUE(dir.isDirectory());
    TEST_ASSERT_EQUAL_STRING("sub", dir.name());

    File entry = dir.openNextFile();

    TEST_ASSERT_TRUE(entry);
    TEST_ASSERT_FALSE(entry.isDirectory());
    TEST_ASSERT_EQUAL_STRING("c.txt", entry.name());
    TEST_ASSERT_EQUAL_STRING("/sub/c.txt", entry.path());
    TEST_ASSERT_EQUAL_UINT32(3U, entry.size());
    entry.close();

    /* No further entry; "." and ".." are skipped. */
    entry = dir.openNextFile();
    TEST_ASSERT_FALSE(entry);

    dir.rewindDirectory();
    entry = dir.openNextFile();
    TEST_ASSERT_TRUE(entry);
    TEST_ASSERT_EQUAL_STRING("c.txt", entry.name());
    entry.close();
    dir.close();

    /* A non-empty directory can't be removed. */
    TEST_ASSERT_FALSE(gFs.rmdir("/sub"));
    TEST_ASSERT_TRUE(gFs.remove("/sub/c.txt"));
    TEST_ASSERT_TRUE(gFs.rmdir("/sub"));
    TEST_ASSERT_FALSE(gFs.exists("/sub"));
    TEST_ASSERT_FALSE(gFs.open("/sub"));
}

/**
 * Test the behavior with missing files and directories.
 */
static void testMissing(void)
{
    File file = gFs.open("/missing.txt");

    TEST_ASSERT_FALSE(file);
    TEST_ASSERT_EQUAL_INT(-1, file.read());
    TEST_ASSERT_EQUAL_UINT32(0U, file.size());
    TEST_ASSERT_EQUAL_INT(0, file.available());

    /* The filesystem can't be mounted on a missing directory. */
    FS other;

    TEST_ASSERT_FALSE(other.begin("tmp_test_fs_missing"));
    TEST_ASSERT_FALSE(other.begin(nullptr));

    /* A file is no directory either. */
    writeFile("/a.txt", "x");
    TEST_ASSERT_FALSE(other.begin("tmp_test_fs/a.txt"));
}

/**
 * Test the reported filesystem size.
 */
static void testSize(void)
{
    TEST_ASSERT_TRUE(0U < gFs.totalBytes());
    TEST_ASSERT_EQUAL_UINT32(0U, gFs.usedBytes());
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

    RUN_TEST(testWriteRead);
    RUN_TEST(testAppend);
    RUN_TEST(testBinaryMode);
    RUN_TEST(testRenameRemove);
    RUN_TEST(testDirectory);
    RUN_TEST(testMissing);
    RUN_TEST(testSize);

    return UNITY_END();
}
