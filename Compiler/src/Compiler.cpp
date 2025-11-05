#include "Compiler.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cctype>
#include <stdexcept>
#include <cstring>

Compiler::Compiler() : labelCounter(0), tempVarCounter(0) {}

std::string Compiler::generateLabel()
{
    return "L" + std::to_string(labelCounter++);
}

std::string Compiler::generateTempVar()
{
    return "t" + std::to_string(tempVarCounter++);
}

void Compiler::emit(InstructionType type, const std::string &op1,
                    const std::string &op2, const std::string &op3)
{
    instructions.emplace_back(type, op1, op2, op3);
}

std::string Compiler::getStringLabel(const std::string &literal)
{
    auto it = stringToLabel.find(literal);
    if (it != stringToLabel.end())
    {
        return it->second;
    }
    std::string label = "str_" + std::to_string(stringLiterals.size());
    stringToLabel[literal] = label;
    stringLiterals.push_back(literal);
    return label;
}

std::vector<Instruction> Compiler::compile(std::unique_ptr<ProgramNode> program)
{
    instructions.clear();
    symbolTable.clear();
    labelCounter = 0;
    tempVarCounter = 0;
    stringLiterals.clear();
    stringToLabel.clear();

    if (program)
    {
        try
        {
            compileProgram(program.get());
            emit(InstructionType::HALT);
        }
        catch (const std::exception &e)
        {
            std::cerr << "❌ خطأ أثناء الترجمة: " << e.what() << std::endl;
            instructions.clear();
        }
    }
    else
    {
        std::cerr << "⚠️  البرنامج فارغ - لا شيء لترجمته" << std::endl;
    }

    return instructions;
}

void Compiler::compileProgram(ProgramNode *program)
{
    if (!program)
    {
        std::cerr << "⚠️  برنامج فارغ في compileProgram" << std::endl;
        return;
    }

    // معالجة التعريفات أولاً
    for (auto &declaration : program->declarations)
    {
        if (declaration)
        {
            compileStatement(declaration.get());
        }
    }

    // ثم معالجة الجمل
    for (auto &statement : program->statements)
    {
        if (statement)
        {
            compileStatement(statement.get());
        }
    }
}

void Compiler::compileStatement(ASTNode *statement)
{
    if (!statement)
    {
        std::cerr << "⚠️  جملة فارغة في compileStatement" << std::endl;
        return;
    }

    if (auto varDecl = dynamic_cast<VariableDeclarationNode *>(statement))
    {
        compileVariableDeclaration(varDecl);
    }
    else if (auto constDecl = dynamic_cast<ConstantDeclarationNode *>(statement))
    {
        compileConstantDeclaration(constDecl);
    }
    else if (auto assignment = dynamic_cast<AssignmentNode *>(statement))
    {
        compileAssignment(assignment);
    }
    else if (auto print = dynamic_cast<PrintNode *>(statement))
    {
        compilePrint(print);
    }
    else if (auto read = dynamic_cast<ReadNode *>(statement))
    {
        compileRead(read);
    }
    else if (auto ifStmt = dynamic_cast<IfNode *>(statement))
    {
        compileIf(ifStmt);
    }
    else if (auto whileStmt = dynamic_cast<WhileNode *>(statement))
    {
        compileWhile(whileStmt);
    }
    else if (auto repeatStmt = dynamic_cast<RepeatNode *>(statement))
    {
        compileRepeat(repeatStmt);
    }
    else if (auto forStmt = dynamic_cast<ForNode *>(statement))
    {
        compileFor(forStmt);
    }
    else if (auto procDecl = dynamic_cast<ProcedureDeclarationNode *>(statement))
    {
        compileProcedureDeclaration(procDecl);
    }
    else if (auto procCall = dynamic_cast<ProcedureCallNode *>(statement))
    {
        compileProcedureCall(procCall);
    }
    else if (auto returnStmt = dynamic_cast<ReturnNode *>(statement))
    {
        compileReturnStatement(returnStmt);
    }
    else
    {
        // Ignore __empty__ nodes which are just semicolons
        if (auto varNode = dynamic_cast<VariableNode *>(statement))
        {
            if (varNode->name == "__empty__")
                return;
        }
        std::cerr << "⚠️  نوع جملة غير معروف في compileStatement: " << statement->getTypeName() << std::endl;
    }
}

