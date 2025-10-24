#ifndef PARSER_H
#define PARSER_H

#include "Lexer.h"
#include <memory>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <string>

// عقد شجرة الاشتقاق النحوي (AST)
struct ASTNode
{
    virtual ~ASTNode() = default;
    virtual std::string toString() const = 0;
    virtual std::string getTypeName() const = 0;
};

struct ProgramNode : public ASTNode
{
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> declarations;
    std::vector<std::unique_ptr<ASTNode>> statements;

    std::string toString() const override
    {
        return "برنامج: " + name;
    }

    std::string getTypeName() const override
    {
        return "ProgramNode";
    }
};

struct VariableDeclarationNode : public ASTNode
{
    std::string name;
    std::unique_ptr<ASTNode> initialValue;

    std::string toString() const override
    {
        std::string result = "تعريف متغير: " + name;
        if (initialValue)
        {
            result += " = [قيمة ابتدائية]";
        }
        return result;
    }

    std::string getTypeName() const override
    {
        return "VariableDeclarationNode";
    }
};

struct ConstantDeclarationNode : public ASTNode
{
    std::string name;
    std::unique_ptr<ASTNode> value;

    std::string toString() const override
    {
        return "تعريف ثابت: " + name + " = [قيمة]";
    }

    std::string getTypeName() const override
    {
        return "ConstantDeclarationNode";
    }
};

struct AssignmentNode : public ASTNode
{
    std::string variableName;
    std::unique_ptr<ASTNode> value;

    std::string toString() const override
    {
        return "تعيين: " + variableName + " = [تعبير]";
    }

    std::string getTypeName() const override
    {
        return "AssignmentNode";
    }
};

struct PrintNode : public ASTNode
{
    std::unique_ptr<ASTNode> expression;

    std::string toString() const override
    {
        return "جملة طباعة: [تعبير]";
    }

    std::string getTypeName() const override
    {
        return "PrintNode";
    }
};

struct ReadNode : public ASTNode
{
    std::string variableName;

    std::string toString() const override
    {
        return "جملة قراءة: " + variableName;
    }

    std::string getTypeName() const override
    {
        return "ReadNode";
    }
};

struct IfNode : public ASTNode
{
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> thenBranch;
    std::vector<std::unique_ptr<ASTNode>> elseBranch;

    std::string toString() const override
    {
        std::string result = "جملة شرطية: ";
        result += "then(" + std::to_string(thenBranch.size()) + " جملة)";
        if (!elseBranch.empty())
        {
            result += ", else(" + std::to_string(elseBranch.size()) + " جملة)";
        }
        return result;
    }

    std::string getTypeName() const override
    {
        return "IfNode";
    }
};

struct WhileNode : public ASTNode
{
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> body;

    std::string toString() const override
    {
        return "حلقة طالما: جسم(" + std::to_string(body.size()) + " جملة)";
    }

    std::string getTypeName() const override
    {
        return "WhileNode";
    }
};

struct RepeatNode : public ASTNode
{
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> body;

    std::string toString() const override
    {
        return "حلقة كرر-حتى: جسم(" + std::to_string(body.size()) + " جملة)";
    }

    std::string getTypeName() const override
    {
        return "RepeatNode";
    }
};

struct BinaryOpNode : public ASTNode
{
    TokenType op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    BinaryOpNode(TokenType operation) : op(operation) {}

