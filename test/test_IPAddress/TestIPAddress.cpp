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
 * @brief  Test the IPAddress class and the Client interface.
 * @author Andreas Merkle <web@blue-andi.de>
 */

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <unity.h>
#include <Arduino.h>
#include <Client.h>

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
 * Minimal client, just to proof that the interface is implementable.
 */
class MinimalClient : public Client
{
public:
    MinimalClient() : Client(), m_lastPort(0U), m_isConnected(false)
    {
    }

    ~MinimalClient() final
    {
    }

    int connect(IPAddress ip, uint16_t port) final
    {
        m_lastIp      = ip;
        m_lastPort    = port;
        m_isConnected = true;
        return 1;
    }

    int connect(const char* host, uint16_t port) final
    {
        (void)host;
        m_lastPort    = port;
        m_isConnected = true;
        return 1;
    }

    size_t write(uint8_t data) final
    {
        (void)data;
        return 1U;
    }

    size_t write(const uint8_t* buffer, size_t size) final
    {
        (void)buffer;
        return size;
    }

    int available() final
    {
        return 0;
    }

    int read() final
    {
        return -1;
    }

    int read(uint8_t* buffer, size_t size) final
    {
        (void)buffer;
        (void)size;
        return -1;
    }

    int peek() final
    {
        return -1;
    }

    void flush() final
    {
    }

    void stop() final
    {
        m_isConnected = false;
    }

    uint8_t connected() final
    {
        return m_isConnected ? 1U : 0U;
    }

    operator bool() final
    {
        return m_isConnected;
    }

    IPAddress m_lastIp;   /**< Address of the last connect. */
    uint16_t  m_lastPort; /**< Port of the last connect. */

private:
    bool m_isConnected; /**< Is connected? */
};

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
 * Test the default address.
 */
static void testDefault(void)
{
    IPAddress ip;

    TEST_ASSERT_EQUAL_STRING("0.0.0.0", ip.toString().c_str());
    TEST_ASSERT_EQUAL_UINT32(0U, static_cast<uint32_t>(ip));
}

/**
 * Test the construction by octets and the string conversion.
 */
static void testOctets(void)
{
    IPAddress ip(192U, 168U, 1U, 2U);

    TEST_ASSERT_EQUAL_STRING("192.168.1.2", ip.toString().c_str());
    TEST_ASSERT_EQUAL_UINT8(192U, ip[0]);
    TEST_ASSERT_EQUAL_UINT8(168U, ip[1]);
    TEST_ASSERT_EQUAL_UINT8(1U, ip[2]);
    TEST_ASSERT_EQUAL_UINT8(2U, ip[3]);

    /* Out of range index. */
    TEST_ASSERT_EQUAL_UINT8(0U, ip[4]);
    TEST_ASSERT_EQUAL_UINT8(0U, ip[-1]);
}

/**
 * Test the conversion from and to a 32-bit value.
 */
static void testUint32(void)
{
    IPAddress ip(0xC0A80102UL);
    IPAddress assigned;

    TEST_ASSERT_EQUAL_STRING("192.168.1.2", ip.toString().c_str());
    TEST_ASSERT_EQUAL_UINT32(0xC0A80102UL, static_cast<uint32_t>(ip));

    assigned = 0x0A000001UL;
    TEST_ASSERT_EQUAL_STRING("10.0.0.1", assigned.toString().c_str());
}

/**
 * Test comparison and copy.
 */
static void testCompareAndCopy(void)
{
    IPAddress a(10U, 0U, 0U, 1U);
    IPAddress b(10U, 0U, 0U, 1U);
    IPAddress c(10U, 0U, 0U, 2U);
    IPAddress copy(c);
    IPAddress assigned;

    TEST_ASSERT_TRUE(a == b);
    TEST_ASSERT_FALSE(a != b);
    TEST_ASSERT_TRUE(a != c);
    TEST_ASSERT_FALSE(a == c);
    TEST_ASSERT_TRUE(copy == c);

    assigned = a;
    TEST_ASSERT_TRUE(assigned == a);
    TEST_ASSERT_EQUAL_STRING("10.0.0.1", assigned.toString().c_str());
}

/**
 * Test that a class derived from Client can be used.
 */
static void testClient(void)
{
    MinimalClient minimal;
    Client&       client = minimal;

    TEST_ASSERT_EQUAL_UINT8(0U, client.connected());
    TEST_ASSERT_EQUAL_INT(1, client.connect(IPAddress(192U, 168U, 1U, 2U), 1883U));
    TEST_ASSERT_EQUAL_UINT8(1U, client.connected());
    TEST_ASSERT_TRUE(static_cast<bool>(client));
    TEST_ASSERT_EQUAL_STRING("192.168.1.2", minimal.m_lastIp.toString().c_str());
    TEST_ASSERT_EQUAL_UINT16(1883U, minimal.m_lastPort);

    /* The string overload of Print::write() must stay reachable through the Client. */
    TEST_ASSERT_EQUAL_UINT(3U, client.write("abc"));

    client.stop();
    TEST_ASSERT_EQUAL_UINT8(0U, client.connected());
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

    RUN_TEST(testDefault);
    RUN_TEST(testOctets);
    RUN_TEST(testUint32);
    RUN_TEST(testCompareAndCopy);
    RUN_TEST(testClient);

    return UNITY_END();
}
