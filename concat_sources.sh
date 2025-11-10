#!/bin/bash
#
# Script to concatenate all essential source files into a single Markdown file.
# This creates a comprehensive codebase snapshot for AI analysis.
#
OUTPUT_FILE="project_codebase.md"
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "🚀 Starting to concatenate ArabicCompiler project files..."
echo "📁 Project root: $PROJECT_ROOT"
echo "📄 Output file: $OUTPUT_FILE"
echo ""

# Clear the output file
> "$OUTPUT_FILE"

# Add header
cat >> "$OUTPUT_FILE" << 'EOF'
# Arabic Programming Language Compiler - Complete Codebase

This file contains the complete source code of the Arabic Programming Language Compiler project.
Generated for AI analysis and documentation purposes.

**Project**: ArabicCompiler
**Date**: $(date)
**Platform**: Cross-platform (Windows, macOS, Linux)
**Languages**: C++17, C#/.NET 6.0, Bash, PowerShell

---

## Table of Contents

1. Core Compiler (C++)
2. IDE (C#)
3. Build System
4. Tests & Examples
5. Documentation
6. Scripts

---

EOF

# Define the list of essential files and directories to include
INCLUDE_PATTERNS=(
    # Core compiler source
    "Compiler/src/*.cpp"
    "Compiler/src/*.h"
    
    # Build configuration
    "Compiler/CMakeLists.txt"
    "Compiler/build.bat"
    "Compiler/build.sh"
    
    # Tests
    "Compiler/tests/*.cpp"
    
    # IDE source
    "IDE/MainForm.cs"
    "IDE/Program.cs"
    "IDE/ArabicIDE.csproj"
    
    # Examples (limit to .arabic files)
    "Examples/**/*.arabic"
    
    # Test files (sample - not all 50+)
    "Tests/test_01*.arabic"
    "Tests/test_02*.arabic"
    "Tests/test_03*.arabic"
    "Tests/test_06*.arabic"
    "Tests/test_08*.arabic"
    
    # Scripts
    "scripts/*.ps1"
    "scripts/*.sh"
    "compile_arabic.sh"
    "run_all_tests.sh"
    
    # Documentation
    "README.md"
    "README_macOS.md"
    "SETUP_COMPLETE.md"
    "IDE_Research.md"
    "LICENSE"
    ".gitignore"
)

# Function to get language identifier for syntax highlighting
get_language() {
    local file="$1"
    local extension="${file##*.}"
    
    case "$extension" in
        cpp) echo "cpp" ;;
        h)   echo "cpp" ;;
        cs)  echo "csharp" ;;
        yml|yaml) echo "yaml" ;;
        json) echo "json" ;;
        ps1) echo "powershell" ;;
        sh)  echo "bash" ;;
        md)  echo "markdown" ;;
        txt) echo "text" ;;
        csproj) echo "xml" ;;
        arabic) echo "text" ;;
        asm) echo "assembly" ;;
        cmake) echo "cmake" ;;
        bat) echo "batch" ;;
        *) echo "text" ;;
    esac
}

# Function to add a file to the output
add_file() {
    local file="$1"
    local relative_path="${file#$PROJECT_ROOT/}"
    local lang=$(get_language "$file")
    
    echo "  ✓ Adding: $relative_path"
    
    {
        echo ""
        echo "---"
        echo ""
        echo "## 📄 \`$relative_path\`"
        echo ""
        echo "\`\`\`$lang"
        cat "$file"
        echo ""
        echo "\`\`\`"
        echo ""
    } >> "$OUTPUT_FILE"
}

# Process each pattern
cd "$PROJECT_ROOT"

for pattern in "${INCLUDE_PATTERNS[@]}"; do
    # Use find with path matching for better glob support
    while IFS= read -r file; do
        if [ -f "$file" ]; then
            add_file "$file"
        fi
    done < <(find . -path "./$pattern" 2>/dev/null | sort)
done

# Add the grammar rules separately (special handling for Arabic filename)
GRAMMAR_FILE="التقرير (Report)/قواعد اللغة .txt"
if [ -f "$GRAMMAR_FILE" ]; then
    echo "  ✓ Adding: $GRAMMAR_FILE"
    {
        echo ""
        echo "---"
        echo ""
        echo "## 📄 Grammar Rules (قواعد اللغة)"
        echo ""
        echo "\`\`\`text"
        cat "$GRAMMAR_FILE"
        echo ""
        echo "\`\`\`"
        echo ""
    } >> "$OUTPUT_FILE"
fi

# Add footer
cat >> "$OUTPUT_FILE" << 'EOF'

---

## End of Codebase

This consolidated file contains all essential source code for the Arabic Programming Language Compiler.

**Components Included**:
- ✅ Lexer, Parser, AST, Compiler (C++)
- ✅ IDE (C#)
- ✅ Build system (CMake)
- ✅ Test cases
- ✅ Example programs
- ✅ Scripts and documentation

**Usage**: This file can be provided to AI assistants for comprehensive analysis, bug fixing, feature development, or documentation generation.

EOF

# Statistics
FILE_COUNT=$(grep -c "^## 📄" "$OUTPUT_FILE")
LINE_COUNT=$(wc -l < "$OUTPUT_FILE")
SIZE=$(du -h "$OUTPUT_FILE" | cut -f1)

echo ""
echo "✅ Done!"
echo ""
echo "📊 Statistics:"
echo "   Files included: $FILE_COUNT"
echo "   Total lines: $LINE_COUNT"
echo "   File size: $SIZE"
echo ""
echo "📄 Output: $OUTPUT_FILE"
echo ""
echo "🎯 Next steps:"
echo "   1. Review the generated file"
echo "   2. Share with AI assistants for analysis"
echo "   3. Use for documentation or code review"

