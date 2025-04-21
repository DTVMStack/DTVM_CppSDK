// This is only an example!

#include "hostapi.h"

static uint8_t selector[4];
static uint8_t receiver_addr[32];
static uint8_t msg_caller[32];
static uint8_t transfer_amount[32];
static uint8_t balance_of_owner[32];
static uint8_t tmp_balance[32];

static uint8_t zero_u256[32] = {0};
static uint8_t one_u256[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};

// _balances slot=0
static uint8_t balances_slot[32] = {0};
// _allowances slot=1

// _totalSupply slot=2
static uint8_t total_supploy_slot[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2};

// _name slot=3
// _symbol slot=4

static uint8_t tmp_slot[32] = {};

static uint8_t tmp_bytes64[64] = {};

static void my_memcpy(uint8_t *target, uint8_t *source, uint32_t len) {
    if (len == 32) {
        uint64_t *target_tmp = (uint64_t*) target;
        uint64_t *source_tmp = (uint64_t*) source;
        for (int i=0;i<4;i++) {
            target_tmp[i] = source_tmp[i];
        }
        return;
    }
    if (len == 20) {
        uint64_t *target_tmp = (uint64_t*) target;
        uint64_t *source_tmp = (uint64_t*) source;
        target_tmp[0] = source_tmp[0]; // 8 bytes
        target_tmp[1] = source_tmp[1]; // 8 bytes
        uint32_t *target_last_tmp = (uint32_t*) (target+16);
        uint32_t *source_last_tmp = (uint32_t*) (source+16);
        target_last_tmp[0] = source_last_tmp[0]; // 4 bytes
        return;
    }
    for (int i=0;i<len;i++) {
        target[i] = source[i];
    }
}

// balance of address slot=keccak256(abi.encode(address, _balances_slot))
static void getAddressBalanceSlot(uint8_t *addr_bytes32, uint8_t *target_slot) {
    my_memcpy((uint8_t*) &tmp_bytes64 + 0, addr_bytes32, 32);
    my_memcpy((uint8_t*) &tmp_bytes64 + 32, &balances_slot, 32);
    keccak256((int32_t) &tmp_bytes64, 32+32, (int32_t) &tmp_slot);
}

static uint8_t invalid_selector_error[32] = {
    0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
    0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
    0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
    0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8
};

/**
 *Target behavior is similar to a Solidity contract
 * 
contract erc20 {
    mapping(address=>uint256) public balances;

    // 0x40c10f19
    // demo params
    // 0000000000000000000000005b38da6a701c568545dcfcb03fcb875f56beddc4
    // 000000000000000000000000000000000000000000000000000000000000007b
    function mint(address owner, uint256 amount) public {
        balances[owner] += amount;
    }
    // 0x70a08231
    function balanceOf(address owner) public returns (uint256) {
        return balances[owner];
    }
}
 */

void u256_add(uint8_t *bytes32_amount1, uint8_t *bytes32_amount2, uint8_t *result_bytes32) {
    int prev_overflow = 0; // bool
    for (int i=31;i>=0;i--) {
        int32_t data = (int32_t) bytes32_amount1[i] + (int32_t) bytes32_amount2[i];
        if (prev_overflow) {
            data += 1;
        }
        if (data > 255) {
            prev_overflow = 1;
            data -= 256;
        } else {
            prev_overflow = 0;
        }
        result_bytes32[i] = (uint8_t) data;
    }
}

void u256_sub(uint8_t *bytes32_amount1, uint8_t *bytes32_amount2, uint8_t *result_bytes32) {
    int borrow = 0; // bool
    for (int i = 32 - 1; i >= 0; i--) {
        int diff = bytes32_amount1[i] - bytes32_amount2[i] - borrow;
        borrow = 0;
        if (diff < 0) {
            diff += 256;
            borrow = 1;
        }
        result_bytes32[i] = (uint8_t)diff;
    }
}

