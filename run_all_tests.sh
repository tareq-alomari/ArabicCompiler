#!/bin/bash
# Run all test files for the Arabic Compiler
# This tests the complete pipeline on macOS

GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
TESTS_DIR="$SCRIPT_DIR/Tests"
COMPILER="$SCRIPT_DIR/Compiler/ArabicCompiler_mac"

# Test files
TESTS=(
    "test_01_program_structure.arabic"
    "test_02_constants.arabic"
    "test_03_arrays.arabic"
    "test_06_if_else.arabic"
    "test_07_while_loop.arabic"
    "test_08_for_loop.arabic"
    "test_09_repeat_until.arabic"
    "test_10_expressions.arabic"
)

passed=0
failed=0
total=0

echo -e "${BLUE}════════════════════════════════════════════${NC}"
echo -e "${BLUE}   Arabic Compiler - Test Suite (macOS)    ${NC}"
echo -e "${BLUE}════════════════════════════════════════════${NC}"
echo ""

for test in "${TESTS[@]}"; do
    total=$((total + 1))
    test_file="$TESTS_DIR/$test"
    
    if [ ! -f "$test_file" ]; then
        echo -e "${YELLOW}⊘ SKIP${NC} $test (file not found)"
        continue
    fi
    
    echo -ne "${BLUE}Testing${NC} $test ... "
    
    # Compile the test
    cd "$TESTS_DIR"
    output=$("$COMPILER" "$test_file" --c 2>&1)
    status=$?
    
    if [ $status -eq 0 ]; then
        # Try to compile with GCC
        base_name=$(basename "$test_file" .arabic)
        if [ -f "${base_name}_output.c" ]; then
            gcc_output=$(gcc "${base_name}_output.c" -o "${base_name}_test" 2>&1)
            gcc_status=$?
            
            if [ $gcc_status -eq 0 ]; then
                # Run the executable
                run_output=$("./${base_name}_test" 2>&1)
                run_status=$?
                
                if [ $run_status -eq 0 ]; then
                    echo -e "${GREEN}✓ PASS${NC}"
                    passed=$((passed + 1))
                else
                    echo -e "${RED}✗ FAIL${NC} (runtime error)"
                    failed=$((failed + 1))
                fi
                
                # Clean up
                rm -f "${base_name}_test"
            else
                echo -e "${YELLOW}⚠ WARN${NC} (gcc compilation failed)"
                failed=$((failed + 1))
            fi
            
            # Clean up generated files
            rm -f "${base_name}_output.c"
            rm -f "${base_name}_output.asm"
            rm -f "${base_name}_output_intermediate.txt"
        else
            echo -e "${YELLOW}⚠ WARN${NC} (no C output generated)"
            failed=$((failed + 1))
        fi
    else
        echo -e "${RED}✗ FAIL${NC} (compilation error)"
        failed=$((failed + 1))
    fi
done

echo ""
echo -e "${BLUE}════════════════════════════════════════════${NC}"
echo -e "${BLUE}Results:${NC}"
echo -e "  ${GREEN}Passed:${NC} $passed"
echo -e "  ${RED}Failed:${NC} $failed"
echo -e "  ${BLUE}Total:${NC}  $total"

if [ $failed -eq 0 ]; then
    echo -e "\n${GREEN}🎉 All tests passed!${NC}"
    exit 0
else
    echo -e "\n${YELLOW}⚠️  Some tests failed${NC}"
    exit 1
fi

