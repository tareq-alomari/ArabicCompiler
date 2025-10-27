#include "Parser.h"
#include <iostream>
#include <memory>

Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens), current(0)
{
}

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
        declaration->typeNode = parseType();
        if (declaration->typeNode)
        {
            std::cout << "[DBG] parsed structured type: '" << declaration->typeNode->toString() << "'" << std::endl;
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
    else if (check(TokenType::REPEAT))
    {
        // Lookahead to distinguish between FOR and REPEAT-UNTIL
        bool isFor = false;
        if (current + 2 < tokens.size() &&
            tokens[current + 1].type == TokenType::IDENTIFIER &&
            tokens[current + 2].type == TokenType::ASSIGN)
        {
            isFor = true;
        }

        if (isFor)
        {
            advance(); // Consume 'كرر'
            return parseForStatement();
        }
        else
        {
            advance(); // Consume 'كرر'
            return parseRepeatStatement();
        }
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

std::unique_ptr<ASTNode> Parser::parseAssignment()
{
    // The left-hand side can be a variable or an array access.
    // We know from parseStatement that the current token is an IDENTIFIER.
    auto var_node = std::make_unique<VariableNode>(consume(TokenType::IDENTIFIER, "خطأ: توقع اسم متغير للتعيين").value);

    std::unique_ptr<ASTNode> left_node;

    // Check if it's an array access
    if (match(TokenType::LBRACKET))
    {
        auto index_access_node = std::make_unique<IndexAccessNode>();
        index_access_node->variable = std::move(var_node);
        index_access_node->index = parseExpression();
        consume(TokenType::RBRACKET, "توقع ']' بعد فهرس القائمة");
        left_node = std::move(index_access_node);
    }
    else
    {
        // It's a simple variable assignment
        left_node = std::move(var_node);
    }

    consume(TokenType::ASSIGN, "توقع '=' في التعيين");
    auto value_node = parseExpression();

    auto assignment = std::make_unique<AssignmentNode>();
    assignment->left = std::move(left_node);
    assignment->value = std::move(value_node);

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

    while (!check(TokenType::ELSE) && !check(TokenType::END) && !isAtEnd())
    {
        ifStmt->thenBranch.push_back(parseStatement());
    }

    IfNode *currentIf = ifStmt.get();

    while (match(TokenType::ELSE))
    {
        if (match(TokenType::IF))
        {
            // Else if
            auto elseIfNode = std::make_unique<IfNode>();
            elseIfNode->condition = parseExpression();
            consume(TokenType::THEN, "توقع كلمة 'فان' بعد الشرط");

            while (!check(TokenType::ELSE) && !check(TokenType::END) && !isAtEnd())
            {
                elseIfNode->thenBranch.push_back(parseStatement());
            }

            currentIf->elseBranch.push_back(std::move(elseIfNode));
            currentIf = static_cast<IfNode *>(currentIf->elseBranch.back().get());
        }
        else
        {
            // Else
            while (!check(TokenType::END) && !isAtEnd())
            {
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

std::unique_ptr<ForNode> Parser::parseForStatement()
{
    auto forNode = std::make_unique<ForNode>();

    // Parse: <iterator> = <start_expr>
    forNode->iteratorName = consume(TokenType::IDENTIFIER, "توقع اسم متغير التكرار في حلقة for").value;
    consume(TokenType::ASSIGN, "توقع '=' بعد اسم متغير التكرار");
    forNode->startValue = parseExpression();

    // Parse: الى <end_expr>
    consume(TokenType::TO, "توقع كلمة 'الى' في حلقة for");
    forNode->endValue = parseExpression();

    // Parse optional step: [اضف <step_expr>]
    if (match(TokenType::ADD))
    {
        forNode->stepValue = parseExpression();
    }

    // Parse loop body until 'نهاية'
    while (!check(TokenType::END) && !isAtEnd())
    {
        forNode->body.push_back(parseStatement());
    }

    consume(TokenType::END, "توقع كلمة 'نهاية' لإنهاء حلقة for");
    match(TokenType::SEMICOLON); // Optional semicolon

    return forNode;
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
        match(TokenType::STRING_LITERAL) || match(TokenType::CHAR_LITERAL))
    {
        auto literal = std::make_unique<LiteralNode>(previous().type, previous().value);
        return literal;
    }

    if (match(TokenType::IDENTIFIER))
    {
        auto var = std::make_unique<VariableNode>(previous().value);
        if (match(TokenType::LBRACKET))
        {
            auto indexAccess = std::make_unique<IndexAccessNode>();
            indexAccess->variable = std::move(var);
            indexAccess->index = parseExpression();
            consume(TokenType::RBRACKET, "توقع ']' بعد فهرس القائمة");
            return indexAccess;
        }
        return var;
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

// Parse a type specification: primitive, array, or record
std::unique_ptr<TypeNode> Parser::parseType()
{
    // Array: قائمة [ number ] من <type>
    if (match(TokenType::ARRAY))
    {
        auto arr = std::make_unique<ArrayTypeNode>();
        consume(TokenType::LBRACKET, "توقع '[' بعد كلمة 'قائمة'");
        Token sizeTok = consume(TokenType::NUMBER, "توقع طول القائمة (عدد)");
        try
        {
            arr->length = std::stoi(sizeTok.value);
        }
        catch (...)
        {
            arr->length = 0;
        }
        consume(TokenType::RBRACKET, "توقع ']' بعد طول القائمة");
        // optional 'من' keyword (lexer may treat it as IDENTIFIER)
        if (check(TokenType::IDENTIFIER) && peek().value == "من")
            advance();
        // element type
        auto elemType = parseType();
        if (elemType)
            arr->elementType = std::move(elemType);
        return arr;
    }

    // Record: سجل { field: type; ... }
    if (match(TokenType::RECORD))
    {
        consume(TokenType::LBRACE, "توقع '{' بعد كلمة 'سجل'");
        auto rec = std::make_unique<RecordTypeNode>();
        while (!check(TokenType::RBRACE) && !isAtEnd())
        {
            // field: IDENTIFIER : type ;
            std::string fieldName = consume(TokenType::IDENTIFIER, "توقع اسم الحقل في السجل").value;
            consume(TokenType::COLON, "توقع ':' بعد اسم الحقل");
            auto fieldType = parseType();
            consume(TokenType::SEMICOLON, "توقع ';' بعد تعريف الحقل");
            FieldDecl fd;
            fd.name = fieldName;
            fd.type = std::move(fieldType);
            rec->fields.push_back(std::move(fd));
        }
        consume(TokenType::RBRACE, "توقع '}' انهاء تعريف السجل");
        return rec;
    }

    // Primitive types or user-defined type names
    if (match(TokenType::INTEGER) || match(TokenType::REAL) || match(TokenType::BOOLEAN) || match(TokenType::STRING))
    {
        auto prim = std::make_unique<PrimitiveTypeNode>(previous().value);
        return prim;
    }

    if (match(TokenType::IDENTIFIER))
    {
        auto prim = std::make_unique<PrimitiveTypeNode>(previous().value);
        return prim;
    }

    // If nothing matched, return nullptr (caller should handle)
    return nullptr;
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
