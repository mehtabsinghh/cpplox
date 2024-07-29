# CPPLox

## Overview

Creating an interpreter using C++ for a language called Lox

## Build Instructions

To build the OrderBook project, follow these steps:

1. **Clone the repository:**

   ```bash
   git clone <https://github.com/mehtabsinghh/cpplox>
   cd OrderBook
   ```
2. **Create a build directory:**
   ```bash
   mkdir build
   cd build
   ```
3. **Generate build files with CMake:**
   ```bash
   cmake ..
   ```

4. **Build the project:**
   ```bash
   cmake --build . --config release
   ```

After building the project, you can run the executable with a filename or without which puts you into a repl:
   ```bash
   ./cpplox
   ```
