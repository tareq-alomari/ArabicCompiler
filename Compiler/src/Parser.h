#ifndef PARSER_H
#define PARSER_H

#include "Lexer.h"
#include <memory>
#include <vector>
#include <stdexcept>

// عقد شجرة الاشتقاق النحوي (AST)
struct ASTNode
{
    virtual ~ASTNode() = default;
    virtual std::string toString() const = 0;
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
};

struct VariableDeclarationNode : public ASTNode
{
    std::string name;
    std::unique_ptr<ASTNode> initialValue;

    std::string toString() const override
    {
        return "تعريف متغير: " + name;
    }
};

struct ConstantDeclarationNode : public ASTNode
{
    std::string name;
    std::unique_ptr<ASTNode> value;

    std::string toString() const override
    {
        return "تعريف ثابت: " + name;
    }
};

struct AssignmentNode : public ASTNode
{
    std::string variableName;
    std::unique_ptr<ASTNode> value;

    std::string toString() const override
    {
        return "تعيين: " + variableName;
    }
};

struct PrintNode : public ASTNode
{
    std::unique_ptr<ASTNode> expression;

    std::string toString() const override
    {
        return "جملة طباعة";
    }
};

struct ReadNode : public ASTNode
{
    std::string variableName;

    std::string toString() const override
    {
        return "جملة قراءة: " + variableName;
    }
};

struct IfNode : public ASTNode
{
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> thenBranch;
    std::vector<std::unique_ptr<ASTNode>> elseBranch;

    std::string toString() const override
    {
        return "جملة شرطية";
    }
};

struct WhileNode : public ASTNode
{
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> body;

    std::string toString() const override
    {
        return "حلقة طالما";
    }
};

struct RepeatNode : public ASTNode
{
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> body;

    std::string toString() const override
    {
        return "حلقة كرر-حتى";
    }
};

struct BinaryOpNode : public ASTNode
{
    TokenType op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    std::string toString() const override
    {
        return "عملية ثنائية";
    }
};

struct UnaryOpNode : public ASTNode
{
    TokenType op;
    std::unique_ptr<ASTNode> operand;

    std::string toString() const override
    {
        return "عملية أحادية";
    }
};

struct LiteralNode : public ASTNode
{
    TokenType type;
    std::string value;

    std::string toString() const override
    {
        return "قيمة ثابتة: " + value;
    }
};

struct VariableNode : public ASTNode
{
    std::string name;

    std::string toString() const override
    {
        return "متغير: " + name;
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

    // دوال مساعدة للتصحيح
    void printAST(const std::unique_ptr<ASTNode> &node, int depth = 0) const;
    void printProgramStructure(const ProgramNode *program) const;
};

#endif