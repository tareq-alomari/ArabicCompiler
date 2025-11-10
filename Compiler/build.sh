#!/bin/bash
#
# Arabic Compiler - Build Script (macOS/Linux)
# ===========================================

set -e  # Exit on error

# Colors for output
BLUE='\033[0;34m'
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${BLUE}==========================================="
echo "   مترجم اللغة العربية - بناء المشروع"
echo -e "===========================================${NC}"
echo ""

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo -e "${BLUE}📁 إنشاء مجلد البناء...${NC}"
    mkdir build
fi

cd build

# Configure with CMake
echo -e "${BLUE}⚙️  تكوين المشروع باستخدام CMake...${NC}"
if ! cmake ..; then
    echo -e "${RED}❌ فشل تكوين المشروع!${NC}"
    exit 1
fi

echo ""

# Build the project
echo -e "${BLUE}🔨 بناء المشروع...${NC}"
if cmake --build . --config Release; then
    echo ""
    echo -e "${GREEN}✅ تم بناء المترجم بنجاح!${NC}"
    
    # Determine the executable name based on platform
    if [[ "$OSTYPE" == "darwin"* ]]; then
        EXECUTABLE="ArabicCompiler"
    elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
        EXECUTABLE="ArabicCompiler.exe"
    else
        EXECUTABLE="ArabicCompiler"
    fi
    
    if [ -f "$EXECUTABLE" ]; then
        echo -e "${YELLOW}📁 الملف التنفيذي: build/$EXECUTABLE${NC}"
        
        # Display file information
        FILE_SIZE=$(du -h "$EXECUTABLE" | awk '{print $1}')
        echo -e "${YELLOW}📊 الحجم: $FILE_SIZE${NC}"
        
        # Check if it's a macOS binary
        if [[ "$OSTYPE" == "darwin"* ]]; then
            ARCH=$(file "$EXECUTABLE" | grep -o "arm64\|x86_64" | head -1)
            echo -e "${YELLOW}🏗️  المعمارية: $ARCH${NC}"
        fi
    fi
    
    echo ""
    echo -e "${GREEN}🚀 يمكنك الآن استخدام:${NC}"
    echo -e "   ${YELLOW}./build/$EXECUTABLE <input.arabic>${NC}"
    echo ""
    echo -e "${GREEN}أو استخدم السكريبت المساعد من المجلد الرئيسي:${NC}"
    echo -e "   ${YELLOW}../compile_arabic.sh ../Tests/test_01_program_structure.arabic --c --run${NC}"
    echo ""
else
    echo ""
    echo -e "${RED}❌ فشل بناء المشروع!${NC}"
    exit 1
fi

cd ..

