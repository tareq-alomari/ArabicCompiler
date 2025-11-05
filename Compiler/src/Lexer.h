#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_map>

enum class TokenType
{
    // الكلمات المحجوزة - البرنامج والكتل
    PROGRAM,
    END,
    PROCEDURE,

    // الكلمات المحجوزة - التعريفات
    VARIABLE,
    CONSTANT,
    TYPE,
    ARRAY,
    RECORD,

    // الكلمات المحجوزة - أنواع البيانات
    INTEGER,
    REAL,
    BOOLEAN,
    STRING,

    // الكلمات المحجوزة - التحكم والشروط
    IF,
    THEN,
    ELSE,
    WHILE,
    REPEAT,
    UNTIL,
    CONTINUE,

    // الكلمات المحجوزة - العمليات
    PRINT,
    READ,
    TO,
    ADD,
    RETURN, // إرجاع

    // الكلمات المحجوزة - معاملات الإجراءات
    BY_VALUE,     // بالقيمة
    BY_REFERENCE, // بالمرجع
    FROM,         // من

    // الكلمات المحجوزة - القيم المنطقية
    TRUE_VAL,  // صح
    FALSE_VAL, // خطأ

    // المعرفات والقيم
    IDENTIFIER,
    NUMBER,
    STRING_LITERAL,
    REAL_LITERAL,
    CHAR_LITERAL,

    // العلامات
    ASSIGN,
    SEMICOLON,
    COMMA,
    COLON,
    DOT,
    LBRACE,
    RBRACE,
    LPAREN,
    RPAREN,
    LBRACKET,
    RBRACKET,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MOD,
    EQUALS,
    NOT_EQUALS,
    LESS,
    GREATER,
    LESS_EQUAL,
    GREATER_EQUAL,
    AND,
    OR,
    NOT,
    END_OF_FILE,
    ERROR
};

struct Token
{
    TokenType type;
    std::string value;
    int line;
    int column;

    Token(TokenType t, const std::string &v, int l, int c)
        : type(t), value(v), line(l), column(c) {}

    std::string typeToString() const
    {
        switch (type)
        {
        case TokenType::PROGRAM:
            return "PROGRAM";
        case TokenType::IF:
            return "IF";
        case TokenType::ELSE:
            return "ELSE";
        case TokenType::PRINT:
            return "PRINT";
        case TokenType::READ:
            return "READ";
        case TokenType::WHILE:
            return "WHILE";
        case TokenType::REPEAT:
            return "REPEAT";
        case TokenType::UNTIL:
            return "UNTIL";
        case TokenType::CONTINUE:
            return "CONTINUE";
        case TokenType::VARIABLE:
            return "VARIABLE";
        case TokenType::CONSTANT:
            return "CONSTANT";
        case TokenType::PROCEDURE:
            return "PROCEDURE";
        case TokenType::INTEGER:
            return "INTEGER";
        case TokenType::REAL:
            return "REAL";
        case TokenType::BOOLEAN:
            return "BOOLEAN";
        case TokenType::STRING:
            return "STRING";
        case TokenType::TYPE:
            return "TYPE";
        case TokenType::ARRAY:
            return "ARRAY";
        case TokenType::RECORD:
            return "RECORD";
        case TokenType::THEN:
            return "THEN";
        case TokenType::END:
            return "END";
        case TokenType::TO:
            return "TO";
        case TokenType::ADD:
            return "ADD";
        case TokenType::IDENTIFIER:
            return "IDENTIFIER";
        case TokenType::NUMBER:
            return "NUMBER";
        case TokenType::STRING_LITERAL:
            return "STRING_LITERAL";
        case TokenType::REAL_LITERAL:
            return "REAL_LITERAL";
        case TokenType::CHAR_LITERAL:
            return "CHAR_LITERAL";
        case TokenType::ASSIGN:
            return "ASSIGN";
        case TokenType::SEMICOLON:
            return "SEMICOLON";
        case TokenType::COMMA:
            return "COMMA";
        case TokenType::COLON:
            return "COLON";
        case TokenType::DOT:
            return "DOT";
        case TokenType::LBRACE:
            return "LBRACE";
        case TokenType::RBRACE:
            return "RBRACE";
        case TokenType::LPAREN:
            return "LPAREN";
        case TokenType::RPAREN:
            return "RPAREN";
        case TokenType::LBRACKET:
            return "LBRACKET";
        case TokenType::RBRACKET:
            return "RBRACKET";
        case TokenType::PLUS:
            return "PLUS";
        case TokenType::MINUS:
            return "MINUS";
        case TokenType::MULTIPLY:
            return "MULTIPLY";
        case TokenType::DIVIDE:
            return "DIVIDE";
        case TokenType::MOD:
            return "MOD";
        case TokenType::EQUALS:
            return "EQUALS";
        case TokenType::NOT_EQUALS:
            return "NOT_EQUALS";
        case TokenType::LESS:
            return "LESS";
        case TokenType::GREATER:
            return "GREATER";
        case TokenType::LESS_EQUAL:
            return "LESS_EQUAL";
        case TokenType::GREATER_EQUAL:
            return "GREATER_EQUAL";
        case TokenType::AND:
            return "AND";
        case TokenType::OR:
            return "OR";
        case TokenType::NOT:
            return "NOT";
        case TokenType::BY_VALUE:
            return "BY_VALUE";
        case TokenType::BY_REFERENCE:
            return "BY_REFERENCE";
        case TokenType::FROM:
            return "FROM";
        case TokenType::TRUE_VAL:
            return "TRUE_VAL";
        case TokenType::FALSE_VAL:
            return "FALSE_VAL";
        case TokenType::END_OF_FILE:
            return "END_OF_FILE";
        case TokenType::ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
        }
    }
};

class Lexer
{
private:
    std::string source;
    size_t position;
    int line;
    int column;
    bool debug;
    std::unordered_map<std::string, TokenType> keywords;

    char peek();
    char advance();
    void skipWhitespace();
    void skipComment();
    Token readNumber();
    Token readString();
    Token readIdentifier();
    bool isArabicChar(char c);
    bool isIdentifierChar(char c);
    void debugChar(char c);
    void analyzeEncoding();

public:
    Lexer(const std::string &source, bool debug = false);
    Token getNextToken();
    std::vector<Token> tokenize();
};

#endif