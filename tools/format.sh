#!/bin/bash

set -e

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( cd "$SCRIPT_DIR/.." && pwd )"

# Function to perform formatting
format() {
    echo "Running solidcpp cargo fmt..."
    cd "$PROJECT_ROOT/solidcpp"
    cargo fmt
    echo "Running solidcpp cargo clippy --fix..."
    cargo clippy --fix --allow-dirty
    cd "$PROJECT_ROOT"


    echo "Running contractlib clang-format..."
    cd "$PROJECT_ROOT/contractlib"
    clang-format -i v1/*.h v1/*.hpp v1/*.cpp
    cd "$PROJECT_ROOT"

    echo "Running cpp_tests clang-format..."
    cd "$PROJECT_ROOT/cpp_tests"
    clang-format -i *.cpp *.hpp
    cd "$PROJECT_ROOT"
}

# Function to perform format checking
check() {
    echo "Running solidcpp cargo fmt --check..."
    cd "$PROJECT_ROOT/solidcpp"
    cargo fmt --all -- --check
    echo "Running solidcpp cargo clippy check..."
    cargo clippy --all-targets --all-features -- -D warnings
    cd "$PROJECT_ROOT"

    echo "Running contractlib make fmt_check..."
    cd "$PROJECT_ROOT/contractlib"
    clang-format --dry-run --Werror v1/*.h v1/*.hpp v1/*.cpp
    cd "$PROJECT_ROOT"

    echo "Running cpp_tests make fmt_check..."
    cd "$PROJECT_ROOT/cpp_tests"
    clang-format --dry-run --Werror *.cpp *.hpp
    cd "$PROJECT_ROOT"
}

# Main script logic
case "$1" in
    "format")
        format
        ;;
    "check")
        check
        ;;
    *)
        echo "Usage: $0 {format|check}"
        echo "  format: Run formatting tools"
        echo "  check:  Run format checking tools"
        exit 1
        ;;
esac
