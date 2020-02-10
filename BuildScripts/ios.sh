#!/bin/bash -e

if [ ! "$1" = "" ]; then
    echo "Invalid build argument, not allowed"
    exit 1
fi

CUR_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
source $CUR_DIR/setup-env.sh

echo "Find cmake"
if [ -x "$(command -v cmake)" ]; then
    CMAKE=cmake
else
    CMAKE=/Applications/CMake.app/Contents/bin/cmake
fi

mkdir -p $FO_BUILD_DEST && cd $FO_BUILD_DEST

# Cross compilation using OSXCross
if [ -d "osxcross" ]; then
    echo "OSXCross cross compilation"
    export OSXCROSS_DIR=$(cd osxcross; pwd)
    export PATH=$PATH:$OSXCROSS_DIR/target/bin
    CMAKE_GEN="$OSXCROSS_DIR/target/bin/x86_64-apple-darwin19-cmake -G \"Unix Makefiles\" -DCMAKE_TOOLCHAIN_FILE=\"$OSXCROSS_DIR/tools/toolchain.cmake\""
else
    CMAKE_GEN="$CMAKE -G \"Xcode\""
fi

mkdir -p iOS && cd iOS

echo "Build default binaries"
mkdir -p default && cd default
eval $CMAKE_GEN -C "$ROOT_FULL_PATH/BuildScripts/ios.cache.cmake" -DFONLINE_OUTPUT_BINARIES_PATH="../../" "$ROOT_FULL_PATH"
$CMAKE --build . --config Release --target FOnline
