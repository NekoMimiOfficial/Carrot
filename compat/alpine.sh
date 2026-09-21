#!/bin/bash
set -e

doas apk add --no-cache readline-static ncurses-static ncurses-dev

cmake -B build -DCMAKE_EXE_LINKER_FLAGS="-static" -DBUILD_SHARED=OFF
cmake --build build
# run in component dir
# alpine needs musl, maybe not a good idea since static linking means bye bye dlfcn qwq
