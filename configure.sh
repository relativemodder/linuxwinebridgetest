#!/usr/bin/env bash

CROSS_TOOLS_DIR="$HOME/.local/share/Geode/cross-tools"
CROSS_SDK_DIR="$CROSS_TOOLS_DIR/clang-msvc-sdk"
SPLAT_DIR="$CROSS_TOOLS_DIR/splat"

/usr/bin/cmake -DCMAKE_BUILD_TYPE:STRING=RelWithDebInfo \
    -DCMAKE_EXPORT_COMPILE_COMMANDS:STRING=ON \
    -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/clang-cl \
    -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/clang-cl \
    -DCMAKE_TOOLCHAIN_FILE:FILEPATH=$CROSS_SDK_DIR/clang-cl-msvc.cmake \
    -DHOST_ARCH:STRING=x86_64 -DSPLAT_DIR:STRING=$SPLAT_DIR \
    --no-warn-unused-cli -B ./build -G Ninja -S .