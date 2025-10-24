#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer.h"
#include "Parser.h"
#include "Compiler.h"

std::string readFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("لا يمكن فتح الملف: " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void printTokens(const std::vector<Token> &tokens)
{
    std::cout << "🎯 الرموز المميزة:" << std::endl;
    std::cout << "=================" << std::endl;

    for (const auto &token : tokens)
    {
        std::cout << "السطر " << token.line << ", العمود " << token.column << ": "
                  << token.typeToString() << " -> '" << token.value << "'" << std::endl;
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
    std::cout << "  --help    عرض هذه المساعدة" << std::endl;
    std::cout << std::endl;
    std::cout << "أمثلة:" << std::endl;
    std::cout << "  ArabicCompiler program.arabic --all" << std::endl;
    std::cout << "  ArabicCompiler program.arabic --asm --tokens" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc < 2 || std::string(argv[1]) == "--help")
    {
        displayHelp();
        return 0;
    }

    std::string filename = argv[1];
    bool generateAsm = false;
    bool generateC = false;
    bool generateIr = false;
    bool showTokens = false;

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
        else
        {
            std::cerr << "❌ خيار غير معروف: " << option << std::endl;
            displayHelp();
            return 1;
        }
    }

    // إذا لم يتم تحديد أي خيار، استخدم الافتراضي
    if (!generateAsm && !generateC && !generateIr)
    {
        generateAsm = true;
    }

    try
    {
        std::string sourceCode = readFile(filename);
        std::cout << "📁 جاري تحليل الملف: " << filename << std::endl;

        // التحليل اللغوي
        Lexer lexer(sourceCode);
        auto tokens = lexer.tokenize();

        if (showTokens)
        {
            printTokens(tokens);
        }

        // التحقق من الأخطاء اللغوية
        for (const auto &token : tokens)
        {
            if (token.type == TokenType::ERROR)
            {
                std::cerr << "❌ خطأ لغوي في السطر " << token.line
                          << ", العمود " << token.column << ": "
                          << token.value << std::endl;
                return 1;
            }
        }

        std::cout << "✅ التحليل اللغوي تم بنجاح!" << std::endl;

        // التحليل النحوي
        Parser parser(tokens);
        auto program = parser.parse();

        if (!program)
        {
            std::cerr << "❌ خطأ نحوي: فشل في تحليل البرنامج" << std::endl;
            return 1;
        }

        std::cout << "✅ التحليل النحوي تم بنجاح!" << std::endl;

        // الترجمة
        Compiler compiler;
        auto instructions = compiler.compile(std::move(program));

        std::cout << "🔧 جاري توليد المخرجات..." << std::endl;

        // توليد المخرجات المطلوبة
        std::string baseName = filename.substr(0, filename.find_last_of('.'));

        if (generateIr)
        {
            std::string irFile = baseName + "_intermediate.txt";
            compiler.generateIntermediateCode(irFile);
            std::cout << "📄 الكود الوسيط: " << irFile << std::endl;
        }

        if (generateC)
        {
            std::string cFile = baseName + ".c";
            compiler.generateCCode(cFile);
            std::cout << "📄 كود C: " << cFile << std::endl;
        }

        if (generateAsm)
        {
            std::string asmFile = baseName + ".asm";
            compiler.generateAssembly(asmFile);
            std::cout << "📄 كود التجميع: " << asmFile << std::endl;
        }

        // عرض إحصائيات
        compiler.displayInstructions();

        std::cout << "🎉 الترجمة تمت بنجاح!" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "❌ خطأ: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}