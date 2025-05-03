#!/bin/sh

BUILD_TYPE=RELEASE

if [ "$1" = "-d" ]; then
  BUILD_TYPE="DEBUG"
fi

if command -v clang-19 >/dev/null && clang-19 --version | grep -q "version 19"; then
    export CC=clang-19
elif command -v clang >/dev/null && clang --version | grep -E "version 1[89]" >/dev/null; then
    export CC=clang
elif command -v gcc-14 >/dev/null && gcc-14 -dumpversion | grep -q "^14"; then
    export CC=gcc-14
elif command -v gcc >/dev/null && gcc -dumpversion | grep -q "^14"; then
    export CC=gcc
else
    echo "No suitable C compiler found. Please install Clang >= 18 or GCC >= 14." >&2
    exit 1
fi

echo "Using compiler: $CC"
echo "Using build type: $BUILD_TYPE"

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..
make
