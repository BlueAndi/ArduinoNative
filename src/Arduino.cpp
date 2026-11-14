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
 * @file   Arduino.cpp
 * @brief  Arduino native
 * @author Andreas Merkle <web@blue-andi.de>
 */

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "Arduino.h"

#include <chrono>
#include <thread>

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

/** Application setup, provided by the application. */
extern void setup();
/** Application loop, provided by the application. */
extern void loop();

static std::chrono::steady_clock::time_point getTimerStartTime();

/******************************************************************************
 * Local Variables
 *****************************************************************************/

/** Function pointer to get the system tick in ms. */
static GetSystemTick gGetSystemTickFunc = nullptr;

/** Function pointer to delay for some time in ms. */
static SystemDelay gSystemDelayFunc = nullptr;

/** Max. number of supported pins. */
static const uint8_t MAX_PINS = 64U;

/** Mode of every pin. There is no GPIO on the host, its just remembered. */
static uint8_t gPinModes[MAX_PINS] = {0U};

/** Level of every pin. There is no GPIO on the host, its just remembered. */
static uint8_t gPinLevels[MAX_PINS] = {0U};

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

void Arduino::setup(GetSystemTick getSystemTickFunc, SystemDelay systemDelayFunc)
{
    gGetSystemTickFunc = getSystemTickFunc;
    gSystemDelayFunc   = systemDelayFunc;

    ::setup();
}

void Arduino::loop()
{
    ::loop();
}

extern unsigned long millis()
{
    unsigned long timestamp = 0UL;

    if (nullptr != gGetSystemTickFunc)
    {
        timestamp = gGetSystemTickFunc();
    }
    else
    {
        using namespace std::chrono;

        timestamp =
            static_cast<unsigned long>(duration_cast<milliseconds>(steady_clock::now() - getTimerStartTime()).count());
    }

    return timestamp;
}

extern unsigned long micros()
{
    unsigned long timestamp = 0UL;

    if (nullptr != gGetSystemTickFunc)
    {
        timestamp = millis() * 1000UL;
    }
    else
    {
        using namespace std::chrono;

        timestamp =
            static_cast<unsigned long>(duration_cast<microseconds>(steady_clock::now() - getTimerStartTime()).count());
    }

    return timestamp;
}

extern void delay(unsigned long ms)
{
    if (nullptr != gSystemDelayFunc)
    {
        gSystemDelayFunc(ms);
    }
    else
    {
        /* sleep_for() may return a little early on some hosts, therefore wait until
         * the deadline of the steady clock is really reached.
         */
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(ms);

        while (std::chrono::steady_clock::now() < deadline)
        {
            std::this_thread::sleep_for(deadline - std::chrono::steady_clock::now());
        }
    }
}

extern void yield()
{
    std::this_thread::yield();
}

extern void pinMode(uint8_t pinNo, uint8_t mode)
{
    if (MAX_PINS > pinNo)
    {
        gPinModes[pinNo] = mode;
    }
}

extern void digitalWrite(uint8_t pinNo, uint8_t level)
{
    if (MAX_PINS > pinNo)
    {
        gPinLevels[pinNo] = level;
    }
}

extern int digitalRead(uint8_t pinNo)
{
    int level = LOW;

    if (MAX_PINS > pinNo)
    {
        level = gPinLevels[pinNo];
    }

    return level;
}

extern uint16_t analogRead(uint8_t pinNo)
{
    /* There is no ADC on the host. */
    (void)pinNo;

    return 0U;
}

extern long random(long max)
{
    return random(0, max);
}

extern long random(long min, long max)
{
    long result = min;

    if (min < max)
    {
        result = min + (rand() % (max - min));
    }

    return result;
}

extern void randomSeed(unsigned long seed)
{
    srand(static_cast<unsigned int>(seed));
}

/******************************************************************************
 * Local Functions
 *****************************************************************************/

/**
 * Get the start time for the timing functions.
 *
 * The time is taken at the first call. It is a function local static on
 * purpose: millis() may be called during the static initialization of another
 * translation unit, which is not ordered against a namespace scope variable of
 * this file.
 *
 * @return Start time
 */
static std::chrono::steady_clock::time_point getTimerStartTime()
{
    static const std::chrono::steady_clock::time_point START_TIME = std::chrono::steady_clock::now();

    return START_TIME;
}