void Compiler::compileVariableDeclaration(VariableDeclarationNode *node)
{
    if (!node)
        return;
    if (node->typeNode)
    {
        // Determine C type representation from explicit type
        if (auto prim = dynamic_cast<PrimitiveTypeNode *>(node->typeNode.get()))
        {
            std::string cname = "int";
            if (prim->name == "صحيح")
                cname = "int";
            else if (prim->name == "حقيقي")
                cname = "double";
            else if (prim->name == "خيط")
                cname = "char*";
            else if (prim->name == "منطقي")
                cname = "int";
            else
                cname = "int"; // fallback
            symbolTable[node->name] = std::string("primitive:") + cname;
        }
        else if (auto arr = dynamic_cast<ArrayTypeNode *>(node->typeNode.get()))
        {
            std::string elemC = "int";
            if (arr->elementType)
            {
                if (auto pe = dynamic_cast<PrimitiveTypeNode *>(arr->elementType.get()))
                {
                    if (pe->name == "صحيح")
                        elemC = "int";
                    else if (pe->name == "حقيقي")
                        elemC = "double";
                    else if (pe->name == "خيط")
                        elemC = "char*";
                }
            }
            symbolTable[node->name] = std::string("array:") + elemC + ":" + std::to_string(arr->length);
        }
        else if (auto rec = dynamic_cast<RecordTypeNode *>(node->typeNode.get()))
        {
            std::string structName = "struct_" + node->name;
            std::stringstream ss;
            ss << "struct " << structName << " {\n";
            for (const auto &f : rec->fields)
            {
                std::string fctype = "int";
                if (f.type)
                {
                    if (auto pf = dynamic_cast<PrimitiveTypeNode *>(f.type.get()))
                    {
                        if (pf->name == "صحيح")
                            fctype = "int";
                        else if (pf->name == "حقيقي")
                            fctype = "double";
                        else if (pf->name == "خيط")
                            fctype = "char*";
                    }
                }
                ss << "    " << fctype << " " << f.name << ";\n";
            }
            ss << "};\n";
            recordDefs[structName] = ss.str();
            symbolTable[node->name] = std::string("record:") + structName;
        }
        else
        {
            symbolTable[node->name] = "primitive:int";
        }
    }
    else
    {
        // No explicit type, so infer from initial value
        if (node->initialValue)
        {
            if (auto literal = dynamic_cast<LiteralNode *>(node->initialValue.get()))
            {
                if (literal->literalType == TokenType::STRING_LITERAL)
                {
                    symbolTable[node->name] = "primitive:char*";
                }
                else if (literal->literalType == TokenType::REAL_LITERAL)
                {
                    symbolTable[node->name] = "primitive:double";
                }
                else
                {
                    symbolTable[node->name] = "primitive:int"; // Default for NUMBER or other literals
                }
            }
            else
            {
                symbolTable[node->name] = "primitive:int"; // Default for complex expressions
            }
        }
        else
        {
            // No initial value and no type, default to int
            symbolTable[node->name] = "primitive:int";
        }
    }

    // handle initialization if present
    if (node->initialValue)
    {
        std::string temp = compileExpression(node->initialValue.get());
        emit(InstructionType::STORE, node->name, temp);
    }
}

void Compiler::compileConstantDeclaration(ConstantDeclarationNode *node)
{
    if (!node)
        return;

    // تخزين الثابت في جدول الرموز
    symbolTable[node->name] = "ثابت";

    std::string temp = compileExpression(node->value.get());
    emit(InstructionType::STORE, node->name, temp);
}

void Compiler::compileAssignment(AssignmentNode *node)
{
    if (!node || !node->left || !node->value)
        return;

    // First, compile the right-hand side to get the value to store
    std::string valueTemp = compileExpression(node->value.get());

    // Now, figure out where to store it
    if (auto varNode = dynamic_cast<VariableNode *>(node->left.get()))
    {
        // Simple assignment: var = value
        emit(InstructionType::STORE, varNode->name, valueTemp);
    }
    else if (auto indexAccessNode = dynamic_cast<IndexAccessNode *>(node->left.get()))
    {
        // Array assignment: array[index] = value

        // Get the array name
        VariableNode *arrayVarNode = dynamic_cast<VariableNode *>(indexAccessNode->variable.get());
        if (!arrayVarNode)
        {
            throw std::runtime_error("الوصول للفهرس في جملة التعيين يجب أن يكون لمتغير مباشر");
        }
        std::string arrayName = arrayVarNode->name;

        // Compile the index expression
        std::string indexTemp = compileExpression(indexAccessNode->index.get());

        // Emit the store instruction
        emit(InstructionType::STORE_INDEXED, arrayName, indexTemp, valueTemp);
    }
    else
    {
        throw std::runtime_error("هدف تعيين غير صالح");
    }
}

void Compiler::compilePrint(PrintNode *node)
{
    if (!node || !node->expression)
    {
        std::cerr << "⚠️  جملة طباعة فارغة أو بدون تعبير" << std::endl;
        return;
    }

    // طباعة السلاسل النصية مباشرة
    if (auto literal = dynamic_cast<LiteralNode *>(node->expression.get()))
    {
        if (literal->literalType == TokenType::STRING_LITERAL)
        {
            std::string label = getStringLabel(literal->value);
            emit(InstructionType::PRINT, "STRING", label);
            return;
        }
    }
    // Handle variables directly to use type information from symbol table
    else if (auto variable = dynamic_cast<VariableNode *>(node->expression.get()))
    {
        emit(InstructionType::PRINT, "VARIABLE", variable->name);
        return;
    }

    std::string temp = compileExpression(node->expression.get());
    emit(InstructionType::PRINT, "VALUE", temp);
}

