#!/bin/bash
set -e
rm -f test.db

echo 'testing deploy contract with argument'
# deploy contract (arg 'hello')
/opt/chain_mockcli -f my_token.wasm --action deploy -i 0x0000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000000568656c6c6f000000000000000000000000000000000000000000000000000000

echo 'testing transfer from owner to user2'
# transfer from owner to user2
/opt/chain_mockcli -f my_token.wasm --action call -i 0xa9059cbb0000000000000000000000005b38da6a701c568545dcfcb03fcb875f56beddc40000000000000000000000000000000000000000000000000000000000000001 | grep 'evm finish with result hex: '

echo 'testing balanceOf'
# balanceOf(address)
/opt/chain_mockcli -f my_token.wasm --action call -i 0x70a082310000000000000000000000005b38da6a701c568545dcfcb03fcb875f56beddc4 | grep 'evm finish with result hex: 0000000000000000000000000000000000000000000000000000000000000001'

echo 'testing msg.value' 
# test msg.value testGetValue(uint256) 0xb9aa1f48 with valid value 7
/opt/chain_mockcli -f my_token.wasm --action call -v 7 -i 0xb9aa1f480000000000000000000000000000000000000000000000000000000000000007 | grep 'evm finish with result hex: '

# test msg.value testGetValue(uint256) 0xb9aa1f48 with invalid value
/opt/chain_mockcli -f my_token.wasm --action call -v 7 -i 0xb9aa1f480000000000000000000000000000000000000000000000000000000000000008 | grep 'evm revert with result hex: 000000000000000000000000000000000000000000000000000000000000000f76616c7565206e6f7420657175616c0000000000000000000000000000000000'

echo 'tests success'
