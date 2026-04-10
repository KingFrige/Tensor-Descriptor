#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"
REPORT_DIR="$BUILD_DIR/valgrind-reports"

function show_help() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -a, --all         Build and run all tests"
    echo "  -b, --build       Build the project only"
    echo "  -t, --test        Run all tests"
    echo "  -v, --valgrind    Run valgrind memory check"
    echo "  -h, --help        Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                # Show this help"
    echo "  $0 -a             # Build and run all tests"
    echo "  $0 --build        # Build only"
    echo "  $0 -t             # Run tests only"
    echo "  $0 -v             # Run valgrind memory check"
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

function run_valgrind() {
    echo ""
    echo "=== Running valgrind memory check ==="
    
    if ! command -v valgrind &> /dev/null; then
        echo "Error: valgrind not found. Please install valgrind first."
        echo "  sudo apt-get install valgrind  # Ubuntu/Debian"
        echo "  brew install valgrind          # macOS"
        return 1
    fi
    
    if [ ! -d "$REPORT_DIR" ]; then
        mkdir -p "$REPORT_DIR"
    fi
    
    cd "$BUILD_DIR"
    
    echo ""
    echo "--- valgrind: constraint_test ---"
    valgrind --leak-check=full \
            --show-leak-kinds=all \
            --track-origins=yes \
            --verbose \
            --error-limit=no \
            --log-file="$REPORT_DIR/constraint_test.log" \
            ./constraint_test 2>&1 | tail -30
    
    echo ""
    echo "--- valgrind: batch_constraint_test ---"
    valgrind --leak-check=full \
            --show-leak-kinds=all \
            --track-origins=yes \
            --verbose \
            --error-limit=no \
            --log-file="$REPORT_DIR/batch_constraint_test.log" \
            ./batch_constraint_test 2>&1 | tail -30
    
    echo ""
    echo "=== Valgrind reports generated ==="
    echo "  $REPORT_DIR/constraint_test.log"
    echo "  $REPORT_DIR/batch_constraint_test.log"
    echo ""
    
    echo "=== Summary ==="
    for log in "$REPORT_DIR"/*.log; do
        if [ -f "$log" ]; then
            echo "--- $(basename "$log") ---"
            grep -E "(LEAK SUMMARY|definitely lost|indirectly lost|possibly lost|ERROR SUMMARY)" "$log" | head -10
            echo ""
        fi
    done
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
    -v|--valgrind)
        run_valgrind
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
