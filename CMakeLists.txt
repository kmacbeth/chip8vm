cmake_minimum_required(VERSION 3.8)
project(chip8 LANGUAGES CXX)

# For clang tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Create executable target
add_executable(chip8
    src/main.cpp
    src/memory.hpp
    src/memory.cpp
    src/cpu.hpp
    src/cpu.cpp
    src/opcode.hpp
    src/gpu.hpp
    src/gpu.cpp
    src/debugger.hpp
    src/debugger.cpp
)

target_include_directories(chip8
    PRIVATE
        ./src
)

add_subdirectory(test)

# Add custom target to copy compile_commands.json at project root
add_custom_target(chip8_db
    ALL
    COMMAND
        ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/compile_commands.json
                                              ${CMAKE_SOURCE_DIR}/compile_commands.json
    DEPENDS
        ${CMAKE_BINARY_DIR}/compile_commands.json
)

