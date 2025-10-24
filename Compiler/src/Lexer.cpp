#include "Lexer.h"
#include <cctype>
#include <algorithm>
#include <iostream>
#include <iomanip>

Lexer::Lexer(const std::string &source)
    : source(source), position(0), line(1), column(1)
{
    // تحليل الترميز للأغراض التصحيحية
    analyzeEncoding();

    // تهيئة الكلمات المحجوزة العربية
    keywords = {
        {"برنامج", TokenType::PROGRAM},
        {"اذا", TokenType::IF},
        {"فان", TokenType::THEN},
        {"وإلا", TokenType::ELSE},
        {"اطبع", TokenType::PRINT},
        {"اقرأ", TokenType::READ},
        {"طالما", TokenType::WHILE},
        {"كرر", TokenType::REPEAT},
        {"حتى", TokenType::UNTIL},
        {"نهاية", TokenType::END},
        {"متغير", TokenType::VARIABLE},
        {"ثابت", TokenType::CONSTANT},
        {"إجراء", TokenType::PROCEDURE},
        {"صحيح", TokenType::INTEGER},
        {"حقيقي", TokenType::REAL},
        {"منطقي", TokenType::BOOLEAN},
        {"خيط", TokenType::STRING}};
}

void Lexer::analyzeEncoding()
{
    std::cout << "=== تحليل الترميز ===" << std::endl;
    std::cout << "طول النص: " << source.length() << " حرف" << std::endl;

    int arabicCount = 0;
    for (size_t i = 0; i < std::min(source.length(), size_t(50)); i++)
    {
        unsigned char uc = static_cast<unsigned char>(source[i]);
        if (isArabicChar(source[i]))
        {
            arabicCount++;
        }
        if (uc >= 0x80 || (uc < 32 && uc != '\n' && uc != '\t' && uc != '\r'))
        {
            std::cout << "Position " << i << ": Hex=0x" << std::hex << (int)uc
                      << std::dec << " Char='";
            if (uc >= 32 && uc < 127)
            {
                std::cout << source[i];
            }
            else
            {
                std::cout << "?";
            }
            std::cout << "'" << std::endl;
        }
    }
    std::cout << "عدد الأحرف العربية المكتشفة: " << arabicCount << std::endl;
    std::cout << "=== انتهى تحليل الترميز ===" << std::endl
              << std::endl;
}

void Lexer::debugChar(char c)
{
    unsigned char uc = static_cast<unsigned char>(c);
    std::cout << "🔍 تصحيح: Char='" << c
              << "' ASCII=" << (int)uc
              << " Hex=0x" << std::hex << (int)uc << std::dec
              << " Position=" << position
              << " Line=" << line
              << " Column=" << column << std::endl;
}

char Lexer::peek()
{
    if (position >= source.length())
        return '\0';
    return source[position];
}

char Lexer::advance()
{
    if (position >= source.length())
        return '\0';
    char c = source[position++];
    if (c == '\n')
    {
        line++;
        column = 1;
    }
    else
    {
        column++;
    }
    return c;
}

void Lexer::skipWhitespace()
{
    while (position < source.length())
    {
        char c = peek();
        unsigned char uc = static_cast<unsigned char>(c);

        // تخطي المسافات العادية والتحكم
        if (std::isspace(uc))
        {
            advance();
            continue;
        }

        // تخطي مسافات NO-BREAK SPACE (شائعة في النصوص العربية)
        if (uc == 0xC2 && position + 1 < source.length() &&
            static_cast<unsigned char>(source[position + 1]) == 0xA0)
        {
            advance(); // تخطي 0xC2
            advance(); // تخطي 0xA0
            continue;
        }

        // تخطي مسافات أخرى غير قياسية
        if (uc == 0xA0)
        { // NO-BREAK SPACE في ترميز Windows-1256
            advance();
            continue;
        }

        break;
    }
}

