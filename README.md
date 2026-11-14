# ArduinoNative <!-- omit in toc -->

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](http://choosealicense.com/licenses/mit/)
[![Repo Status](https://www.repostatus.org/badges/latest/wip.svg)](https://www.repostatus.org/#wip)
[![Release](https://img.shields.io/github/release/BlueAndi/ArduinoNative.svg)](https://github.com/BlueAndi/ArduinoNative/releases)

Projects have sometimes the need to run on different platforms. For example a robot software shall run in a simulation on a native environment and on a microcontroller. If the software on the microcontroller is Arduino based, the problem appears in the native environment, because there is no Arduino framework available.

This library provides for this use case some Arduino interfaces, but not all yet. If something is missing, feel free to contribute, which make our all lifes easier. :-)

## Table of content <!-- omit in toc -->

- [Architecture](#architecture)
  - [The Principle](#the-principle)
  - [Detail](#detail)
- [Supported Interfaces](#supported-interfaces)
- [How to integrate the library?](#how-to-integrate-the-library)
  - [Entry point](#entry-point)
  - [Command line option](#command-line-option)
  - [Serial interface](#serial-interface)
  - [Filesystem and preferences](#filesystem-and-preferences)
  - [HostCompat](#hostcompat)
  - [Example](#example)
- [Migration from 0.2.x to 0.3.0](#migration-from-02x-to-030)
- [Used Libraries](#used-libraries)
- [Issues, Ideas And Bugs](#issues-ideas-and-bugs)
- [License](#license)
- [Contribution](#contribution)

## Architecture

### The Principle

![Principle](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com//BlueAndi/ArduinoNative/master/doc/uml/Principle.plantuml)

### Detail

![ArduinoNative](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/BlueAndi/ArduinoNative/master/doc/uml/ArduinoNative.plantuml)

![DynamicFlow](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/BlueAndi/ArduinoNative/master/doc/uml/DynamicFlow.plantuml)

## Supported Interfaces

Not every Arduino interface is available, just the following ones:

| Header                | Description                                                                                          |
| --------------------- | ---------------------------------------------------------------------------------------------------- |
| _Arduino.h_           | Time (`millis()`, `micros()`, `delay()`, `yield()`), digital/analog pins, random, PROGMEM helpers, `constrain()`, `PI`. |
| _WString.h_           | `String`, based on `std::string`.                                                                    |
| _Print.h_             | `Print` base class with the `print()`/`println()`/`write()` family.                                  |
| _Stream.h_            | `Stream` base class for readable and writable streams.                                               |
| _HardwareSerial.h_    | `HardwareSerial` and the global `Serial`, connected to the standard output and input. `HardwareSerial::setTxTimeoutMs()` is an ESP32 extension. |
| _IPAddress.h_         | `IPAddress`.                                                                                         |
| _Client.h_            | `Client`, the abstraction of a network connection to a remote peer.                                  |
| _cbuf.h_              | Circular buffer.                                                                                     |
| _MD5Builder.h_        | `MD5Builder` to calculate MD5 hashes.                                                                |
| _libb64/cencode.h_    | Base64 encoder.                                                                                      |
| _FS.h_                | Filesystem (`FS`, `File`) and the instance `NativeFS`, which maps the filesystem to a directory of the host. |
| _Preferences.h_       | `Preferences`, key/value storage. ESP32 extension.                                                   |
| _HostCompat.h_        | C library functions which Windows doesn't provide, e.g. `strptime()`.                                |

## How to integrate the library?

1. Add it to the _platformio.ini_ in your environment to the _lib\_deps_ section:

    ```text
    lib_deps =
        BlueAndi/ArduinoNative @ ~0.3.0
    ```

2. Add this build flags to your _platformio.ini_ in your environment:

    ```text
    build_flags =
        -D _USE_MATH_DEFINES
    ```

    The define is required, otherwise _Arduino.h_ raises an error.

3. Implement the usual `setup()` and `loop()` functions of your application. There is no need for a `main()`.

### Entry point

The library provides the `main()`. It calls `setup()` once and `loop()` in an infinite loop, like the Arduino framework does.

If your application brings its own `main()`, it is used instead of the one of the library. In that case it calls `Arduino::setup()` once and `Arduino::loop()` periodically. `Arduino::setup()` calls the `setup()` of the application and takes the time base of `millis()`, `micros()` and `delay()`, for example to run in simulation time:

```cpp
#include <Arduino.h>

static unsigned long getTick()
{
    return simulation.getTimeMs();
}

static void delayMs(unsigned long ms)
{
    simulation.step(ms);
}

int main(int argc, char** argv)
{
    Arduino::setup(getTick, delayMs);

    for (;;)
    {
        Arduino::loop();
    }

    return 0;
}
```

A `nullptr` selects the default for the respective function, which is based on `std::chrono::steady_clock`. `micros()` returns `millis() * 1000`, if a tick function is given.

### Command line option

The `main()` of the library accepts the option `--work-dir <directory>`. It changes the working directory of the process before `setup()` is called. If the directory doesn't exist, the program prints an error and exits with exit code 1. Without the option the current directory is used.

```text
program --work-dir ./run
```

### Serial interface

`Serial` is a `HardwareSerial`. It writes to the standard output and reads from the standard input.

* The input is only served, if the standard input is connected to a terminal, which can be used interactively. In this case `Serial.begin()` switches the terminal to the raw mode (no echo, no line editing), like the serial interface of a microcontroller delivers the characters. `Serial.end()` restores the mode. If the standard input is redirected, e.g. in a CI, `Serial.available()` is always 0.
* Define the compile switch `CONFIG_NATIVE_SERIAL_INPUT` as `0` to disable the terminal input and the raw mode, e.g. if it disturbs a debugger or the terminal of an IDE:

    ```text
    build_flags =
        -D CONFIG_NATIVE_SERIAL_INPUT=0
    ```

* `Serial.setStream(&stream)` redirects the serial interface to any other `Stream`, which is not owned by `Serial`. `Serial.setStream(nullptr)` selects the terminal again.

### Filesystem and preferences

* `NativeFS` maps the paths of the target (e.g. `/configuration/1.json`) to a root directory of the host. `NativeFS.begin()` uses the current working directory, `NativeFS.begin("path")` an existing directory of your choice. `begin()` returns false if the directory doesn't exist.
* `Preferences` keeps the key/value pairs in the file _preferences.txt_ in the working directory. Every modification is written through, so the values survive a restart.

Use `--work-dir` to keep both files together in one directory.

### HostCompat

_HostCompat.h_ provides the POSIX function `strptime()` on Windows. On Linux it does nothing. To make it available for the application and all libraries without changing their sources, force include it in the _platformio.ini_:

```text
build_flags =
    -D _USE_MATH_DEFINES
    -include HostCompat.h
```

### Example

See [minimal example](./examples/example/).

## Migration from 0.2.x to 0.3.0

* `Serial_` and `Terminal` are gone. `Serial` is now a `HardwareSerial` (see _HardwareSerial.h_).
* `Print` follows the Arduino model. A class derived from `Print` implements `write(uint8_t)` and no longer overrides `print(...)` and `println(...)`, which are provided by `Print`.
* A class derived from `Stream` implements `available()`, which is no longer `const`, `read()` and `peek()`. `readBytes()` is no longer virtual, it is built on `read()`.
* `Serial.setStream()` takes a pointer instead of a reference.
* The library _MainNative_ of the example is not needed anymore, the library provides the `main()`.
* `String::length()` returns `unsigned int`.

## Used Libraries

| Library                                                            | Description                                                      | License    |
| ------------------------------------------------------------------ | ---------------------------------------------------------------- | ---------- |
| - | - | - |

## Issues, Ideas And Bugs

If you have further ideas or you found some bugs, great! Create a [issue](https://github.com/BlueAndi/ArduinoNative/issues) or if you are able and willing to fix it by yourself, clone the repository and create a pull request.

## License

The whole source code is published under the [MIT license](http://choosealicense.com/licenses/mit/).
Consider the different licenses of the used third party libraries too!

## Contribution

Unless you explicitly state otherwise, any contribution intentionally submitted for inclusion in the work by you, shall be licensed as above, without any
additional terms or conditions.
