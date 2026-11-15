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
 * @file   Print.h
 * @brief  Print
 * @author Andreas Merkle <web@blue-andi.de>
 *
 * @addtogroup Arduino
 *
 * @{
 */

#ifndef PRINT_H
#define PRINT_H

/******************************************************************************
 * Compile Switches
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <stdint.h>

#include "WString.h"

/******************************************************************************
 * Macros
 *****************************************************************************/

/** Decimal number base */
#define DEC (10)

/** Hexadecimal number base */
#define HEX (16)

/** Octal number base */
#define OCT (8)

/** Binary number base */
#define BIN (2)

/******************************************************************************
 * Types and Classes
 *****************************************************************************/

/**
 * Print class, the counterpart of the Arduino Print.
 *
 * A derived class implements write(uint8_t), everything else is built on it.
 */
class Print
{
public:
    /**
     * Constructs a print object.
     */
    Print()
    {
    }

    /**
     * Destroys a print object.
     */
    virtual ~Print()
    {
    }

    /**
     * Write a single byte to the output stream.
     *
     * @param[in] data  Data byte
     * @return Number of written data bytes.
     */
    virtual size_t write(uint8_t data) = 0;

    /**
     * Write several data bytes to the output stream.
     *
     * @param[in] buffer    Data buffer
     * @param[in] size      Data buffer size
     * @return Number of written data bytes.
     */
    virtual size_t write(const uint8_t* buffer, size_t size);

    /**
     * Wait until everything is written out.
     *
     * Like the Arduino counterpart it does nothing by default, a derived class
     * may override it.
     */
    virtual void flush()
    {
        /* Nothing to do. */
    }

    /**
     * Write a zero terminated string to the output stream.
     *
     * @param[in] str   String
     * @return Number of written characters.
     */
    size_t write(const char* str);

    /**
     * Write several string characters to the output stream.
     *
     * @param[in] buffer    Character buffer
     * @param[in] size      Character buffer size
     * @return Number of written characters.
     */
    size_t write(const char* buffer, size_t size);

    /**
     * Print a string.
     *
     * @param[in] s String
     * @return Number of written characters.
     */
    size_t print(const String& s);

    /**
     * Print a string.
     *
     * @param[in] str String
     * @return Number of written characters.
     */
    size_t print(const char str[]);

    /**
     * Print single character.
     *
     * @param[in] c Character
     * @return Number of written characters.
     */
    size_t print(char c);

    /* Every integral type needs its own overload, otherwise e.g. an int
     * argument is ambiguous, because it can be converted to several types. The
     * 64-bit types are long or long long, depending on the host, therefore both
     * are provided.
     *
     * The number base is 2 to 16. Every other base, including 0 and 1, selects
     * decimal. A negative number is printed with its sign in decimal, in every
     * other base as two's complement of the width of its own type, e.g.
     * print(-1, HEX) prints FFFFFFFF.
     */

    /**
     * Print a number.
     *
     * @param[in] n     Number to print.
     * @param[in] base  Number base.
     *
     * @return Number of written characters.
     */
    size_t print(unsigned char n, int base = DEC)
    {
        return printUnsigned(n, base);
    }

    /**
     * Print a number.
     *
     * @param[in] n     Number to print.
     * @param[in] base  Number base.
     *
     * @return Number of written characters.
     */
    size_t print(int n, int base = DEC)
    {
        return printSigned(n, static_cast<unsigned int>(n), base);
    }

    /**
     * Print a number.
     *
     * @param[in] n     Number to print.
     * @param[in] base  Number base.
     *
     * @return Number of written characters.
     */
    size_t print(unsigned int n, int base = DEC)
    {
        return printUnsigned(n, base);
    }

    /**
     * Print a number.
     *
     * @param[in] n     Number to print.
     * @param[in] base  Number base.
     *
     * @return Number of written characters.
     */
    size_t print(long n, int base = DEC)
    {
        return printSigned(n, static_cast<unsigned long>(n), base);
    }

    /**
     * Print a number.
     *
     * @param[in] n     Number to print.
     * @param[in] base  Number base.
     *
     * @return Number of written characters.
     */
    size_t print(unsigned long n, int base = DEC)
    {
        return printUnsigned(n, base);
    }

    /**
     * Print a number.
     *
     * @param[in] n     Number to print.
     * @param[in] base  Number base.
     *
     * @return Number of written characters.
     */
    size_t print(long long n, int base = DEC)
    {
        return printSigned(n, static_cast<unsigned long long>(n), base);
    }

    /**
     * Print a number.
     *
     * @param[in] n     Number to print.
     * @param[in] base  Number base.
     *
     * @return Number of written characters.
     */
    size_t print(unsigned long long n, int base = DEC)
    {
        return printUnsigned(n, base);
    }

    /**
     * Print a floating point number, rounded to the given number of decimal
     * places. Not a number is printed as "nan", infinity as "inf" or "-inf".
     *
     * @param[in] n         Number to print.
     * @param[in] digits    Number of decimal places. A negative value is handled as 0.
     *
     * @return Number of written characters.
     */
    size_t print(double n, int digits = 2);

    /**
     * Print a carriage return and new line.
     *
     * @return Number of written characters.
     */
    size_t println(void);

    /**
     * Print a string with a carriage return and new line at the end.
     *
     * @param[in] s String
     * @return Number of written characters.
     */
    size_t println(const String& s);

private:
    /**
     * Print an unsigned number.
     *
     * @param[in] n     Number to print
     * @param[in] base  Number base
     *
     * @return Number of written characters.
     */
    size_t printUnsigned(unsigned long long n, int base);

    /**
     * Print a signed number.
     *
     * @param[in] n         Number to print
     * @param[in] bitPattern    The number converted to the unsigned type of the same
     *                          width, which is printed in a base other than decimal.
     * @param[in] base      Number base
     *
     * @return Number of written characters.
     */
    size_t printSigned(long long n, unsigned long long bitPattern, int base);
};

/******************************************************************************
 * Functions
 *****************************************************************************/

#endif /* PRINT_H */

/** @} */
