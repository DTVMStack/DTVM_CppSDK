Contract Events Usage Guide
==================

This document details how to declare and use Solidity-style events in C++ WASM contracts.

## Contents

- [Event Declaration](#event-declaration)
- [Event Parameter Types](#event-parameter-types)
- [Triggering Events](#triggering-events)
- [Indexed Parameters](#indexed-parameters)
- [Anonymous Events](#anonymous-events)
- [Best Practices](#best-practices)

## Event Declaration

When declaring contracts using the SOLIDITY macro, you can define various events:

```cpp
SOLIDITY(contract_name, R"""(
abstract contract Events {
    // Basic event declaration
    event Transfer(address from, address to, uint256 amount);
    
    // Event with indexed parameters
    event Approval(address indexed owner, address indexed spender, uint256 value);
    
    // Event with different parameter types
    event ComplexEvent(
        bool bool_value,
        uint8 uint8_value,
        int16 int16_value,
        uint256 uint256_value,
        string string_value,
        bytes bytes_value,
        bytes32 bytes32_value
    );
}
)""")
```

## Event Parameter Types

Events support the following parameter types:

- Basic types: `bool`, `uint8`, `int16`, `uint256` and other integer types
- Address type: `address`
- String: `string`
- Byte types: `bytes`, `bytes32` and other byte array types
- Array types: supports both fixed-length and dynamic arrays

## Triggering Events

In C++ code, event triggering methods are automatically generated with the format `emit` + event name. For example:

```cpp
class MyContract : public Events {
    CResult transfer(const Address& from, const Address& to, const uint256& amount) {
        // Business logic...
        
        // Trigger Transfer event
        emitTransfer(from, to, amount);
        
        return Ok();
    }
    
    CResult approve(const Address& spender, const uint256& value) {
        // Business logic...
        
        // Trigger Approval event with indexed parameters
        emitApproval(get_msg_sender(), spender, value);
        
        return Ok();
    }
    
    void receive() override {
        // Trigger anonymous Deposit event
        emitDeposit(get_msg_sender(), get_msg_value());
    }
    
    CResult test_complex_event() {
        // Trigger event with multiple parameter types
        emitComplexEvent(
            true,                           // bool
            uint8_t(123),                   // uint8
            int16_t(-100),                  // int16
            uint256(1000000),              // uint256
            std::string("Hello"),          // string
            Bytes({1, 2, 3, 4}),          // bytes
            std::array<uint8_t, 32>{0}     // bytes32
        );
        
        return Ok();
    }
};
```

## Indexed Parameters

Indexed parameters are event parameters that can be efficiently indexed and filtered externally. Use the `indexed` keyword when declaring events:

```cpp
SOLIDITY(indexed_events, R"""(
abstract contract IndexedEvents {
    // Maximum of 3 indexed parameters supported
    event Transfer(
        address indexed from,    // Can be indexed
        address indexed to,      // Can be indexed
        uint256 amount          // Normal parameter
    );
    
    // Support for different types of indexed parameters
    event CustomEvent(
        uint256 indexed id,
        bytes32 indexed hash,
        address indexed account,
        string data
    );
}
)""")
```

When triggering events with indexed parameters in C++ code, parameter order must match the declaration:

```cpp
// Trigger event with indexed parameters
emitTransfer(sender_address, receiver_address, amount);

// Trigger event with multiple indexed parameters
emitCustomEvent(
    uint256(123),              // indexed id
    bytes32_hash,              // indexed hash
    some_address,              // indexed account
    "some data"                // normal parameter
);
```

## Best Practices

1. **Event Naming Conventions**
```cpp
// Recommended naming patterns
event Transfer(...);        // Represents an action
event Approval(...);        // Represents a state change
event DepositReceived(...); // Clearly expresses event meaning
```

2. **Appropriate Use of Indexed Parameters**
- Use for important fields that need external query filtering
- Maximum of 3 indexed parameters per event
- Consider the balance between query performance and gas cost

```cpp
// Good use of indexed parameters
event Transfer(
    address indexed from,    // For querying transfers sent by users
    address indexed to,      // For querying transfers received by users
    uint256 amount          // Amount doesn't need indexing
);
```

3. **Event Parameter Design**
- Include sufficient information to track state changes
- Avoid including sensitive information
- Consider future compatibility

```cpp
// Event with comprehensive information
event TokenMint(
    address indexed to,
    uint256 amount,
    uint256 totalSupply,    // Includes updated total supply
    uint256 timestamp       // Includes timestamp
);
```

4. **Documentation Comments**
It's recommended to add comments explaining important events:

```cpp
SOLIDITY(documented_events, R"""(
abstract contract DocumentedEvents {
    /// @notice Triggered when tokens are transferred
    /// @param from Token sender address
    /// @param to Token recipient address
    /// @param amount Transfer amount
    event Transfer(address indexed from, address indexed to, uint256 amount);
}
)""")
```

5. **Events in Error Handling**
Events can also be triggered when handling errors or special cases:

```cpp
CResult transfer(const Address& to, const uint256& amount) {
    const auto& sender = get_msg_sender();
    uint256 balance = balances_->get(sender);
    
    if (balance < amount) {
        // Trigger transfer failure event
        emitTransferFailed(sender, to, amount, "insufficient balance");
        return Revert("insufficient balance");
    }
    
    // ... transfer logic ...
    
    // Trigger success event
    emitTransfer(sender, to, amount);
    return Ok();
}
```

6. **Combining Events with Storage**
- Update storage and trigger events for important state changes
- Use events to record historical change records
- Provide additional context information through events
