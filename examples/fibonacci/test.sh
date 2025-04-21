#!/bin/bash
set -e

rm -f test.db

wasm_file=$1

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

echo 'test deploy contract $wasm_file'
# deploy contract
/opt/chain_mockcli -f $wasm_file --action deploy -s 0x9988776655443322119900112233445566778899 -i 0x

echo 'test fib(10)'

output=$(/opt/chain_mockcli -f $wasm_file --action call -i 0x61047ff4000000000000000000000000000000000000000000000000000000000000000a)
run_cmd_and_grep "$output" 'evm finish with result hex: 0000000000000000000000000000000000000000000000000000000000000037'
