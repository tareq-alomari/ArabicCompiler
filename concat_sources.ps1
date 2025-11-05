#
# Script to concatenate all essential source files into a single Markdown file.
# This creates a comprehensive codebase snapshot for AI analysis.
#
$outputFile = "project_codebase.md"
$projectRoot = $PSScriptRoot

Clear-Host
Write-Host "🚀 Starting to concatenate ArabicCompiler project files..." -ForegroundColor Cyan
Write-Host "📁 Project root: $projectRoot" -ForegroundColor Gray
Write-Host "📄 Output file: $outputFile" -ForegroundColor Gray
Write-Host ""

# Clear the output file
if (Test-Path $outputFile) {
    Clear-Content $outputFile
}

# Add header
@"
# Arabic Programming Language Compiler - Complete Codebase

This file contains the complete source code of the Arabic Programming Language Compiler project.
Generated for AI analysis and documentation purposes.

**Project**: ArabicCompiler
**Date**: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
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

"@ | Add-Content -Path $outputFile

# Define the list of essential files and directories to include
$includePatterns = @(
    # Core compiler source
    "Compiler\src\*.cpp",
    "Compiler\src\*.h",
    
    # Build configuration
    "Compiler\CMakeLists.txt",
    "Compiler\build.bat",
    
    # Tests
    "Compiler\tests\*.cpp",
    
    # IDE source
    "IDE\MainForm.cs",
    "IDE\Program.cs",
    "IDE\ArabicIDE.csproj",
    
    # Examples (limit to .arabic files)
    "Examples\**\*.arabic",
    
    # Test files (sample - not all 50+)
    "Tests\test_01*.arabic",
    "Tests\test_02*.arabic",
    "Tests\test_03*.arabic",
    "Tests\test_06*.arabic",
    "Tests\test_08*.arabic",
    
    # Scripts
    "scripts\*.ps1",
    "scripts\*.sh",
    "compile_arabic.sh",
    "run_all_tests.sh",
    
    # Documentation
    "README.md",
    "README_macOS.md",
    "LICENSE"
)

# Function to get language identifier for syntax highlighting
function Get-Language {
    param($file)
    
    $extension = $file.Extension.TrimStart('.')
    
    switch ($extension) {
        "cpp"    { "cpp" }
        "h"      { "cpp" }
        "cs"     { "csharp" }
        "yml"    { "yaml" }
        "yaml"   { "yaml" }
        "json"   { "json" }
        "ps1"    { "powershell" }
        "sh"     { "bash" }
        "md"     { "markdown" }
        "txt"    { "text" }
        "csproj" { "xml" }
        "arabic" { "text" }
        "asm"    { "assembly" }
        "cmake"  { "cmake" }
        "bat"    { "batch" }
        default  { "text" }
    }
}

# Function to add a file to the output
function Add-FileToOutput {
    param($file)
    
    $relativePath = $file.FullName.Substring($projectRoot.Length + 1).Replace("\", "/")
    $lang = Get-Language -file $file
    
    Write-Host "  ✓ Adding: $relativePath" -ForegroundColor Green
    
    @"

---

## 📄 ``$relativePath``

``````$lang
$(Get-Content $file.FullName -Raw)
``````

"@ | Add-Content -Path $outputFile
}

# Process each pattern
foreach ($pattern in $includePatterns) {
    $fullPattern = Join-Path -Path $projectRoot -ChildPath $pattern
    $files = Get-ChildItem -Path $fullPattern -File -ErrorAction SilentlyContinue | Sort-Object FullName
    
    foreach ($file in $files) {
        Add-FileToOutput -file $file
    }
}

# Add the grammar rules separately (special handling for Arabic filename)
$grammarFile = Join-Path -Path $projectRoot -ChildPath "التقرير (Report)\قواعد اللغة .txt"
if (Test-Path $grammarFile) {
    $file = Get-Item $grammarFile
    Write-Host "  ✓ Adding: Grammar Rules (قواعد اللغة)" -ForegroundColor Green
    
    @"

---

## 📄 Grammar Rules (قواعد اللغة)

``````text
$(Get-Content $grammarFile -Raw)
``````

"@ | Add-Content -Path $outputFile
}

# Add footer
@"

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

"@ | Add-Content -Path $outputFile

# Statistics
$fileCount = (Select-String -Path $outputFile -Pattern "^## 📄" | Measure-Object).Count
$lineCount = (Get-Content $outputFile | Measure-Object -Line).Lines
$size = "{0:N2} MB" -f ((Get-Item $outputFile).Length / 1MB)

Write-Host ""
Write-Host "✅ Done!" -ForegroundColor Green
Write-Host ""
Write-Host "📊 Statistics:" -ForegroundColor Cyan
Write-Host "   Files included: $fileCount" -ForegroundColor Gray
Write-Host "   Total lines: $lineCount" -ForegroundColor Gray
Write-Host "   File size: $size" -ForegroundColor Gray
Write-Host ""
Write-Host "📄 Output: $outputFile" -ForegroundColor Yellow
Write-Host ""
Write-Host "🎯 Next steps:" -ForegroundColor Cyan
Write-Host "   1. Review the generated file" -ForegroundColor Gray
Write-Host "   2. Share with AI assistants for analysis" -ForegroundColor Gray
Write-Host "   3. Use for documentation or code review" -ForegroundColor Gray

