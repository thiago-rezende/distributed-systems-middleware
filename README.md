# Distributed Systems Middleware
![build-status-badge] ![license-badge] ![cpp-badge] ![cmake-badge]
<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/thiago-rezende/distributed-systems-middleware">
    <img src="https://github.com/thiago-rezende/distributed-systems-middleware/raw/master/.github/logo.png" alt="Logo" width="100" height="100">
  </a>

  <h3 align="center">Distributed Systems Middleware</h3>
  <p align="center">
    Simple implementation of a middleware system
    <br />
    <a href="https://github.com/thiago-rezende/distributed-systems-middleware/issues">Report Bug</a>
    ·
    <a href="https://github.com/thiago-rezende/distributed-systems-middleware/issues">Request Feature</a>
    .
    <a href="https://github.com/thiago-rezende/distributed-systems-middleware/actions">Continuous Integration</a>
  </p>
</div>

 - Libraries Included
    - [Spdlog][spdlog-url]
    - [Catch2][catch2-url]
    - [WebSocket++][websocketpp-url]
    - [Asio][asio-url]
    - [CLI++][clipp-url]
    - [Json][json-url]

## About the Project
This project aims to research and study the concept of `middleware` in distributed systems, it is not 100% testing, and may contain several bugs and race conditions.

For security reasons this system will only work on your local machine, as it does not use any type of encryption in communications.
## Architecture
This system consists of three distinct parts, which are:

- ** Middleware **: Responsible for receiving and distributing messages
- ** Agent **: Responsible for reporting your status to middleware
- ** Client **: Responsible for controlling agents through messages

Each of these components is an executable file that will initialize a logger for displaying messages and a tty to receive user commands

Every time an agent changes state, it communicates with the middleware and the middleware distributes the state of this agent to the other clients

Every time an agent enters the network, the middleware notifies all customers

Whenever a customer changes an agent, the middleware distributes its status over the network.

## In Action
![Middleware in Action](https://github.com/thiago-rezende/distributed-systems-middleware/raw/master/middleware_in_action.gif)

## Requirements
  - CMake >= 3.16
  - Git
  - C++ 17 Compiler
## How To Build
> clone the repo [with submodules]
```sh
    git clone --recursive --depth 1 https://github.com/thiago-rezende/distributed-systems-middleware.git

    # In case of using the source from the zip
    cd distributed-systems-middleware
    git submodule update --init
```

> run cmake
```sh
    mkdir build
    cd build
    cmake ..
    cmake --build .
```

> run the components
```sh
    ./bin/middleware --help
```
```sh
    ./bin/agent --help
```
```sh
    ./bin/client --help
```

 ## Project Structure
 ```
    .
    ├── .github                     # Repository stuff.
    ├── middleware                  # Middleware folder.
    ├── middleware                  # Agent folder.
    ├── middleware                  # Client folder.
    ├── vendor                      # Third-party packages will be here.
    |    ├── spdlog                 # Fast C++ logging library.
    |    ├── catch2                 # A modern, C++-native, header-only, test framework.
    |    ├── websocketpp            # C++ websocket client/server library.
    |    ├── asio                   # Cross-platform C++ library for network and low-level I/O.
    |    ├── clipp                  # Command line interfaces for modern C++.
    |    └── json                   # JSON for Modern C++.
    └── CMakeLists.txt              # Main CMake build script.
```
 ## Compatibility

 | Toolset | Windows |  Linux  |   MacOS    |
 | :-----: | :-----: | :-----: | :--------: |
 |   GCC   |    ✔️    |    ✔️    |    ✔️    |
 |  Clang   |    ✔️    |    ✔️    |    ✔️    |
 |  MSVC   |    ✔️    | ------- | ---------- |

<!-- Links -->
[cmake-url]: https://cmake.org/
[spdlog-url]: https://github.com/gabime/spdlog
[catch2-url]: https://github.com/catchorg/Catch2
[websocketpp-url]: https://github.com/zaphoyd/websocketpp
[asio-url]: https://github.com/chriskohlhoff/asio
[clipp-url]: https://github.com/muellan/clipp
[json-url]: https://github.com/nlohmann/json


<!-- Badges -->
[build-status-badge]: https://github.com/thiago-rezende/distributed-systems-middleware/workflows/build/badge.svg
[license-badge]: https://img.shields.io/badge/license-MIT-blue.svg?style=flat-square
[cmake-badge]: https://img.shields.io/badge/CMake-3.16.0-blueviolet.svg?style=flat-square
[cpp-badge]: https://img.shields.io/badge/C++-17-orange.svg?style=flat-square