void Compiler::compileRead(ReadNode *node)
{
    if (!node)
        return;

    emit(InstructionType::READ, node->variableName);
}

void Compiler::compileIf(IfNode *node)
{
    if (!node || !node->condition)
    {
        std::cerr << "⚠️  جملة شرطية فارغة أو بدون شرط" << std::endl;
        return;
    }

    std::string elseLabel = generateLabel();
    std::string endLabel = generateLabel();

    std::string conditionTemp = compileExpression(node->condition.get());
    emit(InstructionType::JZ, conditionTemp, elseLabel);

    // فرع then
    for (auto &stmt : node->thenBranch)
    {
        if (stmt)
        {
            compileStatement(stmt.get());
        }
    }

    if (!node->elseBranch.empty())
    {
        emit(InstructionType::JMP, endLabel);
    }

    emit(InstructionType::LABEL, elseLabel);

    // فرع else
    for (auto &stmt : node->elseBranch)
    {
        if (stmt)
        {
            compileStatement(stmt.get());
        }
    }

    if (!node->elseBranch.empty())
    {
        emit(InstructionType::LABEL, endLabel);
    }
}

void Compiler::compileWhile(WhileNode *node)
{
    if (!node || !node->condition)
    {
        std::cerr << "⚠️  حلقة طالما فارغة أو بدون شرط" << std::endl;
        return;
    }

    std::string startLabel = generateLabel();
    std::string endLabel = generateLabel();

    emit(InstructionType::LABEL, startLabel);

    std::string conditionTemp = compileExpression(node->condition.get());
    emit(InstructionType::JZ, conditionTemp, endLabel);

    for (auto &stmt : node->body)
    {
        if (stmt)
        {
            compileStatement(stmt.get());
        }
    }

    emit(InstructionType::JMP, startLabel);
    emit(InstructionType::LABEL, endLabel);
}

void Compiler::compileRepeat(RepeatNode *node)
{
    if (!node || !node->condition)
    {
        std::cerr << "⚠️  حلقة كرر-حتى فارغة أو بدون شرط" << std::endl;
        return;
    }

    std::string startLabel = generateLabel();

    emit(InstructionType::LABEL, startLabel);

    for (auto &stmt : node->body)
    {
        if (stmt)
        {
            compileStatement(stmt.get());
        }
    }

    std::string conditionTemp = compileExpression(node->condition.get());
    // في حلقة repeat-until، نكرر حتى يصبح الشرط صحيحاً
    emit(InstructionType::JZ, conditionTemp, startLabel);
}

void Compiler::compileFor(ForNode *node)
{
    if (!node)
        return;

    // 1. Initialize the iterator variable
    symbolTable[node->iteratorName] = "primitive:int"; // Ensure it's an integer
    std::string startValTemp = compileExpression(node->startValue.get());
    emit(InstructionType::STORE, node->iteratorName, startValTemp);

    std::string loopStartLabel = generateLabel();
    std::string loopEndLabel = generateLabel();

    // 2. Start of the loop
    emit(InstructionType::LABEL, loopStartLabel);

    // 3. Condition check
    std::string iteratorTemp = generateTempVar();
    emit(InstructionType::LOAD, iteratorTemp, node->iteratorName);
    std::string endValTemp = compileExpression(node->endValue.get());

    emit(InstructionType::CMP, iteratorTemp, endValTemp);
    emit(InstructionType::JG, loopEndLabel); // Jump if iterator > endValue

    // 4. Compile loop body
    for (auto &stmt : node->body)
    {
        if (stmt)
        {
            compileStatement(stmt.get());
        }
    }

    // 5. Increment step
    std::string stepValTemp;
    if (node->stepValue)
    {
        stepValTemp = compileExpression(node->stepValue.get());
    }
    else
    {
        stepValTemp = generateTempVar();
        emit(InstructionType::LOAD, stepValTemp, "1"); // Default step is 1
    }

    emit(InstructionType::LOAD, iteratorTemp, node->iteratorName); // Reload iterator value
    std::string newIteratorTemp = generateTempVar();
    emit(InstructionType::ADD, newIteratorTemp, iteratorTemp, stepValTemp);
    emit(InstructionType::STORE, node->iteratorName, newIteratorTemp);

    // 6. Jump back to the start
    emit(InstructionType::JMP, loopStartLabel);

    // 7. End of the loop
    emit(InstructionType::LABEL, loopEndLabel);
}

