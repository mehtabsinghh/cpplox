cmake_minimum_required(VERSION 3.12)
project(cpplox)

set(CMAKE_CXX_STANDARD 17)

# Add your source files here
set(SOURCES
    main.cpp
    # Add more source files here if needed
)

add_executable(cpplox ${SOURCES})