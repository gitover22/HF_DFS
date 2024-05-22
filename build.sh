#！/bin/bash    

set -e
BUILD_DIR="build"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
# pushd popd 入栈出栈 like cd
pushd ${BUILD_DIR}
    cmake ..
    make
popd