std::string Compiler::compileExpression(ASTNode *expr)
{
    if (!expr)
    {
        throw std::runtime_error("تعبير فارغ في compileExpression");
    }

    if (auto binaryOp = dynamic_cast<BinaryOpNode *>(expr))
    {
        if (!binaryOp->left || !binaryOp->right)
        {
            throw std::runtime_error("عملية ثنائية بدون معاملين");
        }

        std::string leftTemp = compileExpression(binaryOp->left.get());
        std::string rightTemp = compileExpression(binaryOp->right.get());
        std::string resultTemp = generateTempVar();

        switch (binaryOp->op)
        {
        case TokenType::PLUS:
            emit(InstructionType::ADD, resultTemp, leftTemp, rightTemp);
            break;
        case TokenType::MINUS:
            emit(InstructionType::SUB, resultTemp, leftTemp, rightTemp);
            break;
        case TokenType::MULTIPLY:
            emit(InstructionType::MUL, resultTemp, leftTemp, rightTemp);
            break;
        case TokenType::DIVIDE:
            emit(InstructionType::DIV, resultTemp, leftTemp, rightTemp);
            break;
        case TokenType::MOD:
            emit(InstructionType::MOD, resultTemp, leftTemp, rightTemp);
            break;
        case TokenType::AND:
            emit(InstructionType::AND, resultTemp, leftTemp, rightTemp);
            break;
        case TokenType::OR:
            emit(InstructionType::OR, resultTemp, leftTemp, rightTemp);
            break;
        case TokenType::EQUALS:
        case TokenType::NOT_EQUALS:
        case TokenType::LESS:
        case TokenType::GREATER:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER_EQUAL:
        {
            std::string trueLabel = generateLabel();
            std::string endLabel = generateLabel();

            emit(InstructionType::LOAD, resultTemp, "0");
            emit(InstructionType::CMP, leftTemp, rightTemp);

            switch (binaryOp->op)
            {
            case TokenType::EQUALS:
                emit(InstructionType::JE, trueLabel);
                break;
            case TokenType::NOT_EQUALS:
                emit(InstructionType::JNE, trueLabel);
                break;
            case TokenType::LESS:
                emit(InstructionType::JL, trueLabel);
                break;
            case TokenType::GREATER:
                emit(InstructionType::JG, trueLabel);
                break;
            case TokenType::LESS_EQUAL:
                emit(InstructionType::JLE, trueLabel);
                break;
            case TokenType::GREATER_EQUAL:
                emit(InstructionType::JGE, trueLabel);
                break;
            default:
                break;
            }
            emit(InstructionType::JMP, endLabel);
            emit(InstructionType::LABEL, trueLabel);
            emit(InstructionType::LOAD, resultTemp, "1");
            emit(InstructionType::LABEL, endLabel);
            break;
        }
        default:
            throw std::runtime_error("معامل غير مدعوم في التعبير: " + std::to_string(static_cast<int>(binaryOp->op)));
        }

        return resultTemp;
    }
    else if (auto unaryOp = dynamic_cast<UnaryOpNode *>(expr))
    {
        if (!unaryOp->operand)
        {
            throw std::runtime_error("عملية أحادية بدون معامل");
        }

        std::string operandTemp = compileExpression(unaryOp->operand.get());
        std::string resultTemp = generateTempVar();

        switch (unaryOp->op)
        {
        case TokenType::NOT:
            emit(InstructionType::LOAD, resultTemp, "1");
            emit(InstructionType::SUB, resultTemp, resultTemp, operandTemp);
            break;
        case TokenType::MINUS:
            emit(InstructionType::LOAD, resultTemp, "0");
            emit(InstructionType::SUB, resultTemp, resultTemp, operandTemp);
            break;
        default:
            throw std::runtime_error("معامل أحادي غير مدعوم: " + std::to_string(static_cast<int>(unaryOp->op)));
        }

        return resultTemp;
    }
    else if (auto literal = dynamic_cast<LiteralNode *>(expr))
    {
        std::string temp = generateTempVar();

        if (literal->literalType == TokenType::STRING_LITERAL)
        {
            std::string label = getStringLabel(literal->value);
            emit(InstructionType::LOAD, temp, label);
        }
        else
        {
            emit(InstructionType::LOAD, temp, literal->value);
        }
        return temp;
    }
    else if (auto variable = dynamic_cast<VariableNode *>(expr))
    {
        std::string temp = generateTempVar();
        emit(InstructionType::LOAD, temp, variable->name);
        return temp;
    }
    else if (auto indexAccess = dynamic_cast<IndexAccessNode *>(expr))
    {
        return compileIndexAccess(indexAccess);
    }
    else if (auto fieldAccess = dynamic_cast<FieldAccessNode *>(expr))
    {
        return compileFieldAccess(fieldAccess);
    }

    throw std::runtime_error("نوع تعبير غير مدعوم: " + std::string(expr->getTypeName()));
}