void Lexer::skipComment()
{
    // تخطي التعليقات التي تبدأ بـ //
    if (peek() == '/' && position + 1 < source.length() && source[position + 1] == '/')
    {
        // تخطي //
        advance();
        advance();

        // تخطي بقية السطر
        while (position < source.length() && peek() != '\n')
        {
            advance();
        }
    }
}

bool Lexer::isArabicChar(char c)
{
    unsigned char uc = static_cast<unsigned char>(c);

    // نطاقات Unicode للأحرف العربية
    if ((uc >= 0x0600 && uc <= 0x06FF) || // العربية الأساسية
        (uc >= 0x0750 && uc <= 0x077F) || // العربية الممتدة
        (uc >= 0x08A0 && uc <= 0x08FF) || // العربية الممتدة-ب
        (uc >= 0xFB50 && uc <= 0xFDFF) || // أشكال العرض-أ
        (uc >= 0xFE70 && uc <= 0xFEFF))   // أشكال العرض-ب
    {
        return true;
    }

    // ترميز Windows-1256 (شائع في النصوص العربية)
    if (uc >= 0xC1 && uc <= 0xFE && uc != 0xD7 && uc != 0xF7)
    {
        return true;
    }

    // ترميز ISO-8859-6
    if (uc >= 0xA0 && uc <= 0xFF && uc != 0xA1)
    {
        return true;
    }

    return false;
}

bool Lexer::isIdentifierChar(char c)
{
    unsigned char uc = static_cast<unsigned char>(c);
    return std::isalnum(uc) || c == '_' || isArabicChar(c);
}

Token Lexer::readNumber()
{
    std::string number;
    bool hasDecimal = false;

    while (position < source.length() && (std::isdigit(static_cast<unsigned char>(peek())) || peek() == '.'))
    {
        if (peek() == '.')
        {
            if (hasDecimal)
                break;
            hasDecimal = true;
        }
        number += advance();
    }

    return Token(hasDecimal ? TokenType::REAL_LITERAL : TokenType::NUMBER, number, line, column);
}

Token Lexer::readString()
{
    std::string str;
    advance(); // تخطي الاقتباس الافتتاحي

    while (position < source.length() && peek() != '"' && peek() != '\0')
    {
        if (peek() == '\\')
        {
            advance(); // تخطي الـ backslash
            if (position < source.length())
            {
                switch (peek())
                {
                case 'n':
                    str += '\n';
                    break;
                case 't':
                    str += '\t';
                    break;
                case '"':
                    str += '"';
                    break;
                case '\\':
                    str += '\\';
                    break;
                default:
                    str += '\\';
                    str += peek();
                    break;
                }
                advance();
            }
        }
        else
        {
            str += advance();
        }
    }

    if (position < source.length() && peek() == '"')
    {
        advance(); // تخطي الاقتباس الختامي
        return Token(TokenType::STRING_LITERAL, str, line, column);
    }
    else
    {
        return Token(TokenType::ERROR, "سلسلة نصية غير مكتملة", line, column);
    }
}

Token Lexer::readIdentifier()
{
    std::string identifier;

    // السماح بالأحرف العربية والأرقام وشرطة سفلية
    while (position < source.length() && isIdentifierChar(peek()))
    {
        identifier += advance();
    }

    // التحقق إذا كان معرفًا أو كلمة محجوزة
    auto it = keywords.find(identifier);
    if (it != keywords.end())
    {
        return Token(it->second, identifier, line, column);
    }

    return Token(TokenType::IDENTIFIER, identifier, line, column);
}

