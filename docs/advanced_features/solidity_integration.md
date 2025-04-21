Solidity Contract Integration Guide
========

## Overview

This document describes how to interact with Solidity contracts in our C++ WASM smart contract SDK.

## Compatibility Mechanism

Our C++ SDK implements seamless interaction with Solidity contracts through unified method selectors and parameter encoding/decoding mechanisms. This means C++ contracts can directly call and be called by Solidity contracts, just as in a native Solidity environment.

## Calling Solidity Contracts

### Creating Contract Instances

Use the `from` static method of the interface to create contract instances:

```cpp
// Assuming ITokenService interface is already defined
auto token_contract = ITokenService::from(token_address);
```

### Calling Contract Methods

Directly call contract methods:

```cpp
// Call the balanceOf method
CResult balance = token_contract->balanceOf(owner_address);
```

## Solidity Contracts Calling C++ Contracts

Solidity contracts can call C++ contracts just like regular contracts, without any special handling.

## Parameter and Return Value Encoding

The SDK is fully compatible with Solidity's encoding rules:
- Parameters are encoded according to ABI specifications
- Supports basic types: `uint256`, `address`, `bool`, `bytes`, etc.
- Supports arrays and complex types
- Return values follow the same encoding mechanisms

## Event Handling

### Triggering Events

Triggering events in C++ contracts:

```cpp
// Trigger Solidity-defined events
emitTransfer(from_address, to_address, amount);
```

## Storage Compatibility

- Mappings
- Dynamic arrays
- Basic type storage
- Nested structures

All are fully compatible with Solidity's storage mechanisms.

## Important Notes

1. Ensure method signatures match exactly
2. Follow Solidity's ABI encoding rules
3. Pay attention to precise type matching
