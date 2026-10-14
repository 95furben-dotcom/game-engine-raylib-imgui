#!/bin/bash
set -e

PROJECT_ROOT=$1
COMPILES_DIR="$PROJECT_ROOT/.cache/compiles"

# Create output directory
mkdir -p "$COMPILES_DIR"

# Compile app
echo "Compiling app..."
clang++ $(find . -name '*.cpp' -not -path './.cache/*') \
    -std=c++23 \
    -I/opt/homebrew/include \
    -Ivendor/imgui \
    -Ivendor/rlImGui \
    -L/opt/homebrew/lib \
    -lraylib \
    $(sdl2-config --cflags --libs) \
    -o .cache/App

# Run
echo "Running..."
echo 
echo 
./".cache/App"

echo 
echo