cmake_minimum_required(VERSION "3.27.0")
project(eugineEngine)

set(SOURCES
    main.cpp
    board.cpp
    piece.cpp
    helperFunctions.cpp
)

set(HEADERS
    board.h
    piece.h
    helperFunctions.h
    constants.h
    fen.h
)


# Add the executable target for main.cpp
add_executable(eugineEngine ${SOURCES})

target_compile_features(eugineEngine PRIVATE cxx_std_20)