std::string Compiler::compileIndexAccess(IndexAccessNode *node)
{
    if (!node || !node->variable || !node->index)
    {
        throw std::runtime_error("عقدة وصول للفهرس غير مكتملة");
    }

    VariableNode *varNode = dynamic_cast<VariableNode *>(node->variable.get());
    if (!varNode)
    {
        throw std::runtime_error("الوصول للفهرس لا يدعم إلا المتغيرات المباشرة حاليًا");
    }

    std::string arrayName = varNode->name;
    std::string indexTemp = compileExpression(node->index.get());
    std::string resultTemp = generateTempVar();

    emit(InstructionType::LOAD_INDEXED, resultTemp, arrayName, indexTemp);
    return resultTemp;
}

void Compiler::generateAssembly(const std::string &filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("لا يمكن فتح الملف: " + filename);
    }

    file << "# كود تجميعي مولد للغة العربية" << std::endl;
    file << "# =============================" << std::endl
         << std::endl;

    file << ".data" << std::endl;

    // تعريف المتغيرات
    for (const auto &symbol : symbolTable)
    {
        file << symbol.first << ": .word 0" << std::endl;
    }

    // تعريف المتغيرات المؤقتة
    for (int i = 0; i < tempVarCounter; i++)
    {
        file << "t" << i << ": .word 0" << std::endl;
    }

    file << "newline: .asciiz \"\\n\"" << std::endl;
    file << "int_format: .asciiz \"%d\"" << std::endl;
    file << "str_format: .asciiz \"%s\"" << std::endl;

    // تعريف السلاسل النصية
    for (size_t i = 0; i < stringLiterals.size(); ++i)
    {
        std::string label = "str_" + std::to_string(i);
        file << label << ": .asciiz \"";
        // هروب الأحرف الخاصة في السلاسل النصية
        for (char ch : stringLiterals[i])
        {
            if (ch == '"')
                file << "\\\"";
            else if (ch == '\\')
                file << "\\\\";
            else if (ch == '\n')
                file << "\\n";
            else if (ch == '\t')
                file << "\\t";
            else
                file << ch;
        }
        file << "\"" << std::endl;
    }

    file << std::endl
         << ".text" << std::endl;
    file << ".globl main" << std::endl;
    file << "main:" << std::endl;

    // لتتبع آخر عملية مقارنة
    std::string lastCmpLeft;
    std::string lastCmpRight;

    for (const auto &instr : instructions)
    {
        file << "    ";

        switch (instr.type)
        {
        case InstructionType::LOAD:
            // إذا كان المعامل تسمية نصية، نحمّل عنوانها
            if (!instr.operand2.empty() && instr.operand2.rfind("str_", 0) == 0)
            {
                file << "la $t0, " << instr.operand2 << std::endl;
                file << "    sw $t0, " << instr.operand1 << std::endl;
            }
            else
            {
                bool isNumber = !instr.operand2.empty() &&
                                (std::isdigit(static_cast<unsigned char>(instr.operand2[0])) ||
                                 ((instr.operand2[0] == '-' || instr.operand2[0] == '+') && instr.operand2.size() > 1 &&
                                  std::isdigit(static_cast<unsigned char>(instr.operand2[1]))));
                if (isNumber)
                {
                    file << "li $t0, " << instr.operand2 << std::endl;
                    file << "    sw $t0, " << instr.operand1 << std::endl;
                }
                else
                {
                    // اعتبرها متغيرًا/مؤقتًا: حمّل قيمته
                    file << "lw $t0, " << instr.operand2 << std::endl;
                    file << "    sw $t0, " << instr.operand1 << std::endl;
                }
            }
            break;

        case InstructionType::STORE:
            file << "lw $t0, " << instr.operand2 << std::endl;
            file << "    sw $t0, " << instr.operand1 << std::endl;
            break;

        case InstructionType::ADD:
            file << "lw $t1, " << instr.operand2 << std::endl;
            file << "    lw $t2, " << instr.operand3 << std::endl;
            file << "    add $t0, $t1, $t2" << std::endl;
            file << "    sw $t0, " << instr.operand1 << std::endl;
            break;

        case InstructionType::SUB:
            file << "lw $t1, " << instr.operand2 << std::endl;
            file << "    lw $t2, " << instr.operand3 << std::endl;
            file << "    sub $t0, $t1, $t2" << std::endl;
            file << "    sw $t0, " << instr.operand1 << std::endl;
            break;

        case InstructionType::MUL:
            file << "lw $t1, " << instr.operand2 << std::endl;
            file << "    lw $t2, " << instr.operand3 << std::endl;
            file << "    mul $t0, $t1, $t2" << std::endl;
            file << "    sw $t0, " << instr.operand1 << std::endl;
            break;

        case InstructionType::DIV:
            file << "lw $t1, " << instr.operand2 << std::endl;
            file << "    lw $t2, " << instr.operand3 << std::endl;
            file << "    div $t0, $t1, $t2" << std::endl;
            file << "    sw $t0, " << instr.operand1 << std::endl;
            break;

        case InstructionType::PRINT:
            if (instr.operand1 == "STRING")
            {
                file << "li $v0, 4" << std::endl;
                file << "    la $a0, " << instr.operand2 << std::endl;
                file << "    syscall" << std::endl;
            }
            else
            {
                file << "li $v0, 1" << std::endl;
                file << "    lw $a0, " << instr.operand2 << std::endl;
                file << "    syscall" << std::endl;
            }
            file << "    li $v0, 4" << std::endl;
            file << "    la $a0, newline" << std::endl;
            file << "    syscall" << std::endl;
            break;

        case InstructionType::READ:
            file << "li $v0, 5" << std::endl;
            file << "    syscall" << std::endl;
            file << "    sw $v0, " << instr.operand1 << std::endl;
            break;

        case InstructionType::JMP:
            file << "j " << instr.operand1 << std::endl;
            break;

        case InstructionType::JZ:
            file << "lw $t0, " << instr.operand1 << std::endl;
            file << "    beqz $t0, " << instr.operand2 << std::endl;
            break;

        case InstructionType::LABEL:
            file << instr.operand1 << ":" << std::endl;
            break;

        case InstructionType::CMP:
            lastCmpLeft = instr.operand1;
            lastCmpRight = instr.operand2;
            file << "lw $t1, " << lastCmpLeft << std::endl;
            file << "    lw $t2, " << lastCmpRight << std::endl;
            file << "    # CMP " << instr.operand1 << ", " << instr.operand2 << std::endl;
            break;

        case InstructionType::JE:
            file << "lw $t1, " << lastCmpLeft << std::endl;
            file << "    lw $t2, " << lastCmpRight << std::endl;
            file << "    beq $t1, $t2, " << instr.operand1 << std::endl;
            break;

        case InstructionType::JNE:
            file << "lw $t1, " << lastCmpLeft << std::endl;
            file << "    lw $t2, " << lastCmpRight << std::endl;
            file << "    bne $t1, $t2, " << instr.operand1 << std::endl;
            break;

        case InstructionType::JG:
            file << "lw $t1, " << lastCmpLeft << std::endl;
            file << "    lw $t2, " << lastCmpRight << std::endl;
            file << "    bgt $t1, $t2, " << instr.operand1 << std::endl;
            break;

        case InstructionType::JL:
            file << "lw $t1, " << lastCmpLeft << std::endl;
            file << "    lw $t2, " << lastCmpRight << std::endl;
            file << "    blt $t1, $t2, " << instr.operand1 << std::endl;
            break;

        case InstructionType::JGE:
            file << "lw $t1, " << lastCmpLeft << std::endl;
            file << "    lw $t2, " << lastCmpRight << std::endl;
            file << "    bge $t1, $t2, " << instr.operand1 << std::endl;
            break;

        case InstructionType::JLE:
            file << "lw $t1, " << lastCmpLeft << std::endl;
            file << "    lw $t2, " << lastCmpRight << std::endl;
            file << "    ble $t1, $t2, " << instr.operand1 << std::endl;
            break;

        case InstructionType::HALT:
            file << "li $v0, 10" << std::endl;
            file << "    syscall" << std::endl;
            break;

        default:
            file << "# " << instr.toString() << std::endl;
            break;
        }
    }

    file.close();
    std::cout << "✅ تم توليد كود التجميع في: " << filename << std::endl;
}

