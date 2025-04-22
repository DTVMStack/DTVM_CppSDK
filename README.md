# DTVM_CppSDK

[![Build Status](https://travis-ci.org/DTVMStack/DTVM_CppSDK.svg?branch=master)](https://travis-ci.org/DTVMStack/DTVM_CppSDK)
![license](https://img.shields.io/badge/license-Apache--2.0-green.svg)


DTVM_CppSDK is a C++ SDK for developing smart contracts on blockchain. This SDK is part of the DTVM ecosystem and enables developers to write smart contracts in C++ that compile to WebAssembly (WASM) bytecode for deployment on EVM-compatible wasm contract blockchains.

## Overview

DTVM_CppSDK provides a familiar development environment for C++ developers while leveraging the performance benefits of WASM. It includes tools and libraries to simplify the development of blockchain-based applications.

## Dependencies

* **solc** - Solidity compiler
* **Rust** - Required for some build tools
* **Emscripten 3.1.69** - C++ to WebAssembly compiler
* **Clang** - With C++17 support

## Project Structure

* **contractlib/** - Core contract library providing blockchain abstractions
* **examples/** - Example contracts
  * **erc20/** - ERC20 token implementation 
  * **c_erc20/** - Alternative ERC20 implementation
  * **example1/** - Basic token example with detailed comments
* **docs/** - Documentation
* **solidcpp/** - C++ to WASM compiler tooling

## Getting Started

For detailed installation and usage instructions, see the [documentation](docs/readme.md).

Quick start:
```bash
# Build the library
bash build_lib.sh

# Navigate to an example
cd examples/example1

# Build the example
bash build_wasm.sh

# Test the example locally
bash test_in_mock_cli.sh
```

## Community
[Issues](https://github.com/DTVMStack/DTVM_CppSDK/issues) - Report issues or request features.

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for details.

## License

This project is licensed under the [LICENSE](LICENSE) file.

