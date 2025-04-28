#!/bin/bash

set -e
CUR_PATH=`pwd`

export RUSTUP_DIST_SERVER=https://mirrors.ustc.edu.cn/rust-static
export RUSTUP_UPDATE_ROOT=https://mirrors.ustc.edu.cn/rust-static/rustup
# Install cargo if it doesn't exist
if ! command -v cargo &> /dev/null; then
    curl -sSf https://mirrors.ustc.edu.cn/misc/rustup-install.sh | sh -s -- -y
    . "$HOME/.cargo/env"
    mkdir -vp ${CARGO_HOME:-$HOME/.cargo}
    cp docker/cargo_config ${CARGO_HOME:-$HOME/.cargo}/config
    rustup install 1.81.0
fi
rustup default 1.81.0

if ! command -v emcc --version &> /dev/null; then
    cd /opt/emsdk
    # activate emscripten(emsdk 3.1.69 tested)
    ./emsdk activate 3.1.69
    . "/opt/emsdk/emsdk_env.sh"
    emcc --version
    cd $CUR_PATH
fi

# build contractlib.o
bash build_lib.sh
# build solidcpp
cd solidcpp
cargo build --release

cd $CUR_PATH
mkdir -vp tmp_build
cd tmp_build
cp ../solidcpp/target/release/solidcpp .
# Choose tools/chain_mockcli/linux_x86/*.zip or tools/chain_mockcli/mac_arm/*.zip based on Linux/Mac
if [[ "$OSTYPE" == "linux-gnu" ]]; then
    cp ../tools/chain_mockcli/linux_x86/*.zip .
elif [[ "$OSTYPE" == "darwin"* ]]; then
    cp ../tools/chain_mockcli/mac_arm/*.zip .
fi

cp -r ../examples ./examples
mkdir -p contractlib/ && mkdir -p contractlib/v1
cp -r ../contractlib/v1/*.hpp ./contractlib/v1/
cp -r ../contractlib/v1/*.h ./contractlib/v1/
cp -r ../contractlib/v1/*.o ./contractlib/v1/

rm -f DTVM_CppSDK.nightly.latest.tar.gz
tar -czf DTVM_CppSDK.nightly.latest.tar.gz *
