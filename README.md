# Signal - Library for C++ signal-slot system with customizable result combiners
[![License: MIT](https://img.shields.io/badge/License-MIT-lightgrey.svg)](https://opensource.org/licenses/MIT)

Signal is a lightweight C++ library that implements a signal-slot mechanism with customizable result combiners. It allows connecting slots (callback functions) to signals, emitting those signals, and combining their results using different combiner strategies.

## Features
- Signal-slot system for connecting callback functions (slots) to signals
- Customizable result combiners:
    - `DiscardCombiner`: Ignores the results emitted by slots
    - `LastCombiner`: Keeps only the last emitted result
    - `VectorCombiner`: Collects all emitted results in a vector
- Support for signals with `void` return types
- Built-in test suite using GoogleTest

## Requirements
- C++17 compatible compiler
- CMake 3.10 or higher
- GoogleTest (included)

## Build Instructions
1. Clone the repository
    ```bash
    git clone https://github.com/yourusername/signal-library.git
    ```
2. Create a build directory
    ```bash
    mkdir build && cd build
    ```
3. Configure and build
    ```bash
    cmake ..
    make
    ```

## Run tests
The Signal library includes a test suite to ensure proper functioning. To run the tests:
```bash
./testSignal
```

When all tests pass, you should see output similar to:
```bash
[----------] Global test environment tear-down
[==========] 49 tests from 8 test suites ran. (2 ms total)
[  PASSED  ] 49 tests.
```

