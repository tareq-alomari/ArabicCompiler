#!/bin/bash
# Arabic Compiler Wrapper Script for macOS
# Usage: ./compile_arabic.sh <file.arabic> [options]

# Set colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
COMPILER="$SCRIPT_DIR/Compiler/build/ArabicCompiler"

# Check if compiler exists
if [ ! -f "$COMPILER" ]; then
    echo -e "${RED}❌ خطأ: المترجم غير موجود في: $COMPILER${NC}"
    echo -e "${YELLOW}يرجى بناء المترجم أولاً باستخدام:${NC}"
    echo "  cd $SCRIPT_DIR/Compiler"
    echo "  ./build.sh"
    exit 1
fi

# Check if input file is provided
if [ $# -eq 0 ]; then
    echo -e "${RED}❌ خطأ: يرجى تحديد ملف المصدر${NC}"
    echo -e "${BLUE}الاستخدام:${NC}"
    echo "  $0 <ملف.arabic> [--c|--asm|--ir|--all]"
    echo ""
    echo -e "${BLUE}أمثلة:${NC}"
    echo "  $0 Tests/test_01_program_structure.arabic --c"
    echo "  $0 Tests/test_08_for_loop.arabic --all"
    echo "  $0 program.arabic --c --run"
    exit 1
fi

INPUT_FILE="$1"
shift
OPTIONS="$@"
RUN_AFTER=false

# Check if --run option is specified
if [[ "$OPTIONS" == *"--run"* ]]; then
    RUN_AFTER=true
    OPTIONS="${OPTIONS//--run/}" # Remove --run from options
fi

# Default to --c if no option specified
if [ -z "$OPTIONS" ]; then
    OPTIONS="--c"
fi

# Get the directory and base name of input file
INPUT_DIR="$(cd "$(dirname "$INPUT_FILE")" && pwd)"
INPUT_BASE="$(basename "$INPUT_FILE" .arabic)"
INPUT_FULL="$INPUT_DIR/$(basename "$INPUT_FILE")"

# Change to input directory for output file generation
cd "$INPUT_DIR" || exit 1

# Run the compiler
echo -e "${BLUE}📝 جاري ترجمة: $INPUT_FILE${NC}"
"$COMPILER" "$INPUT_FULL" $OPTIONS

COMPILE_STATUS=$?

if [ $COMPILE_STATUS -eq 0 ]; then
    echo -e "${GREEN}✅ الترجمة تمت بنجاح!${NC}"
    
    # If --run is specified and C code was generated, compile and run
    if [ "$RUN_AFTER" = true ] && [ -f "${INPUT_BASE}_output.c" ]; then
        echo -e "${BLUE}🔨 جاري بناء الكود...${NC}"
        gcc "${INPUT_BASE}_output.c" -o "${INPUT_BASE}_program"
        
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}✅ البناء تم بنجاح!${NC}"
            echo -e "${BLUE}▶️  جاري تشغيل البرنامج...${NC}"
            echo -e "${YELLOW}================== الإخراج ==================${NC}"
            "./${INPUT_BASE}_program"
            echo -e "${YELLOW}==============================================${NC}"
            
            # Clean up executable
            rm -f "${INPUT_BASE}_program"
        else
            echo -e "${RED}❌ فشل البناء!${NC}"
            exit 1
        fi
    fi
else
    echo -e "${RED}❌ فشلت الترجمة!${NC}"
    exit 1
fi

