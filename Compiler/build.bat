@echo off & echo Building Arabic Compiler... 
@echo off
echo بناء المترجم العربي...

g++ -std=c++17 -o ArabicCompiler.exe ^
    src\Lexer.cpp ^
    src\Parser.cpp ^
    src\Compiler.cpp ^
    src\main.cpp

if %errorlevel% equ 0 (
    echo ✅ تم بناء المترجم بنجاح!
    echo الملف: ArabicCompiler.exe
) else (
    echo ❌ فشل بناء المترجم
)

pause