# Quick Start Guide

This guide will help you quickly develop and test your first smart contract using DTVM_CppSDK.

## Prerequisites

- DTVM_CppSDK installed (see [Installation Guide](installation.md))
- Basic knowledge of C++ programming
- Familiarity with blockchain concepts

## Creating Your First Smart Contract

### 1. Create a New Project Directory

```bash
mkdir -p my_first_contract
cd my_first_contract
```

### 2. Copy the Example Files

The fastest way to get started is to use one of the existing examples as a template:

```bash
# Assuming DTVM_CppSDK is installed in /opt
cp -r /opt/examples/example1/* .
```

### 3. Understand the Contract Structure

Open the main contract file:

```bash
# Open the file in your preferred editor
nano my_token.cpp
```

Key components to understand:

- **SOLIDITY** macro: Defines the contract interface in Solidity syntax
- **Contract implementation class**: C++ class that implements the Solidity interface
- **ENTRYPOINT** macro: Generates entry points for the contract

### 4. Modify the Contract

Let's make a simple modification to the token contract:

1. Change the initial token supply
2. Modify the token name

Example changes:

```cpp
// In the constructor method
this->balances_->set(get_msg_sender(), uint256(5000000)); // Changed from 10000000
this->token_symbol_->set("MYT"); // Changed to "MYT"
```

### 5. Build the Contract

Build your contract to WebAssembly:

```bash
# Build the contract
./build_wasm.sh
```

This will generate:
- `my_token.wasm`: The WebAssembly bytecode
- `my_token.wat`: A text representation of the WebAssembly bytecode

### 6. Test the Contract

Test your contract using the mock CLI:

```bash
# Run the test script
./test_in_mock_cli.sh
```

## Contract Development Workflow

1. **Design**: Plan your contract functionality and state variables
2. **Implement**: Write your contract in C++ following the Solidity interface
3. **Build**: Compile to WebAssembly using the build tools
4. **Test**: Test locally using the mock CLI
5. **Deploy**: Deploy to a test network or mainnet

## Next Steps

- Explore the [examples directory](../../examples/) for more complex contracts
- Learn about [contract storage](../basic_concepts/storage.md)
- Understand [contract events](../basic_concepts/events.md)
- Study [contract interfaces](../advanced_features/contract_interfaces.md)

## Troubleshooting

### Common Issues

1. **Build Failures**
   - Check that Emscripten is correctly installed and activated
   - Ensure your C++ code is valid and follows C++17 standard

2. **Runtime Errors**
   - Check your contract's logic for issues
   - Verify storage variable types match their usage

3. **Testing Problems**
   - Ensure the mock CLI is correctly installed
   - Check the input format for contract calls

For more detailed troubleshooting, see the [Testing Guide](../best_practices/testing.md).

## Example: Complete Token Contract

Here's a minimal ERC20-like token contract example:

```cpp
#include "contractlib/v1/contractlib.hpp"

SOLIDITY(minimal_token, R"""(
pragma solidity ^0.8.0;

interface IMinimalToken {
  function balanceOf(address owner) external returns(uint256);
  function transfer(address to, uint256 amount) external;
}

abstract contract MinimalToken is IMinimalToken {
  mapping(address => uint256) private balances;
  string private name;
  
  event Transfer(address from, address to, uint256 amount);
}
)""")

#include "generated/minimal_token_decl.hpp"

using namespace dtvm;

class MinimalTokenImpl : public MinimalToken {
protected:
    CResult constructor(dtvm::CallInfoPtr call_info, Input &input) override {
        std::string token_name = input.read<std::string>();
        this->name_->set(token_name);
        balances_->set(get_msg_sender(), uint256(1000000));
        return Ok();
    }

    CResult balanceOf(const Address &owner) override {
        return Ok(this->balances_->get(owner));
    }

    CResult transfer(const Address &to, const uint256& amount) override {
        const auto& sender = get_msg_sender();
        uint256 sender_bal = this->balances_->get(sender);
        if (sender_bal < amount) {
            return Revert("insufficient balance");
        }
        this->balances_->set(sender, sender_bal - amount);
        uint256 to_bal = this->balances_->get(to);
        this->balances_->set(to, to_bal + amount);
        emitTransfer(sender, to, amount);
        return Ok();
    }
};

ENTRYPOINT(MinimalTokenImpl)
```