void Compiler::generateCCode(const std::string &filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("لا يمكن فتح الملف: " + filename);
    }

    file << "// كود C مولد للغة العربية" << std::endl;
    file << "// =======================" << std::endl
         << std::endl;

    file << "#include <stdio.h>" << std::endl;
    file << "#include <stdlib.h>" << std::endl
         << std::endl;

    // Emit struct definitions for records
    for (const auto &kv : recordDefs)
    {
        file << kv.second << std::endl;
    }

    file << "int main() {" << std::endl;

    // تعريف المتغيرات
    for (const auto &symbol : symbolTable)
    {
        const std::string &name = symbol.first;
        const std::string &meta = symbol.second;
        if (meta.rfind("primitive:", 0) == 0)
        {
            std::string ctype = meta.substr(strlen("primitive:"));
            if (ctype == "char*")
                file << "    " << ctype << " " << name << " = NULL;" << std::endl;
            else
                file << "    " << ctype << " " << name << " = 0;" << std::endl;
        }
        else if (meta.rfind("array:", 0) == 0)
        {
            // format: array:<elemC>:<len>
            auto rest = meta.substr(strlen("array:"));
            auto colonPos = rest.find(':');
            if (colonPos != std::string::npos)
            {
                std::string elemC = rest.substr(0, colonPos);
                std::string len = rest.substr(colonPos + 1);
                file << "    " << elemC << " " << name << "[" << len << "];" << std::endl;
            }
            else
            {
                file << "    int " << name << " = 0; // malformed array meta" << std::endl;
            }
        }
        else if (meta.rfind("record:", 0) == 0)
        {
            std::string structName = meta.substr(strlen("record:"));
            file << "    struct " << structName << " " << name << ";" << std::endl;
        }
        else
        {
            file << "    int " << name << " = 0;" << std::endl;
        }
    }

    // تعريف المتغيرات المؤقتة
    for (int i = 0; i < tempVarCounter; i++)
    {
        file << "    int t" << i << " = 0;" << std::endl;
    }

    // تعريف السلاسل النصية كمتغيرات ثابتة
    for (size_t i = 0; i < stringLiterals.size(); ++i)
    {
        file << "    char* str_" << i << " = \"" << stringLiterals[i] << "\";" << std::endl;
    }

    file << std::endl;

    std::string lastCmpLeft, lastCmpRight;

    for (const auto &instr : instructions)
    {
        file << "    ";

        switch (instr.type)
        {
        case InstructionType::LOAD:
            file << instr.operand1 << " = " << instr.operand2 << ";";
            break;
        case InstructionType::STORE:
            file << instr.operand1 << " = " << instr.operand2 << ";";
            break;
        case InstructionType::ADD:
            file << instr.operand1 << " = " << instr.operand2 << " + " << instr.operand3 << ";";
            break;
        case InstructionType::SUB:
            file << instr.operand1 << " = " << instr.operand2 << " - " << instr.operand3 << ";";
            break;
        case InstructionType::MUL:
            file << instr.operand1 << " = " << instr.operand2 << " * " << instr.operand3 << ";";
            break;
        case InstructionType::DIV:
            file << instr.operand1 << " = " << instr.operand2 << " / " << instr.operand3 << ";";
            break;
        case InstructionType::MOD:
            file << instr.operand1 << " = " << instr.operand2 << " % " << instr.operand3 << ";";
            break;
        case InstructionType::PRINT:
            if (instr.operand1 == "STRING")
            {
                file << "printf(\"%s\\n\", " << instr.operand2 << ");";
            }
            else if (instr.operand1 == "VARIABLE")
            {
                const auto &varName = instr.operand2;
                auto it = symbolTable.find(varName);
                if (it != symbolTable.end() && it->second == "primitive:char*")
                {
                    file << "printf(\"%s\\n\", " << varName << ");";
                }
                else
                {
                    file << "printf(\"%d\\n\", " << varName << ");";
                }
            }
            else // VALUE for complex expressions, assume numeric
            {
                file << "printf(\"%d\\n\", " << instr.operand2 << ");";
            }
            break;
        case InstructionType::READ:
            file << "scanf(\"%d\", &" << instr.operand1 << ");";
            break;
        case InstructionType::JMP:
            file << "goto " << instr.operand1 << ";";
            break;
        case InstructionType::JZ:
            file << "if (!" << instr.operand1 << ") goto " << instr.operand2 << ";";
            break;
        case InstructionType::CMP:
            lastCmpLeft = instr.operand1;
            lastCmpRight = instr.operand2;
            file << "/* CMP " << instr.operand1 << ", " << instr.operand2 << " */";
            break;
        case InstructionType::JE:
            file << "if (" << lastCmpLeft << " == " << lastCmpRight << ") goto " << instr.operand1 << ";";
            break;
        case InstructionType::JNE:
            file << "if (" << lastCmpLeft << " != " << lastCmpRight << ") goto " << instr.operand1 << ";";
            break;
        case InstructionType::JG:
            file << "if (" << lastCmpLeft << " > " << lastCmpRight << ") goto " << instr.operand1 << ";";
            break;
        case InstructionType::JL:
            file << "if (" << lastCmpLeft << " < " << lastCmpRight << ") goto " << instr.operand1 << ";";
            break;
        case InstructionType::JGE:
            file << "if (" << lastCmpLeft << " >= " << lastCmpRight << ") goto " << instr.operand1 << ";";
            break;
        case InstructionType::JLE:
            file << "if (" << lastCmpLeft << " <= " << lastCmpRight << ") goto " << instr.operand1 << ";";
            break;
        case InstructionType::LOAD_INDEXED:
            // op1 = op2[op3] -> target = base[index]
            file << instr.operand1 << " = " << instr.operand2 << "[" << instr.operand3 << "];";
            break;
        case InstructionType::STORE_INDEXED:
            // op1[op2] = op3 -> base[index] = source
            file << instr.operand1 << "[" << instr.operand2 << "] = " << instr.operand3 << ";";
            break;
        case InstructionType::LABEL:
            file << instr.operand1 << ":";
            break;
        case InstructionType::HALT:
            file << "return 0;";
            break;
        default:
            file << "// " << instr.toString() << ";";
            break;
        }

        file << std::endl;
    }

    file << "}" << std::endl;
    file.close();
    std::cout << "✅ تم توليد كود C في: " << filename << std::endl;
}

