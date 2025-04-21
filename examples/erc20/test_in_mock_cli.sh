#!/bin/bash
set -e

rm -f test.db

echo 'test deploy contract'
# deploy contract (arg total supply(uint256))
/opt/chain_mockcli -f my_erc20.wasm --action deploy -i 0x68656c6c6f000000000000000000000000000000000000000000000000000000
# Provide optional total supply here
# /opt/chain_mockcli -f my_erc20.wasm --action deploy -i 0x

echo 'test mint'
# mint(owner,amount)
/opt/chain_mockcli -f my_erc20.wasm --action call -i 0x40c10f1900000000000000000000000000112233445566778899001122334455667788990000000000000000000000000000000000000000000000000000000000000007

echo 'test balanceOf after mint'
# balanceOf(address) after mint
/opt/chain_mockcli -f my_erc20.wasm --action call -i 0x70a082310000000000000000000000000011223344556677889900112233445566778899 | grep 'evm finish with result hex: 68656c6c6f000000000000000000000000000000000000000000000000000007'

echo 'test transfer from owner to user2'
# transfer from owner to user2
/opt/chain_mockcli -f my_erc20.wasm --action call -i 0xa9059cbb0000000000000000000000005b38da6a701c568545dcfcb03fcb875f56beddc40000000000000000000000000000000000000000000000000000000000000005 | grep 'evm finish with result hex:'

echo 'test query balanceOf after transfer'
# balanceOf(address)
/opt/chain_mockcli -f my_erc20.wasm --action call -i 0x70a082310000000000000000000000005b38da6a701c568545dcfcb03fcb875f56beddc4 | grep 'evm finish with result hex: 0000000000000000000000000000000000000000000000000000000000000005'

# test approve, allowance, transferFrom
echo 'test approve, allowance, transferFrom'

# approve to user2
/opt/chain_mockcli -f my_erc20.wasm --action call -i 0x095ea7b30000000000000000000000005b38da6a701c568545dcfcb03fcb875f56beddc40000000000000000000000000000000000000000000000000000000000000001 | grep 'evm finish with result hex:'

# query allowance to user2 (sender is 0x0011223344556677889900112233445566778899)
/opt/chain_mockcli -f my_erc20.wasm --action call -i 0xdd62ed3e00000000000000000000000000112233445566778899001122334455667788990000000000000000000000005b38da6a701c568545dcfcb03fcb875f56beddc4 | grep 'evm finish with result hex: 0000000000000000000000000000000000000000000000000000000000000001'

# transferFrom from 0x0011223344556677889900112233445566778899 to user3 (send by user2)
/opt/chain_mockcli -f my_erc20.wasm --action call --sender-address-hex 0x5b38da6a701c568545dcfcb03fcb875f56beddc4 -i 0x23b872dd00000000000000000000000000112233445566778899001122334455667788990000000000000000000000005b38da6a701c568545dcfcb03fcb875f56beddc50000000000000000000000000000000000000000000000000000000000000001 | grep 'evm finish with result hex:'

# query balanceOf user3
/opt/chain_mockcli -f my_erc20.wasm --action call -i 0x70a082310000000000000000000000005b38da6a701c568545dcfcb03fcb875f56beddc5 | grep 'evm finish with result hex: 0000000000000000000000000000000000000000000000000000000000000001'

echo 'all tests success'