void call() {
    // Read 4 bytes from calldata into the selector array
    callDataCopy((int32_t) &selector, 0, 4);

    if (selector[0] == 0x40) {
        // 0x40c10f19, mint(address,uint256)
        callDataCopy((int32_t) &receiver_addr, 4, 32);
        callDataCopy((int32_t) &transfer_amount, 36, 32);

        // get receiver addr slot
        getAddressBalanceSlot(&receiver_addr, &tmp_slot);
        storageLoad((int32_t) &tmp_slot, (int32_t) &tmp_balance);
        u256_add(&tmp_balance, &transfer_amount, &tmp_balance);
        storageStore((int32_t) &tmp_slot, (int32_t) &tmp_balance);

        // add total supply
        // load existed totalSupply
        storageLoad((int32_t) &total_supploy_slot, (int32_t) &tmp_balance);
        u256_add(&tmp_balance, &transfer_amount, &tmp_balance);
        storageStore((int32_t) &total_supploy_slot, (int32_t) &tmp_balance);

        // return
        finish(0, 0);
        return;
    }
    if (selector[0] == 0xa9) {
        // 0xa9059cbb, transfer(address,uint256)
        // mockcli -f erc20.wasm -i 0xa9059cbb0000000000000000000000005b38da6a701c568545dcfcb03fcb875f56beddc500000000000000000000000000000000000000000000000000000000000001e1 -s 0x5b38da6a701c568545dcfcb03fcb875f56beddc4
        // mockcli -f erc20.wasm -i 0x70a082310000000000000000000000005b38da6a701c568545dcfcb03fcb875f56beddc4

        callDataCopy((int32_t) &receiver_addr, 4, 32);
        callDataCopy((int32_t) &transfer_amount, 36, 32);
        // get sender
        getCaller((int32_t) &msg_caller+12);

        // balances[sender] -= amount; balances[to] += amount

        // This is only an example. There is no check for the sender's balance

        // get sender balance slot
        getAddressBalanceSlot(&msg_caller, &tmp_slot);
        // balances[sender] -= amount
        storageLoad((int32_t) &tmp_slot, (int32_t) &tmp_balance);
        u256_sub(&tmp_balance, &transfer_amount, &tmp_balance);
        storageStore((int32_t) &tmp_slot, (int32_t) &tmp_balance);


        // get receiver balance slot
        getAddressBalanceSlot(&receiver_addr, &tmp_slot);
        // balances[to] += amount
        storageLoad((int32_t) &tmp_slot, (int32_t) &tmp_balance);
        u256_add(&tmp_balance, &transfer_amount, &tmp_balance);
        storageStore((int32_t) &tmp_slot, (int32_t) &tmp_balance);

        // return 1
        finish((int32_t) &one_u256, 32);
        return;
    }
    if (selector[0] == 0x70) {
        // 0x70a08231, balanceOf(address)
        // read address
        callDataCopy((int32_t) &balance_of_owner, 4, 32);

        // get address balance slot
        getAddressBalanceSlot(&balance_of_owner, &tmp_slot);
        // load balance bytes
        storageLoad((int32_t) &tmp_slot, (int32_t) &tmp_balance);

        // emit event Balance(address indexed owner, uint256 value);
        // event topic1: 0x134e340554ff8a7d64280a2a28b982df554e2595e5bf45cd39368f31099172a6 (hash of event signature)
        // topic2: balance_of_owner
        // topic3: bytes32 of tmp_balance
        // uint8_t event_topic1[] = {
        //         0x13, 0x4e, 0x34, 0x05, 0x54,0xff, 0x8a, 0x7d,
        //         0x64, 0x28, 0x0a, 0x2a, 0x28, 0xb9, 0x82, 0xdf,
        //         0x55, 0x4e, 0x25, 0x95, 0xe5, 0xbf, 0x45, 0xcd,
        //         0x39, 0x36, 0x8f, 0x31, 0x09, 0x91, 0x72, 0xa6};
        // emitLogEvent((int32_t) &tmp_balance, 32, 2, (int32_t) &event_topic1, (int32_t) &balance_of_owner, 0, 0);

        finish((int32_t) &tmp_balance, 32);
        return;
    } else if (selector[0] == 0x18) {
        // totalSupply() selector 0x18160ddd
        storageLoad((int32_t) &total_supploy_slot, (int32_t) &tmp_balance);
        finish((int32_t) &tmp_balance, 32);
        return;
    }

    // revert if selector not found
    revert((int32_t) &invalid_selector_error, 32);
}

void deploy() {
  // read totalSupply
  callDataCopy((int32_t) &transfer_amount, 0, 32);
  // get sender
  getCaller((int32_t) &msg_caller+12);

  // get owner balance slot
  getAddressBalanceSlot(&msg_caller, &tmp_slot);
  // init balance
  storageStore((int32_t) &tmp_slot, (int32_t) &transfer_amount);

  // set totalSupply
  storageStore((int32_t) &total_supploy_slot, (int32_t) &transfer_amount);
}