Token Lexer::getNextToken()
{
    while (true)
    {
        skipWhitespace();

        if (position >= source.length())
        {
            return Token(TokenType::END_OF_FILE, "", line, column);
        }

        // التحقق من التعليقات
        if (peek() == '/' && position + 1 < source.length() && source[position + 1] == '/')
        {
            skipComment();
            continue;
        }

        char current = peek();

        // طباعة معلومات تصحيح للأحرف المشكوك فيها
        if ((current < 32 || current > 126) && !std::isspace(static_cast<unsigned char>(current)))
        {
            debugChar(current);
        }

        // الأرقام
        if (std::isdigit(static_cast<unsigned char>(current)))
        {
            return readNumber();
        }

        // السلاسل النصية
        if (current == '"')
        {
            return readString();
        }

        // المعرفات والكلمات المحجوزة (بما في ذلك العربية)
        if (std::isalpha(static_cast<unsigned char>(current)) ||
            current == '_' ||
            isArabicChar(current))
        {
            return readIdentifier();
        }

        // الرموز الخاصة
        switch (current)
        {
        case '=':
            advance();
            if (position < source.length() && peek() == '=')
            {
                advance();
                return Token(TokenType::EQUALS, "==", line, column);
            }
            return Token(TokenType::ASSIGN, "=", line, column);

        case '!':
            advance();
            if (position < source.length() && peek() == '=')
            {
                advance();
                return Token(TokenType::NOT_EQUALS, "!=", line, column);
            }
            return Token(TokenType::NOT, "!", line, column);

        case '<':
            advance();
            if (position < source.length() && peek() == '=')
            {
                advance();
                return Token(TokenType::LESS_EQUAL, "<=", line, column);
            }
            return Token(TokenType::LESS, "<", line, column);

        case '>':
            advance();
            if (position < source.length() && peek() == '=')
            {
                advance();
                return Token(TokenType::GREATER_EQUAL, ">=", line, column);
            }
            return Token(TokenType::GREATER, ">", line, column);

        case '&':
            advance();
            if (position < source.length() && peek() == '&')
            {
                advance();
                return Token(TokenType::AND, "&&", line, column);
            }
            break;

        case '|':
            advance();
            if (position < source.length() && peek() == '|')
            {
                advance();
                return Token(TokenType::OR, "||", line, column);
            }
            break;

        case '+':
            advance();
            return Token(TokenType::PLUS, "+", line, column);
        case '-':
            advance();
            return Token(TokenType::MINUS, "-", line, column);
        case '*':
            advance();
            return Token(TokenType::MULTIPLY, "*", line, column);
        case '/':
            advance();
            return Token(TokenType::DIVIDE, "/", line, column);
        case '%':
            advance();
            return Token(TokenType::MOD, "%", line, column);
        case ';':
            advance();
            return Token(TokenType::SEMICOLON, ";", line, column);
        case ',':
            advance();
            return Token(TokenType::COMMA, ",", line, column);
        case ':':
            advance();
            return Token(TokenType::COLON, ":", line, column);
        case '.':
            advance();
            return Token(TokenType::DOT, ".", line, column);
        case '(':
            advance();
            return Token(TokenType::LPAREN, "(", line, column);
        case ')':
            advance();
            return Token(TokenType::RPAREN, ")", line, column);
        case '{':
            advance();
            return Token(TokenType::LBRACE, "{", line, column);
        case '}':
            advance();
            return Token(TokenType::RBRACE, "}", line, column);
        case '[':
            advance();
            return Token(TokenType::LBRACKET, "[", line, column);
        case ']':
            advance();
            return Token(TokenType::RBRACKET, "]", line, column);
        }

        // رمز غير معروف
        char unknownChar = advance();
        std::string errorChar(1, unknownChar);

        return Token(TokenType::ERROR, "رمز غير متوقع: '" + errorChar + "'", line, column);
    }
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;
    Token token = getNextToken();

    while (token.type != TokenType::END_OF_FILE)
    {
        tokens.push_back(token);
        if (token.type == TokenType::ERROR)
        {
            std::cout << "💥 خطأ في التحليل اللغوي: " << token.value << " في السطر " << token.line << std::endl;
            break;
        }
        token = getNextToken();
    }

    return tokens;
}