#ifndef PARSER_H
#define PARSER_H

#include "Lexer.h"
#include "AST.h" // Include the main AST definitions
#include <memory>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <string>

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
    std::unique_ptr<ProcedureDeclarationNode> parseProcedureDeclaration();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<PrintNode> parsePrintStatement();
    std::unique_ptr<ReadNode> parseReadStatement();
    std::unique_ptr<IfNode> parseIfStatement();
    std::unique_ptr<WhileNode> parseWhileStatement();
    std::unique_ptr<RepeatNode> parseRepeatStatement();
    std::unique_ptr<ForNode> parseForStatement(); // New function for parsing FOR loops
    std::unique_ptr<ReturnNode> parseReturnStatement();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseComparison();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();
    std::unique_ptr<ASTNode> parsePrimary();
    std::unique_ptr<ASTNode> parsePostfix();
    std::unique_ptr<TypeNode> parseType();
    std::vector<FieldDecl> parseFieldList();
    std::vector<ProcedureParameterNode> parseProcedureParameters();

public:
    Parser(const std::vector<Token> &tokens);
    std::unique_ptr<ProgramNode> parse();

    // دوال مساعدة للتصحيح - تم إصلاحها
    void printAST(const std::unique_ptr<ASTNode> &node, int depth = 0) const;
    void printAST(const ASTNode *node, int depth = 0) const; // نسخة تقبل المؤشر الخام
    void printProgramStructure(const ProgramNode *program) const;

    // دالة مساعدة جديدة للتحويل
};

#endif