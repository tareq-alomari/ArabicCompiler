#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include "Lexer.h"
#include "Parser.h"
#include "Compiler.h"

// قراءة الملف العربي باستخدام الترميز الصحيح
std::string readFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "خطأ: لا يمكن فتح الملف: " << filename << std::endl;
        throw std::runtime_error("لا يمكن فتح الملف: " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    // محاولة تحويل الترميز إذا كان هناك مشاكل
    if (!content.empty() && static_cast<unsigned char>(content[0]) == 0xEF &&
        content.size() > 2 && static_cast<unsigned char>(content[1]) == 0xBB &&
        static_cast<unsigned char>(content[2]) == 0xBF)
    {
        // إزالة BOM UTF-8 إذا كان موجوداً
        content = content.substr(3);
    }

    return content;
}

void printTokens(const std::vector<Token> &tokens)
{
    std::cout << "🎯 الرموز المميزة:" << std::endl;
    std::cout << "=================" << std::endl;

    for (const auto &token : tokens)
    {
        std::cout << "السطر " << token.line << ", العمود " << token.column << ": "
                  << token.typeToString() << " -> '";

        // طباعة القيمة مع التعامل مع الأحرف غير القابلة للطباعة
        for (char c : token.value)
        {
            if (c >= 32 && c < 127)
            {
                std::cout << c;
            }
            else
            {
                std::cout << "?";
            }
        }
        std::cout << "'" << std::endl;
    }
    std::cout << std::endl;
}

void displayHelp()
{
    std::cout << "🚀 مترجم اللغة العربية - الإصدار المتكامل" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "استخدام: ArabicCompiler <ملف_المصدر> [خيارات]" << std::endl;
    std::cout << std::endl;
    std::cout << "خيارات:" << std::endl;
    std::cout << "  --asm     توليد كود تجميعي (MIPS)" << std::endl;
    std::cout << "  --c       توليد كود C" << std::endl;
    std::cout << "  --ir      توليد كود وسيط" << std::endl;
    std::cout << "  --all     توليد جميع المخرجات" << std::endl;
    std::cout << "  --tokens  عرض الرموز المميزة" << std::endl;
    std::cout << "  --ast     عرض شجرة الاشتقاق النحوي" << std::endl;
    std::cout << "  --help    عرض هذه المساعدة" << std::endl;
    std::cout << std::endl;
    std::cout << "أمثلة:" << std::endl;
    std::cout << "  ArabicCompiler program.arabic --all" << std::endl;
    std::cout << "  ArabicCompiler program.arabic --asm --tokens" << std::endl;
    std::cout << "  ArabicCompiler program.arabic --tokens --ast" << std::endl;
}

