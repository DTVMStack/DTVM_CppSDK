Type System
==============

This document details the type system in the C++ WASM contract SDK, including the mapping relationship between Solidity types and C++ types, as well as how to use each type.

## Basic Type Mapping

The following correspondences exist between Solidity types and C++ types:

| Solidity Type | C++ Type | Description |
|------------|---------|------|
| address    | Address | 20-byte address type |
| uint256    | uint256 | 256-bit unsigned integer |
| uint128    | __uint128_t | 128-bit unsigned integer |
| uint64     | uint64_t | 64-bit unsigned integer |
| uint32     | uint32_t | 32-bit unsigned integer |
| uint8      | uint8_t | 8-bit unsigned integer |
| int256     | int256 | 256-bit signed integer |
| int128     | __int128_t | 128-bit signed integer |
| int64      | int64_t | 64-bit signed integer |
| int32      | int32_t | 32-bit signed integer |
| int8       | int8_t | 8-bit signed integer |
| bool       | bool | Boolean type |
| string     | std::string | String type |
| T[]        | std::vector<T> | Array type |

## Example Code

```cpp
#include "contractlib/v1/contractlib.hpp"

SOLIDITY(type_example, R"""(
pragma solidity ^0.8.0;

contract TypeExample {
    // Basic type storage variables
    address public owner;
    uint256 public value;
    uint128 public smallValue;
    bool public flag;
    string public name;
    
    // Function parameters and return values using various types
    function exampleFunction(
        address _addr,
        uint256 _value,
        uint128 _smallValue,
        bool _flag,
        string memory _name
    ) public returns (uint256);
}
)""")

class TypeExampleImpl : public TypeExample {
protected:
    CResult constructor(dtvm::CallInfoPtr call_info, Input &input) override {
        // Using various types in the constructor
        owner_->set(get_msg_sender());  // Address type
        value_->set(uint256(1000));     // uint256 type
        smallValue_->set(__uint128_t(100)); // uint128 type
        flag_->set(true);               // bool type
        name_->set("Example");          // string type
        return Ok();
    }

    CResult exampleFunction(
        const Address& _addr,    // address maps to Address
        const uint256& _value,   // uint256 maps to uint256
        const __uint128_t& _smallValue, // uint128 maps to __uint128_t
        bool _flag,             // bool maps to bool
        const std::string& _name // string maps to std::string
    ) override {
        // Using passed parameters
        owner_->set(_addr);
        value_->set(_value);
        smallValue_->set(_smallValue);
        flag_->set(_flag);
        name_->set(_name);
        
        return Ok(_value);
    }
};

ENTRYPOINT(TypeExampleImpl)
```

## Array Type Mapping

Solidity's array types are mapped in C++ in two ways:

1. Arrays in function parameters/return values/event parameters:
   - Mapped to `std::vector<T>`
   - T is the corresponding C++ basic type

2. Arrays in state storage variables:
   - Mapped to `StorageArray<T>`
   - T is the corresponding C++ basic type

Example code:

```cpp
SOLIDITY(array_example, R"""(
pragma solidity ^0.8.0;

contract ArrayExample {
    // Storage arrays
    uint256[] public values;
    address[] public addresses;
    
    // Function parameters using arrays
    function processArrays(
        uint256[] memory _values,
        address[] memory _addresses
    ) public returns (uint256[] memory);
}
)""")

class ArrayExampleImpl : public ArrayExample {
protected:
    CResult constructor(dtvm::CallInfoPtr call_info, Input &input) override {
        // Using storage arrays
        values_->push_back(uint256(100));
        values_->push_back(uint256(200));
        
        addresses_->push_back(get_msg_sender());
        return Ok();
    }

    CResult processArrays(
        const std::vector<uint256>& _values,      // Array parameter
        const std::vector<Address>& _addresses    // Array parameter
    ) override {
        // Return vector
        std::vector<uint256> result;
        result.push_back(uint256(1));
        result.push_back(uint256(2));
        return Ok(result);
    }
};
```

## Mapping Type Mapping

Solidity's mapping type is mapped in C++ to `StorageMap<Key,Value>`:

```cpp
SOLIDITY(mapping_example, R"""(
pragma solidity ^0.8.0;

contract MappingExample {
    // mapping storage variables
    mapping(address => uint256) public balances;
    mapping(uint256 => string) public names;
}
)""")

class MappingExampleImpl : public MappingExample {
protected:
    CResult constructor(dtvm::CallInfoPtr call_info, Input &input) override {
        // Using mapping
        Address addr = get_msg_sender();
        balances_->set(addr, uint256(1000));
        
        names_->set(uint256(1), "Alice");
        names_->set(uint256(2), "Bob");
        
        // Reading from mapping
        uint256 balance = balances_->get(addr);
        std::string name = names_->get(uint256(1));
        
        return Ok();
    }
};
```

## Important Notes

1. When using integer types in C++, special attention needs to be paid to overflow issues, especially when calculating amounts

2. When using complex types (string, arrays, etc.) in function parameters, it's recommended to use const references to avoid unnecessary copying

3. StorageArray and StorageMap are special storage types that can only be used for state variables, not for local variables

4. Operations on storage types (such as set, get, etc.) may incur gas costs, so gas optimization should be considered when using them

These type mapping relationships allow developers to conveniently handle various Solidity data types in C++ while maintaining type safety and gas efficiency.
