#!/bin/bash

#cmake -G Xcode -S . -B build -DCMAKE_PREFIX_PATH="$HOME/Qt6/6.10.1/macos"
cmake -S . -B build -DCMAKE_PREFIX_PATH="$HOME/Qt6/6.10.1/macos"
cmake --build build
