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
 * @brief  Test the serial interface.
 * @author Andreas Merkle <web@blue-andi.de>
 */

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <unity.h>
#include <HardwareSerial.h>

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
 * Stream, which delivers everything again what was written to it.
 */
class LoopbackStream : public Stream
{
public:

    LoopbackStream() :
        Stream(),
        m_buffer()
    {
    }

    ~LoopbackStream() final
    {
    }

    size_t write(uint8_t data) final
    {
        m_buffer += static_cast<char>(data);
        return 1U;
    }

    using Print::write;

    int available() final
    {
        return static_cast<int>(m_buffer.length());
    }

    int read() final
    {
        int data = -1;

        if (0U < m_buffer.length())
        {
            data     = static_cast<unsigned char>(m_buffer[0]);
            m_buffer = m_buffer.substring(1U);
        }

        return data;
    }

    int peek() final
    {
        return (0U < m_buffer.length()) ? static_cast<unsigned char>(m_buffer[0]) : -1;
    }

private:

    String m_buffer; /**< Looped data. */
};

/******************************************************************************
 * Prototypes
 *****************************************************************************/

static void testNoRedirect(void);
static void testRedirect(void);

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

    RUN_TEST(testNoRedirect);
    RUN_TEST(testRedirect);

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
    /* Never leave a redirect behind, independent of the test order. */
    Serial.setStream(nullptr);
}

/******************************************************************************
 * Local Functions
 *****************************************************************************/

/**
 * Without begin() the terminal input is never served.
 */
static void testNoRedirect(void)
{
    TEST_ASSERT_EQUAL_INT(0, Serial.available());
    TEST_ASSERT_EQUAL_INT(-1, Serial.read());
    TEST_ASSERT_EQUAL_INT(-1, Serial.peek());
}

/**
 * Test redirecting the serial interface to a stream and back.
 */
static void testRedirect(void)
{
    LoopbackStream loopback;

    Serial.setStream(&loopback);

    TEST_ASSERT_EQUAL_UINT(1U, Serial.print("x"));
    TEST_ASSERT_EQUAL_INT(1, Serial.available());
    TEST_ASSERT_EQUAL_INT('x', Serial.peek());
    TEST_ASSERT_EQUAL_INT('x', Serial.read());
    TEST_ASSERT_EQUAL_INT(-1, Serial.read());

    TEST_ASSERT_EQUAL_UINT(2U, Serial.write("yz"));
    TEST_ASSERT_EQUAL_INT('y', Serial.read());
    TEST_ASSERT_EQUAL_INT('z', Serial.read());

    Serial.setStream(nullptr);

    TEST_ASSERT_EQUAL_INT(0, Serial.available());
    TEST_ASSERT_EQUAL_INT(-1, Serial.read());
}
