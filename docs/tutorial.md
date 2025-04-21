Tutorial
========

# Tutorial for Writing WASM Contracts in DTVM_CppSDK

This tutorial introduces how to use DTVM_CppSDK to write WASM contracts in C++. The main feature of this library is the ability to declare Solidity interfaces, events, and storage in C++ contracts using the `SOLIDITY` macro. You can then write C++ classes that inherit these Solidity interfaces and implement business logic in C++. In the C++ code, you can read and write Solidity storage, emit events, call other contracts, and more.

Next, I will walk you through a complete example code to explain step by step how to use this library to write a simple contract.

## Table of Contents

- [Environment Setup](#environment-setup)
- [Using the SOLIDITY Macro](#using-the-solidity-macro)
- [Generating C++ Declaration Files](#generating-c-declaration-files)
- [Implementing Contract Logic](#implementing-contract-logic)
  - [Constructor](#constructor)
  - [Reading Storage Data](#reading-storage-data)
  - [Calling Other Contracts](#calling-other-contracts)
  - [Implementing Interface Methods](#implementing-interface-methods)
  - [Receiving Native Tokens](#receiving-native-tokens)
- [Defining Entry Points](#defining-entry-points)
- [Complete Code Example](#complete-code-example)

## Environment Setup

Before starting, make sure you have installed this toolkit and its dependencies as described in the [usage.md](usage.md) document.

## Using the SOLIDITY Macro

In C++ code, we use the `SOLIDITY` macro to embed Solidity code. This macro does not actually perform any operations, but our toolchain uses it to generate corresponding C++ declaration files.


```cpp
#include "contractlib/v1/contractlib.hpp"

SOLIDITY(my_token, R"""(
pragma solidity ^0.8.0;

interface ITokenService {
  function balanceOf(address owner) external returns(uint256);
  function transfer(address to, uint256 amount) external;
}

abstract contract MyToken is ITokenService {
  mapping(address => uint256) private balances;
  string private token_symbol;
  bool private bool_value;
  uint8 private uint8_value;
  int16 private int16_value;
  uint256 private uint256_value;
  bytes private bytes_value;
  bytes32 private bytes32_value;
  uint32[] private uint32_array;
  uint128 private uint128_value;
  int128 private int128_value;

  function callOtherBalance(address token, address owner) public virtual returns (uint256);

  event Transfer(address from, address to, uint256 amount);
}
)""")
```

In the code above:

- The first parameter of the `SOLIDITY` macro is the file name identifier `my_token`, which is used to generate an intermediate C++ header file. This generated C++ header file will need to be included later in the code.
The second parameter is the Solidity code, enclosed in a C++ raw string literal `R"""(...)"""`. 

This Solidity code defines:

- An interface `ITokenService` with the methods `balanceOf` and `transfer`.
- An abstract contract `MyToken` that inherits from `ITokenService`, adds storage variables and events, and includes an abstract method `callOtherBalance`.

## Include the Generated C++ Header File

After writing the Solidity code, the toolkit will automatically generate a C++ header file based on this Solidity code during compilation. The file name prefix is the first parameter of the `SOLIDITY` macro, which in this case is `my_token`, and the suffix is `_decl.hpp`.

```cpp
#include "generated/my_token_decl.hpp"
```

## Implementing Contract Logic

Now, we can inherit `MyToken` in C++ and implement its business logic.

```cpp
using namespace dtvm;

class MyTokenImpl : public MyToken {
  // Implement business logic
};
```

### Constructor

In the constructor, we can initialize storage variables.

```cpp
protected:

CResult constructor(dtvm::CallInfoPtr call_info, Input &input) override {
    std::string symbol = input.read<std::string>();
    this->token_symbol_->set(symbol);
    balances_->set(get_msg_sender(), uint256(10000000));

    this->uint128_value_->set(123);
    this->bytes_value_->set(Bytes ({ 1, 2, 3, 4 }));
    return Ok();
}
```

- Read the `symbol` symbol passed during contract deployment and store it in `token_symbol`.
- Set the initial token balance for the deployer.
- Initialize other storage variables.
- This example contract mainly demonstrates supported syntax and does not have practical use. For meaningful examples, refer to the examples/erc20 contract.
- Since constructors cannot be declared in Solidity abstract classes and interfaces, the constructor in the C++ class must read parameters from the input argument.

### Reading Storage Data

Example methods demonstrating how to read storage variables.

```cpp
CResult test_read_bytes() {
    return Ok(this->bytes_value_->get());
}

CResult test_read_u128() {
    return Ok(this->uint128_value_->get());
}
```

### Calling Other Contracts

Implement a method to call other contracts.

```cpp
CResult callOtherBalance(const Address& token, const Address& owner) override {
    // Call another contract via the Solidity interface
    auto token_contract = ITokenService::from(token);
    return token_contract->balanceOf(owner);
}
```

- Use `ITokenService::from` to create an instance of another contract.
- Call its `balanceOf` method.

### Implementing Interface Methods

Implement the `balanceOf` and `transfer` methods defined in the interface.

```cpp
CResult balanceOf(const Address &owner) override {
    uint256 bal = this->balances_->get(owner);
    // Example of triggering a Solidity event log
    emitTransfer(owner, owner, bal);
    return Ok(bal);
}

CResult transfer(const Address &to, const uint256& amount) override {
    const auto& sender = get_msg_sender();
    uint256 sender_bal = this->balances_->get(sender);
    if (sender_bal < amount) {
        return Revert("balance not enough");
    }
    this->balances_->set(sender, sender_bal - amount);
    uint256 to_bal = this->balances_->get(to);
    this->balances_->set(to, to_bal + amount);
    emitTransfer(sender, to, amount);
    return Ok();
}
```

- `balanceOf` returns the balance of a specified address and triggers a `Transfer` event (from self to self, indicating a query operation).
- `transfer` implements token transfer logic and reverts if the balance is insufficient.

### Receiving Native Tokens

Override the `receive` method to handle the case of directly receiving native tokens (without calldata).

```cpp
public:
void receive() override {
    // Called when directly receiving native tokens (without calldata)
    emitTransfer(Address::zero(), get_msg_sender(), get_msg_value());
}
```

## Defining The contract's EntryPoint (Important)

Use the `ENTRYPOINT` macro to define the contract's entry point, generating the dispatch functions for calls and deployment. This step is mandatory because the C++ source code may contain multiple classes, and we need to specify which class to export as the contract class using the `ENTRYPOINT` macro.

```cpp
ENTRYPOINT(MyTokenImpl)
```

## Complete Code Example

The complete code example can be found in this project, in file `examples/example1/my_token.cpp`.

## Summary

Through this tutorial, you have learned how to:

- Use the `SOLIDITY` macro to declare Solidity interfaces and contracts in C++.
- Use this tool to convert Solidity code into C++ header files.
- Inherit Solidity contracts in C++ and implement business logic.
- Read and write Solidity storage variables.
- Trigger events and call other contracts.
