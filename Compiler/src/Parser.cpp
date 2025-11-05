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
    while (!isAtEnd() && !check(TokenType::END) && !check(TokenType::DOT))
    {
        // تحديد ما إذا كانت جملة تعريف أم جملة عادية
        // التعريفات تبدأ بـ: متغير، ثابت، نوع، إجراء
        // الجمل العادية تبدأ بـ: معرف، اطبع، اقرا، اذا، طالما، كرر، إرجاع

        if (check(TokenType::VARIABLE) || check(TokenType::CONSTANT) ||
            check(TokenType::TYPE) || check(TokenType::PROCEDURE))
        {
            std::cout << "[DBG] parsing declaration, current token=" << peek().typeToString()
                      << " ('" << peek().value << ")" << std::endl;
            auto decl = parseDeclaration();
            if (decl)
            {
                program->declarations.push_back(std::move(decl));
            }
            else
            {
                // إذا كانت النتيجة nullptr، فهذا جملة تعيين، معالجتها كجملة
                std::cout << "[DBG] declaration returned nullptr, treating as statement" << std::endl;
                program->statements.push_back(parseStatement());
            }
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

    // قبول النقطة الأخيرة (نهاية البرنامج)
    if (match(TokenType::DOT))
    {
        std::cout << "[DBG] found DOT (end of program)" << std::endl;
    }

    return program;
}

std::unique_ptr<ASTNode> Parser::parseDeclaration()
{
    if (match(TokenType::VARIABLE))
    {
        // معالجة متغيرات متعددة
        auto firstDecl = parseVariableDeclaration();

        // إذا كانت النتيجة nullptr، فهذا ليس تعريف متغير
        if (!firstDecl)
        {
            return nullptr;
        }

        // إذا كان هناك متغيرات إضافية، نتجاهلها ونعود بالأول
        // لكن نتوقف إذا رأينا LBRACKET أو DOT (علامات جملة تعيين)
        while ((check(TokenType::IDENTIFIER) || check(TokenType::BOOLEAN) ||
                check(TokenType::INTEGER) || check(TokenType::REAL) ||
                check(TokenType::STRING)) &&
               !check(TokenType::LBRACKET) &&
               !check(TokenType::DOT) && !check(TokenType::PROCEDURE) &&
               !check(TokenType::CONSTANT) && !check(TokenType::TYPE) &&
               !check(TokenType::END) && !isAtEnd())
        {
            auto nextDecl = parseVariableDeclaration();
            if (!nextDecl) break; // إذا لم يكن تعريفاً، توقف
        }

        return firstDecl;
    }
    else if (match(TokenType::CONSTANT))
    {
        return parseConstantDeclaration();
    }
    else if (match(TokenType::TYPE))
    {
        // Type definition: نوع <name> = <type> ;
        std::string typeName = consume(TokenType::IDENTIFIER, "توقع اسم النوع").value;
        consume(TokenType::ASSIGN, "توقع '=' بعد اسم النوع");
        auto typeNode = parseType();
        consume(TokenType::SEMICOLON, "توقع ';' بعد تعريف النوع");
        // For now, we'll create a variable declaration node to represent type definitions
        // This could be extended with a TypeDefinitionNode if needed
        auto typeDecl = std::make_unique<VariableDeclarationNode>();
        typeDecl->name = typeName;
        typeDecl->typeNode = std::move(typeNode);
        return typeDecl;
    }
    else if (match(TokenType::PROCEDURE))
    {
        return parseProcedureDeclaration();
    }
    else
    {
        throw ParseError(peek(), "توقع تعريف متغير أو ثابت أو نوع أو إجراء");
    }
}

std::unique_ptr<VariableDeclarationNode> Parser::parseVariableDeclaration()
{
    auto declaration = std::make_unique<VariableDeclarationNode>();

    std::cout << "[DBG] parseVariableDeclaration: expecting IDENTIFIER, got "
              << peek().typeToString() << " ('" << peek().value << ") at line="
              << peek().line << ", col=" << peek().column << std::endl;

    // السماح بالكلمات المحجوزة كأسماء متغيرات (مثل: منطقي، صحيح، إلخ)
    if (check(TokenType::IDENTIFIER))
    {
        declaration->name = advance().value;
    }
    else if (check(TokenType::BOOLEAN) || check(TokenType::INTEGER) ||
             check(TokenType::REAL) || check(TokenType::STRING))
    {
        // السماح بأسماء أنواع البيانات كأسماء متغيرات
        declaration->name = advance().value;
    }
    else
    {
        throw ParseError(peek(), "توقع اسم المتغير");
    }

    std::cout << "[DBG] name='" << declaration->name << "' next="
              << peek().typeToString() << " ('" << peek().value << ")" << std::endl;

    // إذا رأينا LBRACKET أو DOT بدلاً من COLON، فهذا ليس تعريف متغير
    // بل هو جملة تعيين، لذا نرجع nullptr للإشارة إلى أن هذا ليس تعريفاً
    if (check(TokenType::LBRACKET) || check(TokenType::DOT))
    {
        std::cout << "[DBG] Not a variable declaration, looks like assignment" << std::endl;
        // نعود للخلف بخطوة واحدة
        current--;
        return nullptr;
    }

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

    // تجاهل الفاصلة المنقوطة إذا كانت موجودة
    if (check(TokenType::SEMICOLON))
    {
        advance();
    }

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

    // السماح بالكلمات المحجوزة كأسماء متغيرات في جمل التعيين
    if (check(TokenType::IDENTIFIER) || check(TokenType::BOOLEAN) ||
        check(TokenType::INTEGER) || check(TokenType::REAL) ||
        check(TokenType::STRING))
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
        // الصيغة: كرر (ع = 1 الى 5) أو كرر (ع = 1 الى 5) أو اعد ... حتى
        bool isFor = false;
        size_t lookAheadPos = current + 1;

        // تخطي القوس اليمين إذا كان موجوداً
        if (lookAheadPos < tokens.size() && tokens[lookAheadPos].type == TokenType::LPAREN)
        {
            lookAheadPos++;
        }

        // التحقق من: IDENTIFIER =
        if (lookAheadPos + 1 < tokens.size() &&
            tokens[lookAheadPos].type == TokenType::IDENTIFIER &&
            tokens[lookAheadPos + 1].type == TokenType::ASSIGN)
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
    else if (match(TokenType::RETURN))
    {
        return parseReturnStatement();
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
    // The left-hand side can be a variable, an array access, or a procedure call.
    // We know from parseStatement that the current token is an IDENTIFIER or a reserved word used as a variable name.
    std::string identifier;
    if (check(TokenType::IDENTIFIER))
    {
        identifier = advance().value;
    }
    else if (check(TokenType::BOOLEAN) || check(TokenType::INTEGER) ||
             check(TokenType::REAL) || check(TokenType::STRING))
    {
        // السماح بأسماء أنواع البيانات كأسماء متغيرات
        identifier = advance().value;
    }
    else
    {
        throw ParseError(peek(), "خطأ: توقع اسم متغير أو إجراء");
    }

    // Check if it's a procedure call (identifier followed by LPAREN)
    if (check(TokenType::LPAREN))
    {
        advance(); // Consume '('
        auto procCall = std::make_unique<ProcedureCallNode>(identifier);

        // Parse procedure call arguments
        if (!check(TokenType::RPAREN))
        {
            do
            {
                procCall->arguments.push_back(parseExpression());
            } while (match(TokenType::COMMA));
        }

        consume(TokenType::RPAREN, "توقع ')' بعد معاملات الإجراء");
        consume(TokenType::SEMICOLON, "توقع ';' بعد استدعاء الإجراء");
        return procCall;
    }

    auto var_node = std::make_unique<VariableNode>(identifier);
    std::unique_ptr<ASTNode> left_node;

    // Check if it's a record field access (dot notation)
    if (check(TokenType::DOT))
    {
        advance(); // Consume '.'
        std::string fieldName = consume(TokenType::IDENTIFIER, "توقع اسم الحقل بعد النقطة").value;
        left_node = std::make_unique<VariableNode>(identifier + "." + fieldName);
    }
    // Check if it's an array access
    else if (match(TokenType::LBRACKET))
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

    // قد يكون هناك قوس يمين اختياري: كرر (ع = 1 الى 5)
    if (check(TokenType::LPAREN))
    {
        advance(); // استهلاك '('
    }

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

    // قد يكون هناك قوس يسار اختياري
    if (check(TokenType::RPAREN))
    {
        advance(); // استهلاك ')'
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
    auto expr = parsePostfix();

    while (match(TokenType::MULTIPLY) || match(TokenType::DIVIDE) ||
           match(TokenType::MOD) || match(TokenType::AND))
    {
        TokenType op = previous().type;
        auto binaryOp = std::make_unique<BinaryOpNode>(op);
        binaryOp->left = std::move(expr);
        binaryOp->right = parsePostfix();
        expr = std::move(binaryOp);
    }

    return expr;
}

std::unique_ptr<ASTNode> Parser::parsePrimary()
{
    // Handle numeric and string literals
    if (match(TokenType::NUMBER) || match(TokenType::REAL_LITERAL) ||
        match(TokenType::STRING_LITERAL) || match(TokenType::CHAR_LITERAL))
    {
        auto literal = std::make_unique<LiteralNode>(previous().type, previous().value);
        return literal;
    }

    // Handle boolean literals (صح = true, خطأ = false)
    if (match(TokenType::TRUE_VAL))
    {
        auto literal = std::make_unique<LiteralNode>(TokenType::TRUE_VAL, "صح");
        return literal;
    }

    if (match(TokenType::FALSE_VAL))
    {
        auto literal = std::make_unique<LiteralNode>(TokenType::FALSE_VAL, "خطأ");
        return literal;
    }

    // Handle identifiers and array access
    // السماح بالكلمات المحجوزة كأسماء متغيرات (مثل: منطقي، صحيح، إلخ)
    std::string varName;
    if (check(TokenType::IDENTIFIER))
    {
        varName = advance().value;
    }
    else if (check(TokenType::BOOLEAN) || check(TokenType::INTEGER) ||
             check(TokenType::REAL) || check(TokenType::STRING))
    {
        // السماح بأسماء أنواع البيانات كأسماء متغيرات
        varName = advance().value;
    }

    if (!varName.empty())
    {
        auto var = std::make_unique<VariableNode>(varName);
        if (match(TokenType::LBRACKET))
        {
            auto indexAccess = std::make_unique<IndexAccessNode>();
            indexAccess->variable = std::move(var);
            indexAccess->index = parseExpression();
            consume(TokenType::RBRACKET, "توقع ']' بعد فهرس القائمة");
            return indexAccess;
        }
        // Handle record field access (dot notation)
        if (match(TokenType::DOT))
        {
            std::string fieldName = consume(TokenType::IDENTIFIER, "توقع اسم الحقل بعد النقطة").value;
            // For now, we'll create a special variable node for field access
            // This could be extended with a FieldAccessNode if needed
            auto fieldVar = std::make_unique<VariableNode>(var->name + "." + fieldName);
            return fieldVar;
        }
        return var;
    }

    // Handle parenthesized expressions
    if (match(TokenType::LPAREN))
    {
        auto expr = parseExpression();
        consume(TokenType::RPAREN, "توقع ')' بعد التعبير");
        return expr;
    }

    // Handle unary operators
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
        // Handle 'من' keyword (from)
        if (match(TokenType::FROM))
        {
            // 'من' keyword found, continue to parse element type
        }
        else if (check(TokenType::IDENTIFIER) && peek().value == "من")
        {
            // Fallback: if 'من' was not recognized as FROM token
            advance();
        }
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

            // الفاصلة المنقوطة اختيارية بعد الحقل الأخير
            if (check(TokenType::SEMICOLON))
            {
                advance();
            }

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
    printAST(node.get(), depth);
}

// نسخة تقبل المؤشر الخام - هذه هي النسخة الرئيسية
void Parser::printAST(const ASTNode *node, int depth) const
{
    if (!node)
        return;

    std::string indent(depth * 2, ' ');
    std::cout << indent << "├─ " << node->toString() << " [" << node->getTypeName() << "]" << std::endl;

    if (auto program = dynamic_cast<const ProgramNode *>(node))
    {
        for (const auto &decl : program->declarations)
        {
            printAST(decl.get(), depth + 1);
        }
        for (const auto &stmt : program->statements)
        {
            printAST(stmt.get(), depth + 1);
        }
    }
    else if (auto ifStmt = dynamic_cast<const IfNode *>(node))
    {
        printAST(ifStmt->condition.get(), depth + 1);
        for (const auto &stmt : ifStmt->thenBranch)
        {
            printAST(stmt.get(), depth + 1);
        }
        for (const auto &stmt : ifStmt->elseBranch)
        {
            printAST(stmt.get(), depth + 1);
        }
    }
    else if (auto whileStmt = dynamic_cast<const WhileNode *>(node))
    {
        printAST(whileStmt->condition.get(), depth + 1);
        for (const auto &stmt : whileStmt->body)
        {
            printAST(stmt.get(), depth + 1);
        }
    }
    else if (auto repeatStmt = dynamic_cast<const RepeatNode *>(node))
    {
        for (const auto &stmt : repeatStmt->body)
        {
            printAST(stmt.get(), depth + 1);
        }
        printAST(repeatStmt->condition.get(), depth + 1);
    }
    else if (auto binaryOp = dynamic_cast<const BinaryOpNode *>(node))
    {
        printAST(binaryOp->left.get(), depth + 1);
        printAST(binaryOp->right.get(), depth + 1);
    }
    else if (auto unaryOp = dynamic_cast<const UnaryOpNode *>(node))
    {
        printAST(unaryOp->operand.get(), depth + 1);
    }
    else if (auto varDecl = dynamic_cast<const VariableDeclarationNode *>(node))
    {
        if (varDecl->initialValue)
        {
            printAST(varDecl->initialValue.get(), depth + 1);
        }
    }
    else if (auto constDecl = dynamic_cast<const ConstantDeclarationNode *>(node))
    {
        if (constDecl->value)
        {
            printAST(constDecl->value.get(), depth + 1);
        }
    }
    else if (auto assignment = dynamic_cast<const AssignmentNode *>(node))
    {
        if (assignment->value)
        {
            printAST(assignment->value.get(), depth + 1);
        }
    }
    else if (auto printStmt = dynamic_cast<const PrintNode *>(node))
    {
        if (printStmt->expression)
        {
            printAST(printStmt->expression.get(), depth + 1);
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

// تطبيق معالجة الإجراءات
std::unique_ptr<ProcedureDeclarationNode> Parser::parseProcedureDeclaration()
{
    auto procDecl = std::make_unique<ProcedureDeclarationNode>();

    // الحصول على اسم الإجراء
    procDecl->name = consume(TokenType::IDENTIFIER, "توقع اسم الإجراء").value;
    std::cout << "[DBG] Parsing procedure: " << procDecl->name << std::endl;

    // معالجة المعاملات
    consume(TokenType::LPAREN, "توقع '(' بعد اسم الإجراء");
    procDecl->parameters = parseProcedureParameters();
    consume(TokenType::RPAREN, "توقع ')' بعد معاملات الإجراء");

    // معالجة نوع الإرجاع (اختياري)
    if (match(TokenType::COLON))
    {
        procDecl->returnType = parseType();
    }

    consume(TokenType::SEMICOLON, "توقع ';' بعد رأس الإجراء");

    // معالجة جسم الإجراء (قد يحتوي على تعريفات وجمل)
    while (!check(TokenType::END) && !isAtEnd())
    {
        // تحديد ما إذا كانت جملة تعريف أم جملة عادية
        if (check(TokenType::VARIABLE) || check(TokenType::CONSTANT) ||
            check(TokenType::TYPE) || check(TokenType::PROCEDURE))
        {
            std::cout << "[DBG] parsing declaration in procedure body, current token=" << peek().typeToString()
                      << " ('" << peek().value << ")" << std::endl;
            auto decl = parseDeclaration();
            if (decl)
            {
                procDecl->body.push_back(std::move(decl));
            }
            else
            {
                // إذا كانت النتيجة nullptr، فهذا جملة تعيين، معالجتها كجملة
                std::cout << "[DBG] declaration returned nullptr in procedure, treating as statement" << std::endl;
                procDecl->body.push_back(parseStatement());
            }
        }
        else
        {
            procDecl->body.push_back(parseStatement());
        }
    }

    consume(TokenType::END, "توقع 'نهاية' لإنهاء الإجراء");
    match(TokenType::SEMICOLON);

    return procDecl;
}

// معالجة معاملات الإجراء
std::vector<ProcedureParameterNode> Parser::parseProcedureParameters()
{
    std::vector<ProcedureParameterNode> parameters;

    if (check(TokenType::RPAREN))
    {
        return parameters; // لا توجد معاملات
    }

    do
    {
        bool isByReference = false;

        // التحقق من نوع المعامل (بالقيمة أو بالمرجع)
        if (match(TokenType::BY_VALUE))
        {
            isByReference = false;
        }
        else if (match(TokenType::BY_REFERENCE))
        {
            isByReference = true;
        }

        // الحصول على اسم المعامل
        std::string paramName = consume(TokenType::IDENTIFIER, "توقع اسم المعامل").value;

        // الحصول على نوع المعامل
        consume(TokenType::COLON, "توقع ':' بعد اسم المعامل");
        auto paramType = parseType();

        parameters.push_back(ProcedureParameterNode(paramName, std::move(paramType), isByReference));

    } while (match(TokenType::SEMICOLON) || match(TokenType::COMMA));  // دعم الفاصلة المنقوطة والفاصلة

    return parameters;
}

// معالجة جملة الإرجاع
std::unique_ptr<ReturnNode> Parser::parseReturnStatement()
{
    auto returnStmt = std::make_unique<ReturnNode>();

    // التحقق من وجود قيمة إرجاع
    if (!check(TokenType::SEMICOLON) && !check(TokenType::END))
    {
        returnStmt->value = parseExpression();
    }

    consume(TokenType::SEMICOLON, "توقع ';' بعد جملة الإرجاع");

    return returnStmt;
}

// معالجة الوصول للحقول والفهرسة
std::unique_ptr<ASTNode> Parser::parsePostfix()
{
    auto expr = parsePrimary();

    while (true)
    {
        if (match(TokenType::LBRACKET))
        {
            // الوصول للمصفوفة: arr[index]
            auto indexAccess = std::make_unique<IndexAccessNode>();
            indexAccess->variable = std::move(expr);
            indexAccess->index = parseExpression();
            consume(TokenType::RBRACKET, "توقع ']' بعد فهرس المصفوفة");
            expr = std::move(indexAccess);
        }
        else if (match(TokenType::DOT))
        {
            // الوصول للحقل: record.field
            auto fieldAccess = std::make_unique<FieldAccessNode>();
            fieldAccess->record = std::move(expr);
            fieldAccess->fieldName = consume(TokenType::IDENTIFIER, "توقع اسم الحقل").value;
            expr = std::move(fieldAccess);
        }
        else
        {
            break;
        }
    }

    return expr;
}
