#include "contractlib/v1/contractlib.hpp"

// Macro to declare, actually does nothing.
// solc+our repository's solidcpp tool is used to convert it into JSON and .h header files
SOLIDITY(my_erc20, R"""(

pragma solidity ^0.8.0;

interface IERC20 {
    function totalSupply() external view returns (uint256);
    function balanceOf(address account) external view returns (uint256);
    function transfer(address recipient, uint256 amount)
        external
        returns (bool);
    
    function allowance(address owner, address spender)
        external
        view
        returns (uint256);
    function approve(address spender, uint256 amount) external returns (bool);
    function transferFrom(address sender, address recipient, uint256 amount)
        external
        returns (bool);
}

abstract contract ERC20 is IERC20 {
    event Transfer(address indexed from, address indexed to, uint256 value);
    event Approval(
        address indexed owner, address indexed spender, uint256 value
    );

    uint256 private totalSupply;
    mapping(address => uint256) private balances;
    
    mapping(address => mapping(address => uint256)) private allowance;

    string public name;
    string public symbol;
    uint8 public decimals;

    function mint(address owner, uint256 amount) public virtual;
}

)""")

#include "generated/my_erc20_decl.hpp"

using namespace dtvm;

class MyErc20Impl : public ERC20 {

protected:

    CResult constructor(dtvm::CallInfoPtr call_info, Input &input) override {
        this->name_->set("TestToken");
        this->symbol_->set("TT");
        this->decimals_->set(uint8_t(4));
        if (input.eof()) {
            this->totalSupply_->set(uint256(0));
        } else {
            uint256 total_supply = input.read<uint256>();
            this->totalSupply_->set(total_supply);

            // mint inital supply to owner
            balances_->set(get_msg_sender(), total_supply);
            emitTransfer(Address::zero(), get_msg_sender(), total_supply);
        }
        return Ok();
    }

    CResult totalSupply() override {
        return Ok(this->totalSupply_->get());
    }

    CResult decimals() override {
        return Ok(this->decimals_->get());
    }

    CResult name() override {
        return Ok(this->name_->get());
    }

    CResult symbol() override {
        return Ok(this->symbol_->get());
    }

    CResult balanceOf(const Address &owner) override {
        uint256 bal = this->balances_->get(owner);
        return Ok(bal);
    }

    CResult mint(const Address& owner, const uint256& amount) override {
        uint256 total_supply = this->totalSupply_->get();
        this->totalSupply_->set(total_supply + amount);
        uint256 owner_bal = this->balances_->get(owner);
        this->balances_->set(owner, owner_bal + amount);
        emitTransfer(Address::zero(), owner, amount);
        return Ok();
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

    CResult approve(const Address &spender, const uint256& amount) override {
        const auto& owner = get_msg_sender();
        uint256 allowance = this->allowance_->get<Address, uint256>(owner, spender);
        this->allowance_->set(owner, spender, allowance + amount);
        emitApproval(owner, spender, amount);
        return Ok();
    }

    CResult transferFrom(const Address &from, const Address &to, const uint256& amount) override {
        const auto& spender = get_msg_sender();
        uint256 allowance = this->allowance_->get<Address, uint256>(from, spender);
        if (allowance < amount) {
            return Revert("allowance not enough");
        }
        this->allowance_->set(from, spender, allowance - amount);

        uint256 from_bal = this->balances_->get(from);
        if (from_bal < amount) {
            return Revert("balance not enough");
        }
        this->balances_->set(from, from_bal - amount);
        uint256 to_bal = this->balances_->get(to);
        this->balances_->set(to, to_bal + amount);
        emitTransfer(from, to, amount);
        return Ok();
    }

    CResult allowance(const Address &owner, const Address &spender) override {
        return Ok(this->allowance_->get<Address, uint256>(owner, spender));
    }
};

// Generate call and deploy dispatch functions
ENTRYPOINT(MyErc20Impl)
