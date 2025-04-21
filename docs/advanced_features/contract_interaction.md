Contract Interaction
===============

## Overview

In WASM smart contract development, interaction between contracts is a crucial functionality. This document will detail how to implement cross-contract calls, message passing, and interactions in this C++ contract SDK.

## Contents

- [Contract Call Basics](#contract-call-basics)
- [Creating Contract Instances](#creating-contract-instances)
- [Regular Call](#regular-call)
- [staticCall](#staticcall)
- [delegateCall](#delegatecall)
- [Error Handling](#error-handling)
- [Best Practices](#best-practices)

## Contract Call Basics

Cross-contract calls allow one contract to execute methods of another contract. In our C++ contract SDK, this is primarily implemented through interface instantiation and method calls.

### Interface Definition Example

```cpp
SOLIDITY(token_service, R"""(
pragma solidity ^0.8.0;

interface ITokenService {
  function balanceOf(address owner) external returns(uint256);
  function transfer(address to, uint256 amount) external;
}
)""")
```

## Creating Contract Instances

In the C++ code of a contract, you can create instances of other contracts using the following method:

### Through Interface Static Method

```cpp
// Create an instance using a known contract address
auto token_contract = ITokenService::from(token_address);
```

## Regular Call

Dynamic calls can pass more complex parameters and handle:

```cpp
CResult performTokenTransfer(
    const Address& token_address, 
    const Address& recipient, 
    const uint256& amount
) {
    auto token_contract = ITokenService::from(token_address);
    
    // Call the transfer method
    CResult transfer_result = token_contract->transfer(recipient, amount);
    
    if (!transfer_result.success()) {
        // Transfer successful
        return Ok();
    }
    
    return Revert("Transfer failed");
}
```

## staticCall

Static calls are read-only operations that do not modify the blockchain state:

```cpp
CResult checkBalance(const Address& token, const Address& owner) {
    auto token_contract = ITokenService::from(token);
    
    // Directly call methods of other contracts
    CResult balance = token_contract->balanceOf(owner);
    auto call_info = dtvm::current_call_info();
    std::vector<uint8_t> encoded_input = { 1,2,3,4 }; // function selector bytes, insert actual values
    std::vector<uint8_t> encoded_args = dtvm::abi_encode(std::make_tuple(owner, spender));
    encoded_input.insert(encoded_input.end(), encoded_args.begin(), encoded_args.end());
    CResult balance = dtvm::call_static(token, encoded_input, call_info->value, call_info->gas);
    
    if (balance.isOk()) {
        uint256 token_balance = balance.get<uint256>();
        // Process the balance
        return Ok(token_balance);
    }
    
    return Revert("Balance check failed");
}
```


## delegateCall

Delegate calls execute using the bytecode of the called contract but in the context of the current contract's state and address (the sender of the delegate call).

```cpp
CResult checkBalance(const Address& token, const Address& owner) {
    auto token_contract = ITokenService::from(token);
    
    // Directly call methods of other contracts
    CResult balance = token_contract->balanceOf(owner);
    auto call_info = dtvm::current_call_info();
    std::vector<uint8_t> encoded_input = { 1,2,3,4 }; // function selector bytes, insert actual values
    std::vector<uint8_t> encoded_args = dtvm::abi_encode(std::make_tuple(owner, spender));
    encoded_input.insert(encoded_input.end(), encoded_args.begin(), encoded_args.end());
    CResult balance = dtvm::call_delegate(token, encoded_input, call_info->value, call_info->gas);
    
    if (balance.isOk()) {
        uint256 token_balance = balance.get<uint256>();
        // Process the balance
        return Ok(token_balance);
    }
    
    return Revert("Balance check failed");
}
```

## Error Handling

Cross-contract calls need careful handling of potential errors:

```cpp
CResult safeContractCall(const Address& contract_address) {
    auto contract = ITokenService::from(contract_address);
    CResult result = contract->someMethod();
    
    if (!result.success()) {
        // Handle call failure
        return Revert(result.data());
    }
    
    return result;
}
```

## Best Practices

1. Use delegate calls with caution
2. Use interfaces for type-safe contract calls
3. Implement appropriate error handling
4. Be mindful of gas consumption in calls
5. Avoid circular calls and complex cross-contract dependencies
6. Be careful to prevent reentrancy attacks

## Notes

- Cross-contract calls may increase gas consumption
- The target contract must be deployed and have a valid address

