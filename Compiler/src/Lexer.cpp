#include "Lexer.h"
#include <cctype>
#include <algorithm>
#include <iostream>
#include <iomanip> // Required for std::hex, std::setw, std::setfill
#include <sstream> // Required for stringstream

// في Lexer.cpp - إصلاح تهيئة الكلمات المحجوزة
Lexer::Lexer(const std::string &source)
    : source(source), position(0), line(1), column(1)
{
    // استخدام UTF-8 للكلمات المحجوزة
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
    std::cout << "طول النص: " << source.length() << " بايت" << std::endl;

    int arabicCount = 0;
    for (size_t i = 0; i < std::min(source.length(), size_t(50)); ++i)
    {
        unsigned char uc = static_cast<unsigned char>(source[i]);
        if ((uc >= 0xC1 && uc <= 0xDA) || (uc >= 0xDC && uc <= 0xEE) || uc == 0xEF)
        {
            arabicCount++;
        }
        if (uc >= 0x80 || (uc < 32 && uc != '\n' && uc != '\t' && uc != '\r'))
        {
            std::cout << "Position " << i << ": Hex=0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(uc)
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
    std::cout << "عدد الأحرف العربية المحتملة المكتشفة (أول 50 بايت): " << arabicCount << std::endl;
    std::cout << "=== انتهى تحليل الترميز ===" << std::endl
              << std::endl;
}

void Lexer::debugChar(char c)
{
    unsigned char uc = static_cast<unsigned char>(c);
    std::cout << "🔍 تصحيح قراءة حرف: Char='";
    if (uc >= 32 && uc < 127)
    {
        std::cout << c;
    }
    else
    {
        std::cout << "?";
    }
    std::cout << "' ASCII=" << static_cast<int>(uc)
              << " Hex=0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(uc) << std::dec
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

        // ✅ إصلاح: تجاهل رموز السطر الجديد بشكل صريح
        if (c == '\n' || c == '\r')
        {
            advance();
            continue;
        }

        if (std::isspace(uc))
        {
            advance();
            continue;
        }
        if (uc == 0xA0) // Windows-1256 NO-BREAK SPACE
        {
            advance();
            continue;
        }
        if (position + 2 < source.length() && uc == 0xEF &&
            static_cast<unsigned char>(source[position + 1]) == 0xBB &&
            static_cast<unsigned char>(source[position + 2]) == 0xBF)
        {
            advance();
            advance();
            advance();
            continue;
        }
        break;
    }
}

void Lexer::skipComment()
{
    if (peek() == '/' && position + 1 < source.length() && source[position + 1] == '/')
    {
        advance();
        advance();
        while (position < source.length() && peek() != '\n')
        {
            advance();
        }
    }
}

bool Lexer::isIdentifierChar(char c)
{
    unsigned char uc = static_cast<unsigned char>(c);

    if (std::isalnum(uc) || uc == '_')
    {
        return true;
    }
    // Windows-1256 Arabic letter ranges
    if ((uc >= 0xC1 && uc <= 0xDA) || (uc >= 0xDC && uc <= 0xEE))
    {
        return true;
    }
    // Explicitly disallow punctuation
    if (uc == 0xBA || uc == 0xAC || uc == 0xBF)
    {
        return false;
    }
    return false;
}

Token Lexer::readNumber()
{
    std::string number_str;
    bool hasDecimal = false;
    size_t startPos = position;
    int startLine = line;
    int startColumn = column;

    while (position < source.length())
    {
        char current_char = peek();
        if (std::isdigit(static_cast<unsigned char>(current_char)))
        {
            number_str += advance();
        }
        else if (current_char == '.')
        {
            if (hasDecimal)
                break;
            if (position + 1 < source.length() && std::isdigit(static_cast<unsigned char>(source[position + 1])))
            {
                hasDecimal = true;
                number_str += advance();
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
    return Token(hasDecimal ? TokenType::REAL_LITERAL : TokenType::NUMBER, number_str, startLine, startColumn);
}

Token Lexer::readString()
{
    std::string str_val;
    int startLine = line;
    int startColumn = column;
    advance(); // Skip opening quote

    while (position < source.length() && peek() != '"')
    {
        char current_char = peek();

        if (current_char == '\\')
        {
            advance(); // Skip backslash
            if (position >= source.length())
            {
                return Token(TokenType::ERROR, "سلسلة نصية غير مكتملة بعد \\", startLine, startColumn);
            }
            char escaped_char = peek();
            switch (escaped_char)
            {
            case 'n':
                str_val += '\n';
                break;
            case 't':
                str_val += '\t';
                break;
            case '"':
                str_val += '"';
                break;
            case '\\':
                str_val += '\\';
                break;
            default:
                str_val += '\\';
                str_val += escaped_char;
                break;
            }
            advance(); // Skip escaped char
        }
        else if (current_char == '\n')
        {
            return Token(TokenType::ERROR, "سطر جديد غير متوقع داخل السلسلة النصية", line, column);
        }
        else
        {
            str_val += advance();
        }
    }

    if (position >= source.length() || peek() != '"')
    {
        return Token(TokenType::ERROR, "سلسلة نصية غير مغلقة", startLine, startColumn);
    }

    advance(); // Skip closing quote
    return Token(TokenType::STRING_LITERAL, str_val, startLine, startColumn);
}

Token Lexer::readIdentifier()
{
    std::string identifier_val;
    int startLine = line;
    int startColumn = column;

    while (position < source.length())
    {
        char c = peek();
        unsigned char uc = static_cast<unsigned char>(c);

        // قبول الأحرف العربية والإنجليزية والأرقام و underscore
        if (std::isalnum(uc) || uc == '_' ||
            (uc >= 0xC0 && uc <= 0xFF) || // النطاق العام للأحرف العربية في UTF-8
            isArabicChar(c))
        {
            identifier_val += advance();
        }
        else
        {
            break;
        }
    }

    // البحث في الكلمات المحجوزة
    auto it = keywords.find(identifier_val);
    if (it != keywords.end())
    {
        return Token(it->second, identifier_val, startLine, startColumn);
    }

    return Token(TokenType::IDENTIFIER, identifier_val, startLine, startColumn);
}

bool Lexer::isArabicChar(char c)
{
    unsigned char uc = static_cast<unsigned char>(c);
    // نطاقات UTF-8 للأحرف العربية
    return (uc >= 0xD8 && uc <= 0xDF) || // Arabic presentation forms
           (uc >= 0xFE && uc <= 0xFF) || // Arabic presentation forms
           (uc >= 0x06 && uc <= 0x06);   // Basic Arabic in UTF-8
}

Token Lexer::getNextToken()
{
    while (position < source.length())
    {
        skipWhitespace();
        skipComment();

        if (position >= source.length())
        {
            return Token(TokenType::END_OF_FILE, "", line, column);
        }

        char current = peek();
        unsigned char ucCurrent = static_cast<unsigned char>(current);
        int tokenStartColumn = column;

        // Handle Windows-1256 specific punctuation FIRST
        if (ucCurrent == 0xBA)
        { // ؛ Arabic Semicolon
            advance();
            return Token(TokenType::SEMICOLON, ";", line, tokenStartColumn);
        }
        if (ucCurrent == 0xAC)
        { // ، Arabic Comma
            advance();
            return Token(TokenType::COMMA, ",", line, tokenStartColumn);
        }
        // if (ucCurrent == 0xBF) { // ؟ Arabic Question Mark - handle if needed }

        // Numbers
        if (std::isdigit(ucCurrent))
        {
            return readNumber();
        }

        // String literals
        if (current == '"')
        {
            return readString();
        }

        // Identifiers and Keywords
        unsigned char ucCurrentForCheck = ucCurrent;
        if (std::isalpha(ucCurrentForCheck) ||
            current == '_' ||
            ((ucCurrentForCheck >= 0xC1 && ucCurrentForCheck <= 0xDA) || (ucCurrentForCheck >= 0xDC && ucCurrentForCheck <= 0xEE)))
        {
            return readIdentifier();
        }

        // Single-character Operators and Punctuation (ASCII range)
        advance(); // Consume the character
        switch (current)
        {
        case '=':
            if (position < source.length() && peek() == '=')
            {
                advance();
                return Token(TokenType::EQUALS, "==", line, tokenStartColumn);
            }
            return Token(TokenType::ASSIGN, "=", line, tokenStartColumn);

        case '!':
            if (position < source.length() && peek() == '=')
            {
                advance();
                return Token(TokenType::NOT_EQUALS, "!=", line, tokenStartColumn);
            }
            return Token(TokenType::NOT, "!", line, tokenStartColumn);

        case '<':
            if (position < source.length() && peek() == '=')
            {
                advance();
                return Token(TokenType::LESS_EQUAL, "<=", line, tokenStartColumn);
            }
            return Token(TokenType::LESS, "<", line, tokenStartColumn);

        case '>':
            if (position < source.length() && peek() == '=')
            {
                advance();
                return Token(TokenType::GREATER_EQUAL, ">=", line, tokenStartColumn);
            }
            return Token(TokenType::GREATER, ">", line, tokenStartColumn);

        case '&':
            if (position < source.length() && peek() == '&')
            {
                advance();
                return Token(TokenType::AND, "&&", line, tokenStartColumn);
            }
            return Token(TokenType::ERROR, "رمز غير متوقع: '&'", line, tokenStartColumn);

        case '|':
            if (position < source.length() && peek() == '|')
            {
                advance();
                return Token(TokenType::OR, "||", line, tokenStartColumn);
            }
            return Token(TokenType::ERROR, "رمز غير متوقع: '|'", line, tokenStartColumn);

        case '+':
            return Token(TokenType::PLUS, "+", line, tokenStartColumn);
        case '-':
            return Token(TokenType::MINUS, "-", line, tokenStartColumn);
        case '*':
            return Token(TokenType::MULTIPLY, "*", line, tokenStartColumn);
        case '/':
            return Token(TokenType::DIVIDE, "/", line, tokenStartColumn);
        case '%':
            return Token(TokenType::MOD, "%", line, tokenStartColumn);
        case ';':
            return Token(TokenType::SEMICOLON, ";", line, tokenStartColumn);
        case ',':
            return Token(TokenType::COMMA, ",", line, tokenStartColumn);
        case ':':
            return Token(TokenType::COLON, ":", line, tokenStartColumn);
        case '.':
            return Token(TokenType::DOT, ".", line, tokenStartColumn);
        case '(':
            return Token(TokenType::LPAREN, "(", line, tokenStartColumn);
        case ')':
            return Token(TokenType::RPAREN, ")", line, tokenStartColumn);
        case '{':
            return Token(TokenType::LBRACE, "{", line, tokenStartColumn);
        case '}':
            return Token(TokenType::RBRACE, "}", line, tokenStartColumn);
        case '[':
            return Token(TokenType::LBRACKET, "[", line, tokenStartColumn);
        case ']':
            return Token(TokenType::RBRACKET, "]", line, tokenStartColumn);

        } // End switch

        // If none matched, it's an unknown character
        std::string errorVal = "رمز غير متوقع: '";
        // *** الإصلاح 4: استخدام اسم المتغير الصحيح ss_err ***
        if (current >= 32 && current < 127)
        {
            errorVal += current;
        }
        else
        {
            std::stringstream ss_err; // Use the correct name
            ss_err << "\\x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(current));
            errorVal += ss_err.str();
        }
        errorVal += "'";
        return Token(TokenType::ERROR, errorVal, line, tokenStartColumn);

    } // End while loop

    return Token(TokenType::END_OF_FILE, "", line, column);
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;
    Token token(TokenType::ERROR, "", 0, 0);

    do
    {
        token = getNextToken();
        tokens.push_back(token);

        if (token.type == TokenType::ERROR)
        {
            std::cerr << "🛑 خطأ في التحليل اللغوي: " << token.value
                      << " في السطر " << token.line << ", العمود التقريبي " << token.column << std::endl;
            break; // Stop on first error
        }
    } while (token.type != TokenType::END_OF_FILE);

    return tokens;
}