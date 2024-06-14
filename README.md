# ArduinoNative <!-- omit in toc -->

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](http://choosealicense.com/licenses/mit/)
[![Repo Status](https://www.repostatus.org/badges/latest/wip.svg)](https://www.repostatus.org/#wip)
[![Release](https://img.shields.io/github/release/BlueAndi/ArduinoNative.svg)](https://github.com/BlueAndi/ArduinoNative/releases)

Projects have sometimes the need to run on different platforms. For example a robot software shall run in a simulation on a native environment and on a microcontroller. If the software on the microcontroller is Arduino based, the problem appears in the native environment, because there is no Arduino framework available.

This library provides for this use case some Arduino interfaces, but not all yet. If something is missing, feel free to contribute, which make our all lifes easier. :-)

## Table of content

* [Architecture](#architecture)
* [How to integrate the library?](#how-to-integrate-the-library)
  * [Example](#example)
* [Used Libraries](#used-libraries)
* [Issues, Ideas And Bugs](#issues-ideas-and-bugs)
* [License](#license)
* [Contribution](#contribution)

# Architecture

## The Principle

![Principle](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com//BlueAndi/ArduinoNative/master/doc/uml/Principle.plantuml)

## Detail

![ArduinoNative](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/BlueAndi/ArduinoNative/master/doc/uml/ArduinoNative.plantuml)

![DynamicFlow](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/BlueAndi/ArduinoNative/master/doc/uml/DynamicFlow.plantuml)

# How to integrate the library?
1. Add it to the _platformio.ini_ in your environment to the _lib\_deps_ section:
    ```
    lib_deps =
        BlueAndi/ArduinoNative @ ~0.1.0
    ```
3. TODO

## Example
See [minimal example](./examples/example/).

# Used Libraries

| Library                                                            | Description                                                      | License    |
| ------------------------------------------------------------------ | ---------------------------------------------------------------- | ---------- |
| - | - | - |

# Issues, Ideas And Bugs
If you have further ideas or you found some bugs, great! Create a [issue](https://github.com/BlueAndi/ArduinoNative/issues) or if you are able and willing to fix it by yourself, clone the repository and create a pull request.

# License
The whole source code is published under the [MIT license](http://choosealicense.com/licenses/mit/).
Consider the different licenses of the used third party libraries too!

# Contribution
Unless you explicitly state otherwise, any contribution intentionally submitted for inclusion in the work by you, shall be licensed as above, without any
additional terms or conditions.
