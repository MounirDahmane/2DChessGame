#!/usr/bin/env bash
set -e

# 1. Build the project
cmake --build build

# 2. Set the library path for the local SFML
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/thirdparty/SFML-2.6.1/lib

BINARY="./build/ChessGame"

if [ -f "$BINARY" ]; then
    printf "\n--- Launching E2-E4 ---\n"
    $BINARY
else
    printf "\nError: Executable not found at $BINARY\n"
    exit 1
fi