void Compiler::generateIntermediateCode(const std::string &filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("لا يمكن فتح الملف: " + filename);
    }

    file << "الكود الوسيط المولد:" << std::endl;
    file << "====================" << std::endl
         << std::endl;

    for (size_t i = 0; i < instructions.size(); i++)
    {
        file << std::setw(3) << i << ": " << instructions[i].toString() << std::endl;
    }

    file << std::endl
         << "جدول الرموز:" << std::endl;
    file << "============" << std::endl;
    for (const auto &symbol : symbolTable)
    {
        file << symbol.first << " : " << symbol.second << std::endl;
    }

    file << std::endl
         << "السلاسل النصية:" << std::endl;
    file << "==============" << std::endl;
    for (size_t i = 0; i < stringLiterals.size(); ++i)
    {
        file << "str_" << i << ": \"" << stringLiterals[i] << "\"" << std::endl;
    }

    file.close();
    std::cout << "✅ تم توليد الكود الوسيط في: " << filename << std::endl;
}

void Compiler::displayInstructions() const
{
    if (instructions.empty())
    {
        std::cout << "⚠️  لا توجد تعليمات وسيطة لعرضها" << std::endl;
        return;
    }

    std::cout << "\n🔧 الكود الوسيط المولد:" << std::endl;
    std::cout << "====================" << std::endl;

    for (size_t i = 0; i < instructions.size(); i++)
    {
        std::cout << std::setw(3) << i << ": " << instructions[i].toString() << std::endl;
    }

    std::cout << "\n📊 إحصائيات:" << std::endl;
    std::cout << "عدد التعليمات: " << instructions.size() << std::endl;
    std::cout << "عدد المتغيرات المؤقتة: " << tempVarCounter << std::endl;
    std::cout << "عدد العلامات: " << labelCounter << std::endl;
    std::cout << "عدد السلاسل النصية: " << stringLiterals.size() << std::endl;
    std::cout << "عدد الرموز في جدول الرموز: " << symbolTable.size() << std::endl;
}

