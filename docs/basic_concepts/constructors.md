Constructor Usage Guide
==============

This document provides detailed instructions on how to implement and use constructors in C++ WASM contracts.

## Table of Contents

- [Constructor Declaration](#constructor-declaration)
- [Reading Constructor Parameters](#reading-constructor-parameters)
- [Initialization Operations in Constructors](#initialization-operations-in-constructors)
- [Inheritance and Constructors](#inheritance-and-constructors)
- [Error Handling](#error-handling)
- [Best Practices](#best-practices)

## Constructor Declaration

Since constructors cannot be declared in Solidity abstract classes and interfaces, constructors in C++ contracts need to be implemented in the contract class. Also, because parameters cannot be obtained from the Solidity interface code, parameters must be manually decoded in the C++ contract constructor.

In C++ contract classes, constructors need to override the `constructor` method of the base class (note that currently C++ contracts without parameters also need to declare an empty constructor):

```cpp
class MyToken : public SolidityBase {
protected:
    CResult constructor(dtvm::CallInfoPtr call_info, Input &input) override {
        // Constructor implementation. Including parameter decoding, contract initialization logic, etc.
        return Ok();
    }
};
```

Constructors have two parameters:
- `call_info`: Contains call context information
- `input`: Used to read constructor parameters

## Reading Constructor Parameters

Since constructors cannot be declared in Solidity abstract classes and interfaces, C++ constructors need to read parameters from the input parameter in sequence:

```cpp
class MyToken : public SolidityBase {
protected:
    CResult constructor(dtvm::CallInfoPtr call_info, Input &input) override {
        // Read parameters in the declared order
        std::string name = input.read<std::string>();
        std::string symbol = input.read<std::string>();
        uint8_t decimals = input.read<uint8_t>();
        uint256 initial_supply = input.read<uint256>();
        
        // Use these parameters for initialization
        return Ok();
    }
};
```

### Supported Parameter Types

Constructors support reading all parameter types supported by contract interfaces, including bool, integers, address, strings, etc. Usage examples:

```cpp
// Basic types
bool flag = input.read<bool>();
uint8_t u8_value = input.read<uint8_t>();
int16_t i16_value = input.read<int16_t>();
uint256 u256_value = input.read<uint256>();

// Strings
std::string str = input.read<std::string>();

// Addresses
Address addr = input.read<Address>();

```

## Initialization Operations in Constructors

Constructors are typically used to initialize the state of the contract:

```cpp
class MyToken : public SolidityBase {
protected:
    CResult constructor(dtvm::CallInfoPtr call_info, Input &input) override {
        // 1. Read parameters
        std::string name = input.read<std::string>();
        std::string symbol = input.read<std::string>();
        uint256 initial_supply = input.read<uint256>();
        
        // 2. Initialize storage variables
        name_->set(name);
        symbol_->set(symbol);
        
        // 3. Initialize balances
        Address sender = get_msg_sender();
        balances_->set(sender, initial_supply);
        total_supply_->set(initial_supply);
        
        // 4. Trigger initialization event
        emitTransfer(Address::zero(), sender, initial_supply);
        
        return Ok();
    }
};
```

## Inheritance and Constructors

When a contract inherits from multiple base classes, the constructor implementation method:

```cpp
SOLIDITY(base_contracts, R"""(
abstract contract TokenBase {
    string private name;
    string private symbol;
}

abstract contract Pausable {
    address private admin;
    bool private paused;
}

abstract contract MyToken is TokenBase, Pausable {
    mapping(address => uint256) private balances;
}
)""")

class MyTokenImpl : public MyToken {
protected:
    CResult constructor(dtvm::CallInfoPtr call_info, Input &input) override {
        // Read parameters needed for all base classes
        std::string name = input.read<std::string>();
        std::string symbol = input.read<std::string>();
        Address admin = input.read<Address>();
        
        // Initialize TokenBase state
        name_->set(name);
        symbol_->set(symbol);
        
        // Initialize Pausable state
        admin_->set(admin);
        paused_->set(false);
        
        // Initialize the current contract's state
        balances_->set(get_msg_sender(), uint256(1000000));
        
        return Ok();
    }
};
```

## Error Handling

Error handling in constructors:

```cpp
CResult constructor(dtvm::CallInfoPtr call_info, Input &input) override {
    try {
        std::string name = input.read<std::string>();
        std::string symbol = input.read<std::string>();
        
        // Parameter validation
        if (name.empty() || symbol.empty()) {
            return Revert("name and symbol cannot be empty");
        }
        
        // Permission check
        if (get_msg_sender() == Address::zero()) {
            return Revert("invalid deployer address");
        }
        
        // Initialization operations
        name_->set(name);
        symbol_->set(symbol);
        
        return Ok();
    } catch (const std::exception& e) {
        return Revert(e.what());
    }
}
```

## Best Practices

1. **Strict parameter validation in contract constructors**
```cpp
CResult constructor(dtvm::CallInfoPtr call_info, Input &input) override {
    std::string name = input.read<std::string>();
    std::string symbol = input.read<std::string>();
    uint256 initial_supply = input.read<uint256>();
    
    // Validate parameters
    if (name.empty()) {
        return Revert("name cannot be empty");
    }
    if (symbol.empty()) {
        return Revert("symbol cannot be empty");
    }
    if (initial_supply == uint256(0)) {
        return Revert("initial supply cannot be zero");
    }
    
    // Continue initialization...
    return Ok();
}
```

2. **Event Logging**
```cpp
CResult constructor(dtvm::CallInfoPtr call_info, Input &input) override {
    std::string name = input.read<std::string>();
    std::string symbol = input.read<std::string>();
    uint256 initial_supply = input.read<uint256>();
    
    // Initialization...
    
    // Record initialization event
    emitTokenInitialized(
        get_msg_sender(),
        name,
        symbol,
        initial_supply
    );
    
    return Ok();
}
```

3. **Security Considerations**
- Validate all input parameters
- Record privileged addresses to storage variables in the contract constructor when there are permission management requirements
- Ensure correct initialization of critical states
