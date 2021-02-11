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

 ## Project Structure
 ```
    .
    ├── .github                     # Repository stuff.
    ├── middleware                  # Middleware folder.
    |    ├── assets                 # Assets folder.
    |    ├── core                   # Core functionalities folder.
    |    |    ├── logger.h          # Logger utils.
    |    |    └── logger.cpp        # Logger utils implementation.
    |    ├── debug                  # Debug functionalities folder.
    |    |    ├── assert.h          # Assertion utils.
    |    |    └── instrumentor.h    # Profiling utils.
    |    ├── pch.hpp                # Pre-compiled header.
    |    ├── config.h               # Populated at config time.
    |    ├── main.cpp               # Middleware entrypoint.
    |    └── CMakeLists.txt         # Middleware CMake build script.
    ├── tests                       # Tests folder.
    |    ├── middleware             # Middleware tests.
    |    |    ├── CMakeLists.txt    # Middleware tests CMake script.
    |    |    └── never_fails.cpp   # Middleware test file.
    |    └── CMakeLists.txt         # Main CMake tests script.
    ├── vendor                      # Third-party packages will be here.
    |    ├── spdlog                 # Fast C++ logging library.
    |    └── catch2                 # A modern, C++-native, header-only, test framework.
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

<!-- Badges -->
[build-status-badge]: https://github.com/thiago-rezende/distributed-systems-middleware/workflows/build/badge.svg
[license-badge]: https://img.shields.io/badge/license-MIT-blue.svg?style=flat-square
[cmake-badge]: https://img.shields.io/badge/CMake-3.16.0-blueviolet.svg?style=flat-square
[cpp-badge]: https://img.shields.io/badge/C++-17-orange.svg?style=flat-square