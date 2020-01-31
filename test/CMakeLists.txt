add_executable(chip8tests
    ${CMAKE_SOURCE_DIR}/src/cpu.cpp
    ${CMAKE_SOURCE_DIR}/src/gpu.cpp
    ${CMAKE_SOURCE_DIR}/src/memory.cpp
    ${CMAKE_SOURCE_DIR}/src/debugger.cpp
    test_cpu.cpp
    main.cpp
)

target_include_directories(chip8tests
    PRIVATE
        .
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/deps
)