    std::string toString() const override
    {
        std::string opStr;
        switch (op)
        {
        case TokenType::PLUS:
            opStr = "+";
            break;
        case TokenType::MINUS:
            opStr = "-";
            break;
        case TokenType::MULTIPLY:
            opStr = "*";
            break;
        case TokenType::DIVIDE:
            opStr = "/";
            break;
        case TokenType::MOD:
            opStr = "%";
            break;
        case TokenType::EQUALS:
            opStr = "==";
            break;
        case TokenType::NOT_EQUALS:
            opStr = "!=";
            break;
        case TokenType::LESS:
            opStr = "<";
            break;
        case TokenType::GREATER:
            opStr = ">";
            break;
        case TokenType::LESS_EQUAL:
            opStr = "<=";
            break;
        case TokenType::GREATER_EQUAL:
            opStr = ">=";
            break;
        case TokenType::AND:
            opStr = "&&";
            break;
        case TokenType::OR:
            opStr = "||";
            break;
        default:
            opStr = "?";
            break;
        }
        return "عملية ثنائية: " + opStr;
    }

    std::string getTypeName() const override
    {
        return "BinaryOpNode";
    }
};

struct UnaryOpNode : public ASTNode
{
    TokenType op;
    std::unique_ptr<ASTNode> operand;

    UnaryOpNode(TokenType operation) : op(operation) {}

    std::string toString() const override
    {
        std::string opStr;
        switch (op)
        {
        case TokenType::NOT:
            opStr = "!";
            break;
        case TokenType::MINUS:
            opStr = "-";
            break;
        default:
            opStr = "?";
            break;
        }
        return "عملية أحادية: " + opStr;
    }

    std::string getTypeName() const override
    {
        return "UnaryOpNode";
    }
};

struct LiteralNode : public ASTNode
{
    TokenType literalType;
    std::string value;

    LiteralNode(TokenType type, const std::string &val)
        : literalType(type), value(val) {}

    std::string toString() const override
    {
        std::string typeStr;
        switch (literalType)
        {
        case TokenType::NUMBER:
            typeStr = "عدد";
            break;
        case TokenType::REAL_LITERAL:
            typeStr = "حقيقي";
            break;
        case TokenType::STRING_LITERAL:
            typeStr = "خيط";
            break;
        default:
            typeStr = "قيمة";
            break;
        }
        return "قيمة ثابتة (" + typeStr + "): " + value;
    }

    std::string getTypeName() const override
    {
        return "LiteralNode";
    }
};

struct VariableNode : public ASTNode
{
    std::string name;

    VariableNode(const std::string &n) : name(n) {}

    std::string toString() const override
    {
        return "متغير: " + name;
    }

    std::string getTypeName() const override
    {
        return "VariableNode";
    }
};

class ParseError : public std::runtime_error
{
public:
    ParseError(const Token &token, const std::string &message)
        : std::runtime_error(message), token(token) {}

    Token token;
};

class Parser
{
private:
    std::vector<Token> tokens;
    size_t current;

    Token advance();
    bool match(TokenType type);
    Token consume(TokenType type, const std::string &message);
    bool check(TokenType type) const;
    Token peek() const;
    Token previous() const;
    bool isAtEnd() const;
    void synchronize();

    // قواعد التحليل
    std::unique_ptr<ProgramNode> parseProgram();
    std::unique_ptr<ASTNode> parseDeclaration();
    std::unique_ptr<VariableDeclarationNode> parseVariableDeclaration();
    std::unique_ptr<ConstantDeclarationNode> parseConstantDeclaration();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<AssignmentNode> parseAssignment();
    std::unique_ptr<PrintNode> parsePrintStatement();
    std::unique_ptr<ReadNode> parseReadStatement();
    std::unique_ptr<IfNode> parseIfStatement();
    std::unique_ptr<WhileNode> parseWhileStatement();
    std::unique_ptr<RepeatNode> parseRepeatStatement();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseComparison();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();
    std::unique_ptr<ASTNode> parsePrimary();

public:
    Parser(const std::vector<Token> &tokens);
    std::unique_ptr<ProgramNode> parse();

    // دوال مساعدة للتصحيح - تم إصلاحها
    void printAST(const std::unique_ptr<ASTNode> &node, int depth = 0) const;
    void printProgramStructure(const ProgramNode *program) const;

    // دالة مساعدة جديدة للتحويل
};

#endif