#!/bin/bash
set -e

rm -f test.db

# Read the input ERC20 wasm file and execute similar test cases as in test_my_token.sh
wasm_file=$1

# If no wasm file path is provided or file does not exist, show error
if [ -z "$wasm_file" ] || [ ! -f "$wasm_file" ]; then
    echo "Usage: $0 <path_to_wasm_file>"
    exit 1
fi


function run_cmd_and_grep() {
    # run command, echo result, and grep $grep. if exit, not run continue
    local exit_code=$?
    local output="$1"
    local grep_pattern="$2"

    # Echo the output
    echo "$output"

    # Check if the command was successful
    if [ $exit_code -ne 0 ]; then
        echo "Command failed with exit code $exit_code"
        exit $exit_code
    fi

    # Check if the output matches the grep pattern
#    echo "$output" | grep -E -zo "$grep_pattern"
    echo "matching pattern: $grep_pattern"
    echo "$output" | awk -v pattern="$grep_pattern" 'BEGIN { RS="\0" } $0 ~ pattern { found=1 } END { if (!found) exit 1 }'
    echo "grep pattern matched"
}

echo 'test deploy ERC20 contract $wasm_file'
# deploy contract (arg total supply(uint256))
/opt/chain_mockcli -f $wasm_file --action deploy -s 0x9988776655443322119900112233445566778899 -i 0x68656c6c6f000000000000000000000000000000000000000000000000000000
# total supply is optional here
# /opt/chain_mockcli -f $wasm_file --action deploy -i 0x
echo 'test totalSupply after deploy erc20'
# query totalSupply()
output=$(/opt/chain_mockcli -f $wasm_file --action call -i 0x18160ddd)
run_cmd_and_grep "$output" 'evm finish with result hex: 68656c6c6f000000000000000000000000000000000000000000000000000000'

echo 'test mint'
# mint(owner,amount)
output=$(/opt/chain_mockcli -f $wasm_file --action call -s 0x9988776655443322119900112233445566778899 -i 0x40c10f1900000000000000000000000000112233445566778899001122334455667788990000000000000000000000000000000000000000000000000000000000000007)
run_cmd_and_grep "$output" 'evm finish with result hex: \ngas used'

echo 'test balanceOf after mint'
# balanceOf(address) after mint 68656c6c6f000000000000000000000000000000000000000000000000000007 when has total_supply big
output=$(/opt/chain_mockcli -f $wasm_file --action call -i 0x70a082310000000000000000000000000011223344556677889900112233445566778899)
run_cmd_and_grep "$output" 'evm finish with result hex: 0000000000000000000000000000000000000000000000000000000000000007'

echo 'test transfer from owner to user2'
# transfer from owner to user2
output=$(/opt/chain_mockcli -f $wasm_file --action call -i 0xa9059cbb0000000000000000000000005b38da6a701c568545dcfcb03fcb875f56beddc40000000000000000000000000000000000000000000000000000000000000005)
run_cmd_and_grep "$output" 'evm finish with result hex:'

echo 'test query balanceOf after transfer'
# balanceOf(address)
output=$(/opt/chain_mockcli -f $wasm_file --action call -i 0x70a082310000000000000000000000005b38da6a701c568545dcfcb03fcb875f56beddc4)
run_cmd_and_grep "$output" 'evm finish with result hex: 0000000000000000000000000000000000000000000000000000000000000005'
