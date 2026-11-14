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
 * @file   Print.cpp
 * @brief  Print
 * @author Andreas Merkle <web@blue-andi.de>
 */

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "Print.h"

#include <cmath>
#include <string.h>

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

static int normalizeBase(int base);

/******************************************************************************
 * Local Variables
 *****************************************************************************/

/******************************************************************************
 * Public Methods
 *****************************************************************************/

size_t Print::write(const uint8_t* buffer, size_t size)
{
    size_t n = 0;
    while (size--)
    {
        n += write(*buffer++);
    }
    return n;
}

size_t Print::write(const char* str)
{
    if (str == nullptr)
    {
        return 0;
    }

    return write(reinterpret_cast<const uint8_t*>(str), strlen(str));
}

size_t Print::write(const char* buffer, size_t size)
{
    return write(reinterpret_cast<const uint8_t*>(buffer), size);
}

size_t Print::print(const String& s)
{
    return write(s.c_str(), s.length());
}

size_t Print::print(const char str[])
{
    return write(str);
}

size_t Print::print(char c)
{
    return write(c);
}

size_t Print::print(double n, int digits)
{
    size_t written = 0U;

    if (true == std::isnan(n))
    {
        written = print("nan");
    }
    else if (true == std::isinf(n))
    {
        written = print((0.0 > n) ? "-inf" : "inf");
    }
    else
    {
        written = print(String(n, static_cast<unsigned int>((0 < digits) ? digits : 0)));
    }

    return written;
}

size_t Print::println()
{
    return print("\r\n");
}

size_t Print::println(const String& s)
{
    size_t n  = print(s);
    n        += println();
    return n;
}

/******************************************************************************
 * Protected Methods
 *****************************************************************************/

/******************************************************************************
 * Private Methods
 *****************************************************************************/

size_t Print::printUnsigned(unsigned long long n, int base)
{
    String number(n, static_cast<unsigned char>(normalizeBase(base)));

    number.toUpperCase();

    return print(number);
}

size_t Print::printSigned(long long n, unsigned long long bitPattern, int base)
{
    size_t written = 0U;

    if (DEC == normalizeBase(base))
    {
        written = print(String(n, static_cast<unsigned char>(DEC)));
    }
    else
    {
        written = printUnsigned(bitPattern, base);
    }

    return written;
}

/******************************************************************************
 * External Functions
 *****************************************************************************/

/******************************************************************************
 * Local Functions
 *****************************************************************************/

/**
 * Get the number base which is used for printing.
 *
 * @param[in] base  Requested number base
 *
 * @return The requested base, if it is supported (2 to 16), otherwise decimal.
 */
static int normalizeBase(int base)
{
    return ((BIN <= base) && (HEX >= base)) ? base : DEC;
}