int main(int argc, char *argv[])
{
// إعداد الترميز للمخرجات العربية
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

    if (argc < 2 || (argc >= 2 && std::string(argv[1]) == "--help"))
    {
        displayHelp();
        return 0;
    }

    std::string filename = argv[1];
    bool generateAsm = false;
    bool generateC = false;
    bool generateIr = false;
    bool showTokens = false;
    bool showAST = false;

    // معالجة الخيارات
    for (int i = 2; i < argc; i++)
    {
        std::string option = argv[i];
        if (option == "--asm")
            generateAsm = true;
        else if (option == "--c")
            generateC = true;
        else if (option == "--ir")
            generateIr = true;
        else if (option == "--all")
        {
            generateAsm = generateC = generateIr = true;
        }
        else if (option == "--tokens")
            showTokens = true;
        else if (option == "--ast")
            showAST = true;
        else
        {
            std::cerr << "❌ خيار غير معروف: " << option << std::endl;
            displayHelp();
            return 1;
        }
    }

    // إذا لم يتم تحديد أي خيار، استخدم الافتراضي
    if (!generateAsm && !generateC && !generateIr && !showTokens && !showAST)
    {
        generateAsm = true; // الافتراضي: توليد كود التجميع
    }

    try
    {
        std::string sourceCode = readFile(filename);
        std::cout << "📁 جاري تحليل الملف: " << filename << std::endl;
        std::cout << "📏 طول النص: " << sourceCode.length() << " حرف" << std::endl;

        // التحليل اللغوي
        Lexer lexer(sourceCode);
        auto tokens = lexer.tokenize();

        // التحقق من الأخطاء اللغوية
        bool hasLexicalErrors = false;
        for (const auto &token : tokens)
        {
            if (token.type == TokenType::ERROR)
            {
                std::cerr << "❌ خطأ لغوي في السطر " << token.line
                          << ", العمود " << token.column << ": "
                          << token.value << std::endl;
                hasLexicalErrors = true;
            }
        }

        if (hasLexicalErrors)
        {
            std::cerr << "❌ فشل التحليل اللغوي بسبب وجود أخطاء." << std::endl;
            return 1;
        }

        // إذا طلب المستخدم عرض الرموز فقط
        if (showTokens)
        {
            printTokens(tokens);
            if (!generateAsm && !generateC && !generateIr && !showAST)
            {
                std::cout << "✅ عرض الرموز المميزة تم بنجاح!" << std::endl;
                return 0;
            }
        }

        std::cout << "✅ التحليل اللغوي تم بنجاح! (" << tokens.size() << " رمز)" << std::endl;

        // التحليل النحوي
        Parser parser(tokens);
        auto program = parser.parse();

        if (!program)
        {
            std::cerr << "❌ فشل التحليل النحوي." << std::endl;
            return 1;
        }

        std::cout << "✅ التحليل النحوي تم بنجاح!" << std::endl;

        // عرض شجرة الاشتقاق النحوي إذا طلب المستخدم
        if (showAST)
        {
            std::cout << "🌳 شجرة الاشتقاق النحوي (AST):" << std::endl;
            std::cout << "=============================" << std::endl;

            // استخدام الدالة العادية مباشرة
            parser.printAST(std::unique_ptr<ASTNode>(static_cast<ASTNode *>(program.get())));
            parser.printProgramStructure(program.get());

            if (!generateAsm && !generateC && !generateIr)
            {
                std::cout << "✅ عرض الشجرة النحوية تم بنجاح!" << std::endl;
                return 0;
            }
        }
        // الترجمة
        Compiler compiler;
        auto instructions = compiler.compile(std::move(program));

        if (instructions.empty())
        {
            std::cerr << "⚠️  لم يتم توليد أي تعليمات. قد يكون البرنامج فارغاً." << std::endl;
        }

        std::cout << "🔧 جاري توليد المخرجات..." << std::endl;

        // توليد أسماء الملفات الناتجة
        std::string baseName = filename;
        size_t last_dot = filename.find_last_of('.');
        if (last_dot != std::string::npos)
        {
            baseName = filename.substr(0, last_dot);
        }

        // إضافة لاحقة لتجنب الكتابة فوق الملف الأصلي
        baseName = baseName + "_output";

        // توليد المخرجات المطلوبة
        bool generatedAnyOutput = false;

        if (generateIr)
        {
            std::string irFile = baseName + "_intermediate.txt";
            compiler.generateIntermediateCode(irFile);
            std::cout << "📄 تم توليد الكود الوسيط: " << irFile << std::endl;
            generatedAnyOutput = true;
        }

        if (generateC)
        {
            std::string cFile = baseName + ".c";
            compiler.generateCCode(cFile);
            std::cout << "📄 تم توليد كود C: " << cFile << std::endl;
            generatedAnyOutput = true;
        }

        if (generateAsm)
        {
            std::string asmFile = baseName + ".asm";
            compiler.generateAssembly(asmFile);
            std::cout << "📄 تم توليد كود التجميع: " << asmFile << std::endl;
            generatedAnyOutput = true;
        }

        // عرض إحصائيات وتعليمات وسيطة
        if (!instructions.empty())
        {
            compiler.displayInstructions();
        }

        if (generatedAnyOutput)
        {
            std::cout << "🎉 الترجمة تمت بنجاح!" << std::endl;
            std::cout << "📊 الملفات الناتجة:" << std::endl;
            if (generateIr)
                std::cout << "   - " << baseName << "_intermediate.txt (الكود الوسيط)" << std::endl;
            if (generateC)
                std::cout << "   - " << baseName << ".c (كود C)" << std::endl;
            if (generateAsm)
                std::cout << "   - " << baseName << ".asm (كود Assembly)" << std::endl;
        }
        else
        {
            std::cout << "ℹ️  لم يتم توليد أي مخرجات. تحقق من الخيارات المحددة." << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "❌ خطأ استثنائي: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}