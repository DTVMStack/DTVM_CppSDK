Contract Interfaces
=======

## Overview

In this SDK, contract interface definitions are implemented through the `SOLIDITY` macro. This mechanism allows developers to use Solidity-like syntax to declare contract interfaces, storage variables, events, and methods.

## Interface Definition Syntax

### SOLIDITY Macro

```cpp
SOLIDITY(filename_identifier, R"""(
    Solidity code
)""")
```

#### Parameter Description
- First parameter: filename identifier, used to generate the corresponding C++ header file
- Second parameter: Solidity code contained in a raw string literal `R"""(...)"""` 

## Supported Solidity Syntax Elements

### 1. Interface

```solidity
interface IMyInterface {
    function methodName(parameters) external returns (returnType);
}
```

#### Characteristics
- Only method declarations, no implementations
- Can inherit from other interfaces
- Cannot define constructors
- Methods are default `external`

### 2. Abstract Contract

```solidity
abstract contract MyContract is IMyInterface {
    // Storage variables
    mapping(address => uint256) private balances;
    
    // Events
    event Transfer(address from, address to, uint256 amount);
    
    // Abstract methods
    function abstractMethod() public virtual;
}
```

#### Characteristics
- Can include storage variables
- Can define events
- Can only define abstract methods
- Can inherit from interfaces or other abstract contracts
- Cannot define constructors

## Supported Data Types

### Basic Types
- `uint8`, `uint16`, `uint32`, `uint64`, `uint128`, `uint256`
- `int8`, `int16`, `int32`, `int64`, `int128`, `int256`
- `bool`, `address`, `string`

### Composite Types
- Fixed-length arrays: such as `uint32[5]`
- Dynamic arrays: such as `uint32[]`
- Mappings: such as `mapping(address => uint256)`

### Special Types
- `bytes`
- `bytes32`

## Storage Variable Declarations

```solidity
abstract contract MyContract {
    // Basic type storage variables
    uint256 private tokenAmount;
    
    // Mapping type storage variables
    mapping(address => uint256) private balances;
    
    // Array type storage variables
    uint32[] private transactions;
}
```

## Event Declarations

```solidity
abstract contract MyContract {
    // Simple event
    event Transfer(address from, address to, uint256 amount);
    
    // Complex event
    event Approval(address indexed owner, address indexed spender, uint256 value);
}
```

### Event Characteristics
- Can use the `indexed` keyword
- Supports multiple parameters
- Can be triggered in C++ implementation using `emitEventName()`

## Method Types

### External Methods
```solidity
function transfer(address to, uint256 amount) external;
```

### Abstract Methods (Virtual)
```solidity
function calculateFee() public virtual returns (uint256);
```

## Notes

1. The `SOLIDITY` macro is only used to generate C++ declarations, not directly executed
2. The generated header file will contain corresponding C++ class declarations
3. Actual business logic needs to be implemented in the inherited C++ class

## Best Practices

- Keep interface definitions concise and clear
- Use appropriate access modifiers
- Design storage variables and methods properly
- Follow Solidity coding standards
