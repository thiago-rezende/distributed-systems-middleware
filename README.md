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
 > TODO
 ## Architecture
 > TODO
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
