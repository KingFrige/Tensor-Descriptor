#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"

function show_help() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -a, --all     Build and run all tests"
    echo "  -b, --build   Build the project only"
    echo "  -t, --test    Run all tests"
    echo "  -h, --help    Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0            # Show this help"
    echo "  $0 -a         # Build and run all tests"
    echo "  $0 --build    # Build only"
    echo "  $0 -t         # Run tests only"
}

function build() {
    echo "=== Building project ==="
    if [ ! -d "$BUILD_DIR" ]; then
        mkdir -p "$BUILD_DIR"
    fi
    cd "$BUILD_DIR"
    cmake .. && make
    if [ $? -eq 0 ]; then
        echo "=== Build successful ==="
        return 0
    else
        echo "=== Build failed ==="
        return 1
    fi
}

function run_tests() {
    echo ""
    echo "=== Running tests ==="
    cd "$BUILD_DIR"
    
    echo ""
    echo "--- constraint_test ---"
    ./constraint_test
    
    echo ""
    echo "--- batch_constraint_test ---"
    ./batch_constraint_test
    
    echo ""
    echo "=== All tests completed ==="
}

if [ $# -eq 0 ]; then
    show_help
    exit 0
fi

case "$1" in
    -a|--all)
        build
        if [ $? -eq 0 ]; then
            run_tests
        fi
        ;;
    -b|--build)
        build
        ;;
    -t|--test)
        run_tests
        ;;
    -h|--help)
        show_help
        ;;
    *)
        echo "Unknown option: $1"
        show_help
        exit 1
        ;;
esac
