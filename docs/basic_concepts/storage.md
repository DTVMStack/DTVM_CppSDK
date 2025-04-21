Storage Variables Usage Guide
===============

This document details how to use Solidity-style storage variables in C++ WASM contracts.

## Contents

- [Storage Variable Declaration](#storage-variable-declaration)
- [Basic Type Storage](#basic-type-storage)
- [Mapping Type Storage](#mapping-type-storage)
- [Array Type Storage](#array-type-storage)
- [Storage Variable Access Control](#storage-variable-access-control)
- [Best Practices](#best-practices)

## Storage Variable Declaration

When declaring contracts using the SOLIDITY macro, you can define various types of storage variables:

```cpp
SOLIDITY(contract_name, R"""(
abstract contract Storage {
    // Basic types
    bool private bool_value;
    uint8 private uint8_value;
    int16 private int16_value;
    uint256 private uint256_value;
    string private string_value;
    
    // bytes types
    bytes private bytes_value;
    bytes32 private bytes32_value;
    
    // mapping types
    mapping(address => uint256) private balances;
    mapping(address => mapping(address => uint256)) private allowances;
    
    // array types
    uint32[] private uint32_array;
}
)""")
```

For specific mappings between Solidity state variable types and C++ types, please refer to the documentation [types.md](types.md).

## Basic Type Storage

When accessing basic type storage variables in C++, variable names automatically have an underscore suffix `_`. All storage variables are pointer types and need to be accessed using `->`.

### Reading Storage Values

Use the `get()` method to read storage values:

```cpp
// Read bool value
bool value = bool_value_->get();

// Read numeric values
uint8_t v1 = uint8_value_->get();
int16_t v2 = int16_value_->get();
uint256 v3 = uint256_value_->get();

// Read string
std::string str = string_value_->get();

// Read bytes
Bytes b1 = bytes_value_->get();
std::array<uint8_t, 32> b2 = bytes32_value_->get();
```

### Writing Storage Values

Use the `set()` method to write storage values:

```cpp
// Write bool value
bool_value_->set(true);

// Write numeric values
uint8_value_->set(123);
int16_value_->set(-100);
uint256_value_->set(uint256(1000000));

// Write string
string_value_->set("Hello");

// Write bytes
bytes_value_->set(Bytes({1, 2, 3, 4}));
std::array<uint8_t, 32> b32 = {0};
bytes32_value_->set(b32);
```

## Mapping Type Storage

Mapping types support both single-level and multi-level mappings. When using them in C++, access and modify values through the `get()` and `set()` methods.

### Single-level Mapping

```cpp
// Read mapping value
Address account = /*...*/;
uint256 balance = balances_->get(account);

// Modify mapping value
balances_->set(account, uint256(1000));
```

### Multi-level Mapping

The SDK provides convenient access methods for two-level mappings:
```cpp
// allowances_ has Solidity type mapping(address => mapping(address => uint256))
// Read two-level mapping
Address owner = /*...*/;
Address spender = /*...*/;
uint256 allowance = allowances_->get(owner, spender);

// Modify two-level mapping
allowances_->set(owner, spender, uint256(500));
```

## Array Type Storage

Array type storage provides rich operation methods.

### Basic Array Operations

```cpp
// Get array length
size_t len = uint32_array_->size();

// Read value at specific position
uint32_t value = uint32_array_->get(0);

// Modify value at specific position
uint32_array_->set(0, 123);

// Append element
uint32_array_->push(456);

// Pop last element (only modifies array length, doesn't clear the original value. Developers need to be aware of this)
uint32_array_->pop();

```

### Iterating Arrays

```cpp
size_t len = uint32_array_->size();
for(size_t i = 0; i < len; i++) {
    uint32_t value = uint32_array_->get(i);
    // Process value...
}
```

## Storage Variable Access Control

Storage variable access control in Solidity (public, private, internal) affects the generated C++ code:

- `private`: Can only be accessed within the current contract
- `internal`: Can be accessed within the current contract and its inheriting contracts
- `public`: Automatically generates getter methods, can be accessed externally

```cpp
SOLIDITY(storage_access, R"""(
abstract contract StorageAccess {
    uint256 private priv_value;     // Can only be accessed within the contract
    uint256 internal internal_value; // Can be accessed in inheriting contracts
    uint256 public pub_value;       // Automatically generates getter method
}
)""")
```

Storage variables with different access permissions all generate meaningful storage slots and internal state variables, but public and internal also generate corresponding public contract interfaces and internal C++ access functions.

## Best Practices

1. **Atomic Operations**  
For scenarios requiring atomic operations (such as balance updates), complete reading and writing in a single function call:

```cpp
CResult transfer(const Address &to, const uint256& amount) {
    const auto& sender = get_msg_sender();
    uint256 sender_bal = balances_->get(sender);
    if (sender_bal < amount) {
        return Revert("insufficient balance");
    }
    balances_->set(sender, sender_bal - amount);
    uint256 to_bal = balances_->get(to);
    balances_->set(to, to_bal + amount);
    return Ok();
}
```

2. **Storage Space Optimization**  
To save storage space, it's recommended to:
- Use appropriately sized integer types
- Use memory variables instead of storage variables for data that doesn't need on-chain storage

3. **Security Considerations**  
- Perform necessary permission checks before modifying storage
- Validate user input
- Consider integer overflow issues

