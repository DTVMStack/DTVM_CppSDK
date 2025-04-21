How to use
============

# Environment Setup

Currently, it is recommended to use Ubuntu 22.04 environment, or you can use Docker.

# Installing Dependencies

* Install git, python3, xz

```
apt update -y
apt install -y git python3 xz-utils
```

* Install emsdk 3.1.69 (clang-based C++ to WASM compiler), it is recommended to use this specific version

```
mkdir -p /opt
cd /opt
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install 3.1.69
./emsdk activate 3.1.69
# activate in current bash
source "/opt/emsdk/emsdk_env.sh"
# configure when startup bash
echo 'source "/opt/emsdk/emsdk_env.sh"' >> $HOME/.bash_profile
```

* Download the toolkit to the /opt directory

```
cp path/to/DTVM_CppSDK.tar.gz /opt
cd /opt
tar -xzvf DTVM_CppSDK.tar.gz
# Check if all binary programs are available
./solc --version
./solidcpp --version

# Add the toolkit to the PATH environment variable
echo 'export PATH="/opt:$PATH"' >> $HOME/.bash_profile
source $HOME/.bash_profile
```

# Developing Contracts

Refer to [tutorial.md](tutorial.md) for contract development. The downloaded toolkit also includes several simple example contracts. Here we use the example contracts to illustrate how to compile contracts with this toolkit.

# Compiling Contracts

Here we use the ERC20 contract in the toolkit as an example

```
# Navigate to the contract directory, you should navigate to your own contract directory
cd /opt/examples/erc20

# Compile C++ contract source files to WASM bytecode files. If you have multiple contract source files, you can specify multiple input parameters
solidcpp build --input my_erc20.cpp --contractlib-dir=../.. --generated-dir ./generated --output my_erc20.wasm

# You can view the generated contract bytecode
ll my_erc20.wasm

```

# Testing Contracts

It is recommended to test the deployment and invocation of contract interfaces and functions on a test chain after contract development to verify the correctness of the contract.

After your contracts deployed to the testnet, You can use web3 sdks or wallets to test the deployed contract.
