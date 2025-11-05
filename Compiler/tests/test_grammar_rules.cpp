// اختبارات شاملة لقواعد اللغة العربية
// Comprehensive tests for Arabic language grammar rules

#include <iostream>
#include <cassert>
#include <string>
#include <memory>
#include "../src/Lexer.h"
#include "../src/Parser.h"
#include "../src/Compiler.h"

// Test helper functions
void testLexer(const std::string& code, const std::string& testName) {
    std::cout << "\n=== اختبار Lexer: " << testName << " ===" << std::endl;
    try {
        Lexer lexer(code);
        auto tokens = lexer.tokenize();
        std::cout << "✓ نجح: تم الحصول على " << tokens.size() << " رمز" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ فشل: " << e.what() << std::endl;
    }
}

void testParser(const std::string& code, const std::string& testName) {
    std::cout << "\n=== اختبار Parser: " << testName << " ===" << std::endl;
    try {
        Lexer lexer(code);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        auto program = parser.parse();
        if (program) {
            std::cout << "✓ نجح: تم بناء AST للبرنامج: " << program->name << std::endl;
        } else {
            std::cout << "✗ فشل: لم يتم بناء AST" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "✗ فشل: " << e.what() << std::endl;
    }
}

void testCompiler(const std::string& code, const std::string& testName) {
    std::cout << "\n=== اختبار Compiler: " << testName << " ===" << std::endl;
    try {
        Lexer lexer(code);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        auto program = parser.parse();
        if (program) {
            Compiler compiler;
            auto instructions = compiler.compile(std::move(program));
            std::cout << "✓ نجح: تم توليد " << instructions.size() << " تعليمة" << std::endl;
        } else {
            std::cout << "✗ فشل: لم يتم بناء AST" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "✗ فشل: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  اختبارات شاملة لقواعد اللغة العربية                        ║" << std::endl;
    std::cout << "║  Comprehensive Grammar Rules Tests                         ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;

    // Test 1: Basic Program
    testParser("برنامج اختبار؛", "برنامج بسيط");

    // Test 2: Variable Declaration
    testParser("برنامج اختبار؛ متغير س = 10؛", "تعريف متغير");

    // Test 3: Constant Declaration
    testParser("برنامج اختبار؛ ثابت ط = 3.14؛", "تعريف ثابت");

    // Test 4: Print Statement
    testParser("برنامج اختبار؛ اطبع \"مرحبا\"؛", "جملة طباعة");

    // Test 5: If Statement
    testParser("برنامج اختبار؛ اذا (1 > 0) فان اطبع \"صح\"؛", "جملة شرطية");

    // Test 6: While Loop
    testParser("برنامج اختبار؛ متغير س = 0؛ طالما (س < 10) استمر س = س + 1؛", "حلقة while");

    // Test 7: For Loop
    testParser("برنامج اختبار؛ كرر (س = 1 الى 10) اطبع س؛ نهاية؛", "حلقة for");

    // Test 8: Array Type
    testParser("برنامج اختبار؛ متغير قائمة : قائمة[10] من صحيح؛", "نوع قائمة");

    // Test 9: Record Type
    testParser("برنامج اختبار؛ متغير سجل : سجل { س : صحيح؛ }؛", "نوع سجل");

    // Test 10: Expression with operators
    testParser("برنامج اختبار؛ متغير ن = 5 + 3 * 2؛", "تعبير حسابي");

    std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  انتهت الاختبارات                                          ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;

    return 0;
}

