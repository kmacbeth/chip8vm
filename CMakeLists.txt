cmake_minimum_required(VERSION 3.8)
project(cmake_exec LANGUAGES CXX)

# For clang tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Create executable target
add_executable(cmake_exec
    src/main.cpp
    src/cpu.hpp
    src/cpu.cpp
)

target_include_directories(cmake_exec
    PRIVATE
        ./src
)

# Add custom target to copy compile_commands.json at project root
add_custom_target(cmake_exec_db
    ALL
    COMMAND
        ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/compile_commands.json
                                              ${CMAKE_SOURCE_DIR}/compile_commands.json
    DEPENDS
        ${CMAKE_BINARY_DIR}/compile_commands.json
)

