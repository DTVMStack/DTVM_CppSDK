#include "contractlib/v1/contractlib.hpp"

// This macro is a declaration that doesn't do anything by itself. 
// The solc compiler and the solidcpp tool in our repository convert it to json and .h header files
SOLIDITY(my_token, R"""(

pragma solidity ^0.8.0;

interface ITokenService {
  function balanceOf(address owner) external returns(uint256);
  function transfer(address to, uint256 amount) external;
}

abstract contract MyToken is ITokenService {
  mapping(address => uint256) private balances;
  string private token_symbol;
  bool private bool_value;
  uint8 private uint8_value;
  int16 private int16_value;
  uint256 private uint256_value;
  bytes private bytes_value;
  bytes32 private bytes32_value;
  uint32[] private uint32_array;
  uint128 private uint128_value;
  int128 private int128_value;

  function callOtherBalance(address token, address owner) public virtual returns (uint256);

  function testGetValue(uint256 value) public virtual payable;

  event Transfer(address from, address to, uint256 amount);
}

)""")

// #include "manual_generated/my_token_decl.hpp"
#include "generated/my_token_decl.hpp"

using namespace dtvm;

class MyTokenImpl : public MyToken {

protected:

    CResult constructor(dtvm::CallInfoPtr call_info, Input &input) override {
        std::string symbol = input.read<std::string>();
        this->token_symbol_->set(symbol);
        balances_->set(get_msg_sender(), uint256(10000000));

        this->uint128_value_->set(123);
        this->bytes_value_->set(Bytes ({ 1, 2, 3, 4 }));
        return Ok();
    }

    CResult test_read_bytes() {
        return Ok(this->bytes_value_->get());
    }

    CResult test_read_u128() {
        return Ok(this->uint128_value_->get());
    }

    CResult callOtherBalance(const Address& token, const Address& owner) override {
        // dtvm::call method can pass gas, value, encoded input, addr etc
        auto token_contract = ITokenService::from(token);
        return token_contract->balanceOf(owner);
    }

    CResult testGetValue(const uint256& value) override {
        const auto& msg_value = get_msg_value();
        if (msg_value != value) {
            return Revert("value not equal");
        }
        return Ok();
    }

    CResult balanceOf(const Address &owner) override {
        uint256 bal = this->balances_->get(owner);
        // emit event demo
        emitTransfer(owner, owner, bal);
        return Ok(bal);
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

public:
    void receive() override {
        // override when receive native token directly(no calldata)
        emitTransfer(Address::zero(), get_msg_sender(), get_msg_value());
    }
};

// generate call and deploy dispatch functions
ENTRYPOINT(MyTokenImpl)
