#!/bin/bash
set -e

PROJECT_ROOT=$1

mkdir -p .cache

# Compile app
echo "Compiling app..."
clang++ $(find . -name '*.cpp' -not -path './.cache/*') \
    -std=c++23 \
    -I/opt/homebrew/include \
    -Ivendor/imgui \
    -Ivendor/rlImGui \
    -L/opt/homebrew/lib \
    -lraylib \
    -o .cache/App
    # $(sdl2-config --cflags --libs) \

# Run
echo "Running..."
echo 
echo 
./".cache/App"

echo 
echo