// معالجة تعريف الإجراء
void Compiler::compileProcedureDeclaration(ProcedureDeclarationNode *node)
{
    if (!node)
        return;

    std::cout << "[DBG] Compiling procedure: " << node->name << std::endl;

    // إنشء تسمية فريدة للإجراء
    std::string procLabel = "proc_" + node->name;
    procedureDefs[node->name] = procLabel;

    // إصدار تسمية الإجراء
    emit(InstructionType::LABEL, procLabel);

    // معالجة جسم الإجراء
    for (auto &stmt : node->body)
    {
        if (stmt)
        {
            compileStatement(stmt.get());
        }
    }

    // إذا لم يكن هناك جملة إرجاع، أضف إرجاع افتراضي
    emit(InstructionType::RET);
}

// معالجة استدعاء الإجراء
void Compiler::compileProcedureCall(ProcedureCallNode *node)
{
    if (!node)
        return;

    std::cout << "[DBG] Compiling procedure call: " << node->name << std::endl;

    // معالجة المعاملات (إذا كانت موجودة)
    for (size_t i = 0; i < node->arguments.size(); ++i)
    {
        if (node->arguments[i])
        {
            std::string argTemp = compileExpression(node->arguments[i].get());
            // تخزين المعامل في موقع معروف (مثل stack)
            std::string paramName = "param_" + std::to_string(i);
            emit(InstructionType::STORE, paramName, argTemp);
        }
    }

    // البحث عن تسمية الإجراء
    auto it = procedureDefs.find(node->name);
    if (it != procedureDefs.end())
    {
        emit(InstructionType::CALL, it->second);
    }
    else
    {
        std::cerr << "⚠️  تحذير: الإجراء '" << node->name << "' لم يتم تعريفه" << std::endl;
        emit(InstructionType::CALL, "proc_" + node->name);
    }
}

// معالجة جملة الإرجاع
void Compiler::compileReturnStatement(ReturnNode *node)
{
    if (!node)
    {
        emit(InstructionType::RET);
        return;
    }

    // إذا كانت هناك قيمة إرجاع، قم بتجميعها
    if (node->value)
    {
        std::string returnValueTemp = compileExpression(node->value.get());
        // تخزين قيمة الإرجاع في موقع معروف
        emit(InstructionType::STORE, "return_value", returnValueTemp);
    }

    emit(InstructionType::RET);
}

// معالجة وصول الحقل
std::string Compiler::compileFieldAccess(FieldAccessNode *node)
{
    if (!node || !node->record)
    {
        throw std::runtime_error("عقدة وصول الحقل غير مكتملة");
    }

    // الحصول على اسم السجل
    VariableNode *recordVar = dynamic_cast<VariableNode *>(node->record.get());
    if (!recordVar)
    {
        throw std::runtime_error("وصول الحقل يدعم فقط المتغيرات المباشرة حالياً");
    }

    std::string recordName = recordVar->name;
    std::string fieldName = node->fieldName;

    // إنشاء اسم مركب للحقل
    std::string fieldFullName = recordName + "." + fieldName;

    std::string temp = generateTempVar();
    emit(InstructionType::LOAD, temp, fieldFullName);

    return temp;
}