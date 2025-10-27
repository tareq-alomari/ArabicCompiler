#include "Parser.h"
#include <iostream>
#include <memory>

Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens), current(0) {}

bool Parser::match(TokenType type)
{
    if (isAtEnd())
        return false;
    if (peek().type == type)
    {
        advance();
        return true;
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string &message)
{
    if (check(type))
        return advance();
    std::cout << "[ERR] consume failed: expected=" << Token(type, "", 0, 0).typeToString()
              << ", got=" << peek().typeToString() << " ('" << peek().value << ")"
              << " at line=" << peek().line << ", col=" << peek().column << std::endl;
    throw ParseError(peek(), message);
}

bool Parser::check(TokenType type) const
{
    if (isAtEnd())
        return false;
    return peek().type == type;
}

Token Parser::advance()
{
    if (!isAtEnd())
        current++;
    return previous();
}

bool Parser::isAtEnd() const
{
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() const
{
    if (current >= tokens.size())
        return Token(TokenType::END_OF_FILE, "", 0, 0);
    return tokens[current];
}

Token Parser::previous() const
{
    if (current == 0)
        return Token(TokenType::END_OF_FILE, "", 0, 0);
    return tokens[current - 1];
}

void Parser::synchronize()
{
    advance();

    while (!isAtEnd())
    {
        if (previous().type == TokenType::SEMICOLON)
            return;

        switch (peek().type)
        {
        case TokenType::PROGRAM:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::PRINT:
        case TokenType::VARIABLE:
        case TokenType::CONSTANT:
            return;
        default:
            break;
        }

        advance();
    }
}

std::unique_ptr<ProgramNode> Parser::parse()
{
    try
    {
        std::cout << "[DBG] Parser::parse() starting with " << tokens.size() << " tokens" << std::endl;
        return parseProgram();
    }
    catch (const ParseError &error)
    {
        std::cout << "💥 خطأ في التحليل النحوي: " << error.what()
                  << " في السطر " << error.token.line << std::endl;
        return nullptr;
    }
}

std::unique_ptr<ProgramNode> Parser::parseProgram()
{
    auto program = std::make_unique<ProgramNode>();

    // التحقق من وجود كلمة "برنامج" في البداية
    if (!check(TokenType::PROGRAM))
    {
        throw ParseError(peek(), "توقع كلمة 'برنامج' في بداية البرنامج");
    }
    advance(); // استهلاك كلمة "برنامج"

    // الحصول على اسم البرنامج
    if (!check(TokenType::IDENTIFIER))
    {
        throw ParseError(peek(), "توقع اسم البرنامج بعد 'برنامج'");
    }
    program->name = advance().value;

    std::cout << "[DBG] program name='" << program->name << "' next token="
              << peek().typeToString() << " ('" << peek().value << ")" << std::endl;

    // قبول فاصلة منقوطة اختيارية بعد اسم البرنامج
    if (match(TokenType::SEMICOLON))
    {
        // لا شيء - تم استهلاك الفاصلة
    }

    // تحليل التعريفات والجمل
    while (!isAtEnd() && !check(TokenType::END))
    {
        if (check(TokenType::VARIABLE) || check(TokenType::CONSTANT))
        {
            std::cout << "[DBG] parsing declaration, current token=" << peek().typeToString()
                      << " ('" << peek().value << ")" << std::endl;
            program->declarations.push_back(parseDeclaration());
        }
        else
        {
            std::cout << "[DBG] parsing statement, current token=" << peek().typeToString()
                      << " ('" << peek().value << ") at line=" << peek().line << std::endl;
            program->statements.push_back(parseStatement());
        }
    }

    // قبول كلمة "نهاية" اختيارية في النهاية
    if (match(TokenType::END))
    {
        std::cout << "[DBG] found END keyword" << std::endl;
    }

    return program;
}

std::unique_ptr<ASTNode> Parser::parseDeclaration()
{
    if (match(TokenType::VARIABLE))
    {
        return parseVariableDeclaration();
    }
    else if (match(TokenType::CONSTANT))
    {
        return parseConstantDeclaration();
    }
    else
    {
        throw ParseError(peek(), "توقع تعريف متغير أو ثابت");
    }
}

std::unique_ptr<VariableDeclarationNode> Parser::parseVariableDeclaration()
{
    auto declaration = std::make_unique<VariableDeclarationNode>();

    std::cout << "[DBG] parseVariableDeclaration: expecting IDENTIFIER, got "
              << peek().typeToString() << " ('" << peek().value << ") at line="
              << peek().line << ", col=" << peek().column << std::endl;

    declaration->name = consume(TokenType::IDENTIFIER, "توقع اسم المتغير").value;
    std::cout << "[DBG] name='" << declaration->name << "' next="
              << peek().typeToString() << " ('" << peek().value << ")" << std::endl;

    // نوع اختياري: ": نوع"
    if (match(TokenType::COLON))
    {
        std::cout << "[DBG] saw ':' then token=" << peek().typeToString() << " ('" << peek().value << ")" << std::endl;
        // تخطي النوع
        while (!check(TokenType::ASSIGN) && !check(TokenType::SEMICOLON) && !isAtEnd())
        {
            std::cout << "[DBG] skipping token after ':' -> " << peek().typeToString() << " ('" << peek().value << ")" << std::endl;
            advance();
        }
    }

    if (match(TokenType::ASSIGN))
    {
        declaration->initialValue = parseExpression();
    }

    consume(TokenType::SEMICOLON, "توقع ';' بعد تعريف المتغير");

    return declaration;
}

std::unique_ptr<ConstantDeclarationNode> Parser::parseConstantDeclaration()
{
    auto declaration = std::make_unique<ConstantDeclarationNode>();

    declaration->name = consume(TokenType::IDENTIFIER, "توقع اسم الثابت").value;

    // نوع اختياري للثابت
    if (match(TokenType::COLON))
    {
        while (!check(TokenType::ASSIGN) && !check(TokenType::SEMICOLON) && !isAtEnd())
        {
            advance();
        }
    }

    consume(TokenType::ASSIGN, "توقع '=' بعد اسم الثابت");
    declaration->value = parseExpression();
    consume(TokenType::SEMICOLON, "توقع ';' بعد تعريف الثابت");

    return declaration;
}

std::unique_ptr<ASTNode> Parser::parseStatement()
{
    std::cout << "[DBG] parseStatement at token=" << peek().typeToString()
              << " ('" << peek().value << ") line=" << peek().line << std::endl;

    if (check(TokenType::IDENTIFIER))
    {
        return parseAssignment();
    }
    else if (match(TokenType::PRINT))
    {
        return parsePrintStatement();
    }
    else if (match(TokenType::READ))
    {
        return parseReadStatement();
    }
    else if (match(TokenType::IF))
    {
        return parseIfStatement();
    }
    else if (match(TokenType::WHILE))
    {
        return parseWhileStatement();
    }
    else if (match(TokenType::REPEAT))
    {
        return parseRepeatStatement();
    }
    else if (match(TokenType::SEMICOLON))
    {
        // تجاهل الفواصل المنقوطة الفارغة بين الجمل
        return std::make_unique<VariableNode>("__empty__");
    }
    else
    {
        std::cout << "[DBG] parseStatement no match for token=" << peek().typeToString()
                  << " ('" << peek().value << ") line=" << peek().line << std::endl;
        throw ParseError(peek(), "توقع جملة صالحة");
    }
}

std::unique_ptr<AssignmentNode> Parser::parseAssignment()
{
    auto assignment = std::make_unique<AssignmentNode>();

    assignment->variableName = consume(TokenType::IDENTIFIER, "توقع اسم المتغير").value;
    consume(TokenType::ASSIGN, "توقع '=' في التعيين");
    assignment->value = parseExpression();
    consume(TokenType::SEMICOLON, "توقع ';' بعد الجملة");

    return assignment;
}

std::unique_ptr<PrintNode> Parser::parsePrintStatement()
{
    auto printStmt = std::make_unique<PrintNode>();

    printStmt->expression = parseExpression();
    consume(TokenType::SEMICOLON, "توقع ';' بعد جملة الطباعة");

    return printStmt;
}

std::unique_ptr<ReadNode> Parser::parseReadStatement()
{
    auto readStmt = std::make_unique<ReadNode>();

    // دعم الصيغتين: اقرأ اسم؛ أو اقرأ(اسم)؛
    if (match(TokenType::LPAREN))
    {
        readStmt->variableName = consume(TokenType::IDENTIFIER, "توقع اسم المتغير للقراءة").value;
        consume(TokenType::RPAREN, "توقع ')' بعد اسم المتغير");
    }
    else
    {
        readStmt->variableName = consume(TokenType::IDENTIFIER, "توقع اسم المتغير للقراءة").value;
    }
    consume(TokenType::SEMICOLON, "توقع ';' بعد جملة القراءة");

    return readStmt;
}

std::unique_ptr<IfNode> Parser::parseIfStatement()
{
    auto ifStmt = std::make_unique<IfNode>();
    ifStmt->condition = parseExpression();
    consume(TokenType::THEN, "توقع كلمة 'فان' بعد الشرط");

    while (!check(TokenType::ELSE) && !check(TokenType::END) && !isAtEnd()) {
        ifStmt->thenBranch.push_back(parseStatement());
    }

    IfNode* currentIf = ifStmt.get();

    while (match(TokenType::ELSE)) {
        if (match(TokenType::IF)) {
            // Else if
            auto elseIfNode = std::make_unique<IfNode>();
            elseIfNode->condition = parseExpression();
            consume(TokenType::THEN, "توقع كلمة 'فان' بعد الشرط");

            while (!check(TokenType::ELSE) && !check(TokenType::END) && !isAtEnd()) {
                elseIfNode->thenBranch.push_back(parseStatement());
            }
            
            currentIf->elseBranch.push_back(std::move(elseIfNode));
            currentIf = static_cast<IfNode*>(currentIf->elseBranch.back().get());
        } else {
            // Else
            while (!check(TokenType::END) && !isAtEnd()) {
                currentIf->elseBranch.push_back(parseStatement());
            }
            break; // No more 'else' or 'else if' after a final 'else'
        }
    }

    consume(TokenType::END, "توقع كلمة 'نهاية' لجملة if");
    match(TokenType::SEMICOLON);

    return ifStmt;
}

std::unique_ptr<WhileNode> Parser::parseWhileStatement()
{
    auto whileStmt = std::make_unique<WhileNode>();

    whileStmt->condition = parseExpression();

    // السماح بوجود 'فان' بعد شرط الحلقة
    match(TokenType::THEN);

    // تحليل جسم الحلقة
    while (!check(TokenType::END) && !isAtEnd())
    {
        whileStmt->body.push_back(parseStatement());
    }

    consume(TokenType::END, "توقع كلمة 'نهاية' لجملة while");

    // السماح بشكل "نهاية طالما" مع فاصلة منقوطة
    match(TokenType::WHILE);
    match(TokenType::SEMICOLON);

    return whileStmt;
}

std::unique_ptr<RepeatNode> Parser::parseRepeatStatement()
{
    auto repeatStmt = std::make_unique<RepeatNode>();

    // تحليل جسم الحلقة
    while (!check(TokenType::UNTIL) && !isAtEnd())
    {
        repeatStmt->body.push_back(parseStatement());
    }

    consume(TokenType::UNTIL, "توقع كلمة 'حتى'");
    repeatStmt->condition = parseExpression();
    consume(TokenType::SEMICOLON, "توقع ';' بعد جملة repeat");

    return repeatStmt;
}

std::unique_ptr<ASTNode> Parser::parseExpression()
{
    return parseComparison();
}

std::unique_ptr<ASTNode> Parser::parseComparison()
{
    auto expr = parseTerm();

    while (match(TokenType::EQUALS) || match(TokenType::NOT_EQUALS) ||
           match(TokenType::LESS) || match(TokenType::LESS_EQUAL) ||
           match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL))
    {
        TokenType op = previous().type;
        auto binaryOp = std::make_unique<BinaryOpNode>(op);
        binaryOp->left = std::move(expr);
        binaryOp->right = parseTerm();
        expr = std::move(binaryOp);
    }

    return expr;
}

std::unique_ptr<ASTNode> Parser::parseTerm()
{
    auto expr = parseFactor();

    while (match(TokenType::PLUS) || match(TokenType::MINUS) ||
           match(TokenType::OR))
    {
        TokenType op = previous().type;
        auto binaryOp = std::make_unique<BinaryOpNode>(op);
        binaryOp->left = std::move(expr);
        binaryOp->right = parseFactor();
        expr = std::move(binaryOp);
    }

    return expr;
}

std::unique_ptr<ASTNode> Parser::parseFactor()
{
    auto expr = parsePrimary();

    while (match(TokenType::MULTIPLY) || match(TokenType::DIVIDE) ||
           match(TokenType::MOD) || match(TokenType::AND))
    {
        TokenType op = previous().type;
        auto binaryOp = std::make_unique<BinaryOpNode>(op);
        binaryOp->left = std::move(expr);
        binaryOp->right = parsePrimary();
        expr = std::move(binaryOp);
    }

    return expr;
}

std::unique_ptr<ASTNode> Parser::parsePrimary()
{
    if (match(TokenType::NUMBER) || match(TokenType::REAL_LITERAL) ||
        match(TokenType::STRING_LITERAL))
    {
        auto literal = std::make_unique<LiteralNode>(previous().type, previous().value);
        return literal;
    }

    if (match(TokenType::IDENTIFIER))
    {
        auto variable = std::make_unique<VariableNode>(previous().value);
        return variable;
    }

    if (match(TokenType::LPAREN))
    {
        auto expr = parseExpression();
        consume(TokenType::RPAREN, "توقع ')' بعد التعبير");
        return expr;
    }

    if (match(TokenType::NOT) || match(TokenType::MINUS))
    {
        auto unaryOp = std::make_unique<UnaryOpNode>(previous().type);
        unaryOp->operand = parsePrimary();
        return unaryOp;
    }

    throw ParseError(peek(), "توقع تعبير صالح");
}

// دوال مساعدة للتصحيح - الإصدار المصحح
void Parser::printAST(const std::unique_ptr<ASTNode> &node, int depth) const
{
    if (!node)
        return;

    std::string indent(depth * 2, ' ');
    std::cout << indent << "├─ " << node->toString() << " [" << node->getTypeName() << "]" << std::endl;

    if (auto program = dynamic_cast<ProgramNode *>(node.get()))
    {
        for (const auto &decl : program->declarations)
        {
            printAST(decl, depth + 1);
        }
        for (const auto &stmt : program->statements)
        {
            printAST(stmt, depth + 1);
        }
    }
    else if (auto ifStmt = dynamic_cast<IfNode *>(node.get()))
    {
        printAST(ifStmt->condition, depth + 1);
        for (const auto &stmt : ifStmt->thenBranch)
        {
            printAST(stmt, depth + 1);
        }
        for (const auto &stmt : ifStmt->elseBranch)
        {
            printAST(stmt, depth + 1);
        }
    }
    else if (auto whileStmt = dynamic_cast<WhileNode *>(node.get()))
    {
        printAST(whileStmt->condition, depth + 1);
        for (const auto &stmt : whileStmt->body)
        {
            printAST(stmt, depth + 1);
        }
    }
    else if (auto repeatStmt = dynamic_cast<RepeatNode *>(node.get()))
    {
        for (const auto &stmt : repeatStmt->body)
        {
            printAST(stmt, depth + 1);
        }
        printAST(repeatStmt->condition, depth + 1);
    }
    else if (auto binaryOp = dynamic_cast<BinaryOpNode *>(node.get()))
    {
        printAST(binaryOp->left, depth + 1);
        printAST(binaryOp->right, depth + 1);
    }
    else if (auto unaryOp = dynamic_cast<UnaryOpNode *>(node.get()))
    {
        printAST(unaryOp->operand, depth + 1);
    }
    else if (auto varDecl = dynamic_cast<VariableDeclarationNode *>(node.get()))
    {
        if (varDecl->initialValue)
        {
            printAST(varDecl->initialValue, depth + 1);
        }
    }
    else if (auto constDecl = dynamic_cast<ConstantDeclarationNode *>(node.get()))
    {
        if (constDecl->value)
        {
            printAST(constDecl->value, depth + 1);
        }
    }
    else if (auto assignment = dynamic_cast<AssignmentNode *>(node.get()))
    {
        if (assignment->value)
        {
            printAST(assignment->value, depth + 1);
        }
    }
    else if (auto printStmt = dynamic_cast<PrintNode *>(node.get()))
    {
        if (printStmt->expression)
        {
            printAST(printStmt->expression, depth + 1);
        }
    }
}

void Parser::printProgramStructure(const ProgramNode *program) const
{
    if (!program)
        return;

    std::cout << "\n🌳 هيكل شجرة الاشتقاق النحوي (AST):" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "اسم البرنامج: " << program->name << std::endl;
    std::cout << "عدد التعريفات: " << program->declarations.size() << std::endl;
    std::cout << "عدد الجمل: " << program->statements.size() << std::endl;
    std::cout << "---------------------------------" << std::endl;

    for (const auto &decl : program->declarations)
    {
        std::cout << "تعريف: " << decl->toString() << std::endl;
    }

    for (const auto &stmt : program->statements)
    {
        std::cout << "جملة: " << stmt->toString() << std::endl;
    }
}
