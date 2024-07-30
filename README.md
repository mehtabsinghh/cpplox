# CPPLox

## Overview

Creating an interpreter using C++ for a language called Lox

## Build Instructions

To build the OrderBook project, follow these steps:

1. **Clone the repository:**

   ```bash
   git clone https://github.com/mehtabsinghh/cpplox
   cd cpplox
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

After building the project, you can run the executable alone to use the repl or add a filepath:
   ```bash
   ./cpplox filepath
   ```
