#pragma once
#include "contractlib/contractlib.hpp"

class ITokenService : public dtvm::Contract {
public:
    virtual dtvm::CResult balanceOf(const dtvm::Address &owner) = 0;
    virtual dtvm::CResult balanceOf(dtvm::CallInfoPtr call_info, const dtvm::Address &owner) {
        return balanceOf(owner);
    }

    static std::shared_ptr<ITokenService> from(const dtvm::Address& addr);

protected:
    inline dtvm::CResult interface_balanceOf(dtvm::CallInfoPtr call_info, dtvm::Input &input) {
        dtvm::require(dtvm::get_msg_value() == 0, "not payable method");
        dtvm::Address arg0 = input.read_address();
        return balanceOf(call_info, arg0);
    }
};

// generate a proxy subclass for each solidity interface
class ITokenServiceProxy : public ITokenService {
public:
    inline ITokenServiceProxy(const dtvm::Address& addr) {
        addr_ = addr;
    }
    inline dtvm::CResult balanceOf(dtvm::CallInfoPtr call_info, const dtvm::Address& owner) override {
        std::vector<uint8_t> encoded_input = { 112, 160, 130, 49 }; // 1889567281, function selector bytes, balanceOf(address)
        std::vector<uint8_t> encoded_args = dtvm::abi_encode(std::make_tuple(owner));
        encoded_input.insert(encoded_input.end(), encoded_args.begin(), encoded_args.end());
        return dtvm::call(addr_, encoded_input, call_info->value, call_info->gas);
    }
    virtual dtvm::CResult balanceOf(const dtvm::Address &owner) {
        return balanceOf(dtvm::default_call_info(), owner);
    }
protected:
     dtvm::CResult constructor(dtvm::CallInfoPtr call_info, dtvm::Input &input) override {
        // no need to execute contract constructor since it's just a proxy
        // do nothing
        return dtvm::Revert("unreachable");
     }
private:
    dtvm::Address addr_;
};

std::shared_ptr<ITokenService> ITokenService::from(const dtvm::Address& addr) {
    return std::make_shared<ITokenServiceProxy>(addr);
}

// Since the solidity meta json does not contain inheritance relationships, we cannot directly inherit from interfaces like ITokenService.
// Instead, we implement all interface function signatures based on the Meta json.
class MyToken : public dtvm::Contract {
protected:
    dtvm::StorageSlot balances_slot;
    std::shared_ptr<dtvm::StorageMap<dtvm::Address, dtvm::uint256>> balances_;

    dtvm::StorageSlot token_symbol_slot;
    std::shared_ptr<dtvm::StorageValue<std::string>> token_symbol_;
public:
    inline MyToken() {
        balances_slot = dtvm::StorageSlot(1, 0);
        balances_ = std::make_shared<dtvm::StorageMap<dtvm::Address, dtvm::uint256>>(balances_slot);

        token_symbol_slot = dtvm::StorageSlot(2, 0);
        token_symbol_ = std::make_shared<dtvm::StorageValue<std::string>>(token_symbol_slot);
    }

    // Use public virtual for ordinary non-external interfaces, so they can be called by other functions in derived classes.
public:
    virtual dtvm::CResult callOtherBalance(const dtvm::Address& token, const dtvm::Address& owner) = 0;
    virtual dtvm::CResult callOtherBalance(dtvm::CallInfoPtr call_info, const dtvm::Address& token, const dtvm::Address& owner) {
        return callOtherBalance(token, owner);
    }
    virtual dtvm::CResult balanceOf(const dtvm::Address &owner) = 0;
    virtual dtvm::CResult balanceOf(dtvm::CallInfoPtr call_info, const dtvm::Address &owner) {
        return balanceOf(owner);
    }

protected:
    inline void emitTransfer(dtvm::Address from, dtvm::Address to, dtvm::uint256 amount) {
        const std::vector <uint8_t> topic1 = { 221, 242, 82, 173 }; // 3723645613, Transfer(address,address,uint256)
        const std::vector <uint8_t> data = dtvm::abi_encode(std::make_tuple(from, to, amount));
        dtvm::hostio::emit_log({topic1}, data);
    }

// interface_XXX needs to be protected so that subcontracts can call it as entrypoint
protected:
    // inline dtvm::CResult interface_constructor(dtvm::CallInfoPtr call_info, dtvm::Input &input) {
    //     // payable, support transfer, no need to check transfer amount
    //     std::string arg0 = input.read_string();
    //     return constructor(call_info, arg0);
    // }

    inline dtvm::CResult interface_callOtherBalance(dtvm::CallInfoPtr call_info, dtvm::Input &input) {
        dtvm::require(dtvm::get_msg_value() == 0, "not payable method");
        dtvm::Address arg0 = input.read_address();
        dtvm::Address arg1 = input.read_address();
        return callOtherBalance(call_info, arg0, arg1);
    }

    inline dtvm::CResult interface_balanceOf(dtvm::CallInfoPtr call_info, dtvm::Input &input) {
        dtvm::require(dtvm::get_msg_value() == 0, "not payable method");
        dtvm::Address arg0 = input.read_address();
        return balanceOf(call_info, arg0);
    }

public:
    dtvm::CResult dispatch(dtvm::CallInfoPtr call_info, dtvm::Input &input_with_selector) {
        const uint32_t selector = input_with_selector.read_selector();
        // ignore first 4 bytes selector, read the rest as contract abi input
        dtvm::Input input(input_with_selector.data() + 4, input_with_selector.size() - 4);
        switch (selector) {
            
            case 2326892680: { // selector of callOtherBalance
                return interface_callOtherBalance(call_info, input);
            }
                break;

            case 1889567281: { // selector of balanceOf
                return interface_balanceOf(call_info, input);
            }
                break;

            default: {
                fallback();
                return dtvm::Ok();
            }
        }
    }

    dtvm::CResult dispatch_constructor(dtvm::CallInfoPtr call_info, dtvm::Input &input) {
        return constructor(call_info, input);
    }
};
