#!/usr/bin/env bash
set -e

BUILD_TYPE=${1:-Debug}

if [[ "$BUILD_TYPE" != "Debug" && "$BUILD_TYPE" != "Release" ]]; then
    echo "Usage: $0 [Debug|Release]"
    exit 1
fi

echo "Configuring E2-E4 in $BUILD_TYPE mode..."

cmake \
    -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON