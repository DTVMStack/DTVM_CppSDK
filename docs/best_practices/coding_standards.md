# Coding Standards

This document outlines the coding standards and best practices for developing smart contracts with DTVM_CppSDK.

## Code Style

### Indentation and Formatting

- Use **4 spaces** for indentation (not tabs)
- Keep line length under 100 characters
- Use consistent bracing style
- Group related code together

Example:
```cpp
class MyContract : public IContract {
protected:
    // Good: Consistent indentation with 4 spaces
    CResult myFunction(const Address& addr, const uint256& amount) override {
        if (amount > 0) {
            // Function implementation
            return Ok();
        } else {
            return Revert("Invalid amount");
        }
    }
};
```

### Naming Conventions

- **Classes**: PascalCase (e.g., `TokenContract`)
- **Functions**: camelCase (e.g., `transferTokens`)
- **Variables**: camelCase (e.g., `tokenBalance`)
- **Constants**: UPPER_SNAKE_CASE (e.g., `MAX_SUPPLY`)
- **Private members**: camelCase with underscore suffix (e.g., `balance_`)

## Documentation and Comments

### File Headers

Each source file should start with a header comment that includes:
- Brief description of the file
- Author information
- License information

Example:
```cpp
/**
 * @file MyToken.cpp
 * @brief Implementation of a basic ERC20-compatible token contract
 * @author Your Name <your.email@example.com>
 * @license See LICENSE file
 */
```

### Class and Interface Documentation

Document classes and interfaces with a description of their purpose and any important implementation notes:

```cpp
/**
 * @brief A simple ERC20-compatible token contract
 * 
 * This contract implements the basic functionality of an ERC20 token:
 * - Token transfers
 * - Balance checking
 * - Allowance mechanism
 * 
 * Note: This implementation has not been audited for security.
 */
class MyTokenImpl : public MyToken {
    // Implementation...
};
```

### Function Documentation

Document all public and protected functions with:
- Brief description
- Parameter descriptions
- Return value description
- Any side effects or exceptions

Example:
```cpp
/**
 * @brief Transfer tokens from sender to another address
 * 
 * @param to The address to transfer tokens to
 * @param amount The amount of tokens to transfer
 * @return CResult Success if transfer completed, Revert with error message otherwise
 * 
 * Emits a Transfer event if successful.
 * Reverts if sender has insufficient balance.
 */
CResult transfer(const Address &to, const uint256& amount) override {
    // Implementation...
}
```

### Comments Within Code

- Use comments to explain **why** code works a certain way, not just what it does
- Avoid unnecessary comments that simply restate the code
- Comment complex algorithms or business logic

Good:
```cpp
// Apply 2.5% fee for external transfers
// (0.5% to treasury, 2% to liquidity pool)
uint256 fee = amount * 25 / 1000;
```

Bad:
```cpp
// Subtract amount from sender
balances_->set(sender, senderBal - amount); // Don't do this
```

## Contract Structure

### Separation of Concerns

- Separate logical components into different contracts
- Use inheritance effectively, but avoid deep inheritance hierarchies
- Consider using libraries for common functionality

### State Variables

- Declare state variables at the top of the contract
- Group related variables together
- Use appropriate visibility modifiers

### Function Organization

Organize functions in the following order:
1. Constructor
2. External/public functions
3. Internal/protected helper functions
4. Private functions

## Error Handling

- Use descriptive error messages in `Revert` calls
- Validate all inputs at the beginning of functions
- Handle edge cases explicitly

Example:
```cpp
CResult withdraw(const uint256& amount) override {
    // Input validation first
    if (amount == 0) {
        return Revert("Amount must be greater than zero");
    }
    
    const auto& sender = get_msg_sender();
    uint256 balance = balances_->get(sender);
    
    if (balance < amount) {
        return Revert("Insufficient balance");
    }
    
    // Implementation after validation passes
    // ...
}
```

## Testing

- Write tests for all contract functionality
- Test edge cases and failure conditions
- Structure tests logically by function or feature

## Security Considerations

- Be aware of common smart contract vulnerabilities
- Avoid using timestamp for critical logic
- Check for integer overflow/underflow
- Use access control for sensitive functions
- Consider gas costs and optimization

## Documentation Maintenance

- Update documentation when code changes
- Keep examples in documentation up to date
- Document known limitations or issues
