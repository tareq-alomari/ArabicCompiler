# Arabic Programming Language Compiler - Complete Codebase

This file contains the complete source code of the Arabic Programming Language Compiler project.
Generated for AI analysis and documentation purposes.

**Project**: ArabicCompiler
**Date**: $(date)
**Platform**: Cross-platform (Windows, macOS, Linux)
**Languages**: C++17, C#/.NET 6.0, Bash, PowerShell

---

## Table of Contents

1. Core Compiler (C++)
2. IDE (C#)
3. Build System
4. Tests & Examples
5. Documentation
6. Scripts

---


---

## 📄 `./Compiler/src/AST.cpp`

```cpp
#include "AST.h"

// Implementation of toString methods that were moved from the header to break circular dependencies.

std::string AssignmentNode::toString() const
{
    std::string leftStr = "غير معروف";
    if (left)
    {
        if (auto var = dynamic_cast<VariableNode *>(left.get()))
        {
            leftStr = "متغير: " + var->name;
        }
        else if (dynamic_cast<IndexAccessNode *>(left.get()))
        {
            leftStr = "عنصر مصفوفة";
        }
    }
    return "تعيين: " + leftStr + " = [تعبير]";
}

std::string IndexAccessNode::toString() const
{
    std::string varName = "غير معروف";
    if (variable) {
        if (auto var = dynamic_cast<VariableNode*>(variable.get())) {
            varName = var->name;
        }
    }
    return "الوصول للمصفوفة: " + varName + "[فهرس]";
}

```


---

## 📄 `./Compiler/src/Compiler.cpp`

```cpp
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
```


---

## 📄 `./Compiler/src/Lexer.cpp`

```cpp
#include "Lexer.h"
#include <cctype>
#include <algorithm>
#include <iostream>
#include <iomanip> // Required for std::hex, std::setw, std::setfill
#include <sstream> // Required for stringstream

// في Lexer.cpp - إصلاح تهيئة الكلمات المحجوزة
Lexer::Lexer(const std::string &source, bool debugFlag)
    : source(source), position(0), line(1), column(1), debug(debugFlag)
{
    // استخدام UTF-8 للكلمات المحجوزة
    keywords = {
        // البرنامج والكتل
        {"برنامج", TokenType::PROGRAM},
        {"نهاية", TokenType::END},
        {"إجراء", TokenType::PROCEDURE},

        // التعريفات
        {"متغير", TokenType::VARIABLE},
        {"ثابت", TokenType::CONSTANT},
        {"نوع", TokenType::TYPE},
        {"قائمة", TokenType::ARRAY},
        {"سجل", TokenType::RECORD},

        // أنواع البيانات
        {"صحيح", TokenType::INTEGER},
        {"حقيقي", TokenType::REAL},
        {"منطقي", TokenType::BOOLEAN},
        {"خيط", TokenType::STRING},
        {"خيط_رمزي", TokenType::STRING},
        {"حرفي", TokenType::STRING},

        // التحكم والشروط
        {"اذا", TokenType::IF},
        {"فان", TokenType::THEN},
        {"والا", TokenType::ELSE},
        {"وإلا", TokenType::ELSE},  // بديل مع همزة
        {"طالما", TokenType::WHILE},
        {"كرر", TokenType::REPEAT},
        {"اعد", TokenType::REPEAT},  // بديل لـ REPEAT (repeat-until)
        {"حتى", TokenType::UNTIL},
        {"استمر", TokenType::CONTINUE},  // كلمة محجوزة إضافية

        // العمليات
        {"اطبع", TokenType::PRINT},
        {"اقرأ", TokenType::READ},
        {"الى", TokenType::TO},
        {"اضف", TokenType::ADD},
        {"إرجاع", TokenType::RETURN},

        // معاملات الإجراءات
        {"بالقيمة", TokenType::BY_VALUE},
        {"بالمرجع", TokenType::BY_REFERENCE},
        {"من", TokenType::FROM},

        // القيم المنطقية
        {"صح", TokenType::TRUE_VAL},
        {"خطأ", TokenType::FALSE_VAL}};
}

void Lexer::analyzeEncoding()
{
    if (!debug)
        return;
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
    if (!debug)
        return;
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
    // Normalize CR/LF handling: treat CR, LF, or CRLF as a single newline
    if (c == '\r')
    {
        // If CRLF, consume LF as part of the newline
        if (position < source.length() && source[position] == '\n')
        {
            position++; // consume LF
        }
        line++;
        column = 1;
    }
    else if (c == '\n')
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

        // Explicitly handle ASCII whitespace characters to avoid locale issues
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v')
        {
            advance();
            continue;
        }
        // Windows-1256 NO-BREAK SPACE
        if (uc == 0xA0)
        {
            advance();
            continue;
        }
        // UTF-8 BOM sequence
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
        while (position < source.length())
        {
            char c = peek();
            if (c == '\n' || c == '\r')
            {
                break;
            }
            advance();
        }
        // Consume the newline after the comment (handle CRLF and LF)
        if (position < source.length())
        {
            if (peek() == '\r')
                advance();
            if (peek() == '\n')
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
    return Token(TokenType::STRING_LITERAL, str_val, line, startColumn);
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

        // Stop if a whitespace character is encountered.
        if (std::isspace(uc))
        {
            break;
        }

        // Stop reading identifier if punctuation is found
        if (c == ';' || c == ',' || c == '(' || c == ')' || c == '=' || c == '+' || c == '-' || c == '*' || c == '/')
        {
            break;
        }
        if (position + 1 < source.length())
        {
            unsigned char byte1 = uc;
            unsigned char byte2 = static_cast<unsigned char>(source[position + 1]);
            if (byte1 == 0xD8 && (byte2 == 0x9B || byte2 == 0x8C))
            { // Arabic Semicolon or Comma
                break;
            }
        }

        // Consume valid identifier characters (letters, numbers, non-ASCII)
        if (std::isalnum(uc) || uc == '_' || (uc >= 0x80))
        {
            identifier_val += advance();
        }
        else
        {
            break;
        }
    }

    // Check for keywords
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

Token Lexer::getNextToken()
{
    while (position < source.length())
    {
        // Keep skipping whitespace and comments until we reach something meaningful
        while (true)
        {
            size_t before = position;
            skipWhitespace();
            skipComment();
            if (position == before)
                break; // nothing consumed
        }

        if (position >= source.length())
        {
            return Token(TokenType::END_OF_FILE, "", line, column);
        }

        char current = peek();
        unsigned char ucCurrent = static_cast<unsigned char>(current);
        int tokenStartColumn = column;

        // Correctly handle multi-byte UTF-8 Arabic punctuation before identifier check
        if (position + 1 < source.length())
        {
            unsigned char byte1 = static_cast<unsigned char>(source[position]);
            unsigned char byte2 = static_cast<unsigned char>(source[position + 1]);

            // Arabic Semicolon ؛ is 0xD8 0x9B in UTF-8
            if (byte1 == 0xD8 && byte2 == 0x9B)
            {
                advance();
                advance();
                return Token(TokenType::SEMICOLON, "؛", line, tokenStartColumn);
            }
            // Arabic Comma ، is 0xD8 0x8C in UTF-8
            if (byte1 == 0xD8 && byte2 == 0x8C)
            {
                advance();
                advance();
                return Token(TokenType::COMMA, "،", line, tokenStartColumn);
            }
        }

        // Handle Windows-1256 specific punctuation FIRST
        if (ucCurrent == 0xBA) // ؛ Arabic Semicolon
        {
            advance();
            return Token(TokenType::SEMICOLON, ";", line, tokenStartColumn);
        }
        if (ucCurrent == 0xAC) // ، Arabic Comma
        {
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

        // Character literals using single quotes
        if (current == '\'')
        {
            int startLine = line;
            int startColumn = column;
            advance(); // consume opening '
            if (position >= source.length())
            {
                return Token(TokenType::ERROR, "رمز حرفي غير مكتمل", startLine, startColumn);
            }
            char ch = peek();
            // handle escape sequences like '\n' '\'' '\\'
            std::string val;
            if (ch == '\\')
            {
                advance();
                if (position >= source.length())
                    return Token(TokenType::ERROR, "رمز حرفي غير مكتمل بعد \\", startLine, startColumn);
                char esc = peek();
                switch (esc)
                {
                case 'n':
                    val = "\n";
                    break;
                case 't':
                    val = "\t";
                    break;
                case '\'':
                    val = "'";
                    break;
                case '\\':
                    val = "\\";
                    break;
                default:
                    val = std::string(1, esc);
                    break;
                }
                advance();
            }
            else
            {
                val = std::string(1, ch);
                advance();
            }

            if (peek() != '\'')
            {
                return Token(TokenType::ERROR, "سلسلة حرفية غير مغلقة", startLine, startColumn);
            }
            advance(); // consume closing '
            return Token(TokenType::CHAR_LITERAL, val, startLine, startColumn);
        }

        // Identifiers and Keywords
        unsigned char ucCurrentForCheck = ucCurrent;
        if (std::isalpha(ucCurrentForCheck) ||
            current == '_' || ((ucCurrentForCheck >= 0xC1 && ucCurrentForCheck <= 0xDA) || (ucCurrentForCheck >= 0xDC && ucCurrentForCheck <= 0xEE)))
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
            errorVal.push_back(current);
        }
        else
        {
            std::stringstream ss_err; // Use the correct name
            ss_err << "\\x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(current));
            errorVal += ss_err.str();
        }
        // Print surrounding byte context for diagnostics
        try
        {
            size_t ctxStart = position >= 8 ? position - 8 : 0;
            size_t ctxEnd = std::min(position + 8, source.length());
            std::stringstream ctx;
            ctx << "\n[DBG] Context bytes around error: ";
            for (size_t i = ctxStart; i < ctxEnd; ++i)
            {
                ctx << std::hex << std::setw(2) << std::setfill('0') << (static_cast<int>(static_cast<unsigned char>(source[i]))) << " ";
            }
            ctx << std::dec << "\n";
            std::cerr << ctx.str();
        }
        catch (...)
        {
            // ignore diagnostics failures
        }
        errorVal += "'";
        return Token(TokenType::ERROR, errorVal, line, tokenStartColumn);

    } // End while loop

    return Token(TokenType::END_OF_FILE, "", line, column);
}

```


---

## 📄 `./Compiler/src/main.cpp`

```cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include "Lexer.h"
#include "Parser.h"
#include "Compiler.h"

// قراءة الملف العربي باستخدام الترميز الصحيح
std::string readFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "خطأ: لا يمكن فتح الملف: " << filename << std::endl;
        throw std::runtime_error("لا يمكن فتح الملف: " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    // محاولة تحويل الترميز إذا كان هناك مشاكل
    if (!content.empty() && static_cast<unsigned char>(content[0]) == 0xEF &&
        content.size() > 2 && static_cast<unsigned char>(content[1]) == 0xBB &&
        static_cast<unsigned char>(content[2]) == 0xBF)
    {
        // إزالة BOM UTF-8 إذا كان موجوداً
        content = content.substr(3);
    }

    return content;
}

void printTokens(const std::vector<Token> &tokens)
{
    std::cout << "🎯 الرموز المميزة:" << std::endl;
    std::cout << "=================" << std::endl;

    for (const auto &token : tokens)
    {
        std::cout << "السطر " << token.line << ", العمود " << token.column << ": "
                  << token.typeToString() << " -> '";

        // طباعة القيمة مع التعامل مع الأحرف غير القابلة للطباعة
        for (char c : token.value)
        {
            if (c >= 32 && c < 127)
            {
                std::cout << c;
            }
            else
            {
                std::cout << "?";
            }
        }
        std::cout << "'" << std::endl;
    }
    std::cout << std::endl;
}

void displayHelp()
{
    std::cout << "🚀 مترجم اللغة العربية - الإصدار المتكامل" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "استخدام: ArabicCompiler <ملف_المصدر> [خيارات]" << std::endl;
    std::cout << std::endl;
    std::cout << "خيارات:" << std::endl;
    std::cout << "  --asm     توليد كود تجميعي (MIPS)" << std::endl;
    std::cout << "  --c       توليد كود C" << std::endl;
    std::cout << "  --ir      توليد كود وسيط" << std::endl;
    std::cout << "  --all     توليد جميع المخرجات" << std::endl;
    std::cout << "  --tokens  عرض الرموز المميزة" << std::endl;
    std::cout << "  --ast     عرض شجرة الاشتقاق النحوي" << std::endl;
    std::cout << "  --help    عرض هذه المساعدة" << std::endl;
    std::cout << "  --debug-lexer طباعة معلومات تشخيصية للـ lexer (بايتس)" << std::endl;
    std::cout << std::endl;
    std::cout << "أمثلة:" << std::endl;
    std::cout << "  ArabicCompiler program.arabic --all" << std::endl;
    std::cout << "  ArabicCompiler program.arabic --asm --tokens" << std::endl;
    std::cout << "  ArabicCompiler program.arabic --tokens --ast" << std::endl;
}

int main(int argc, char *argv[])
{
// إعداد الترميز للمخرجات العربية
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

    if (argc < 2 || (argc >= 2 && std::string(argv[1]) == "--help"))
    {
        displayHelp();
        return 0;
    }

    std::string filename = argv[1];
    bool generateAsm = false;
    bool generateC = false;
    bool generateIr = false;
    bool showTokens = false;
    bool showAST = false;
    bool debugLexer = false;

    // معالجة الخيارات
    for (int i = 2; i < argc; i++)
    {
        std::string option = argv[i];
        if (option == "--asm")
            generateAsm = true;
        else if (option == "--c")
            generateC = true;
        else if (option == "--ir")
            generateIr = true;
        else if (option == "--all")
        {
            generateAsm = generateC = generateIr = true;
        }
        else if (option == "--tokens")
            showTokens = true;
        else if (option == "--ast")
            showAST = true;
        else if (option == "--debug-lexer")
            debugLexer = true;
        else
        {
            std::cerr << "❌ خيار غير معروف: " << option << std::endl;
            displayHelp();
            return 1;
        }
    }

    // إذا لم يتم تحديد أي خيار، استخدم الافتراضي
    if (!generateAsm && !generateC && !generateIr && !showTokens && !showAST)
    {
        generateAsm = true; // الافتراضي: توليد كود التجميع
    }

    try
    {
        std::string sourceCode = readFile(filename);
        std::cout << "📁 جاري تحليل الملف: " << filename << std::endl;
        std::cout << "📏 طول النص: " << sourceCode.length() << " حرف" << std::endl;

        // التحليل اللغوي
        Lexer lexer(sourceCode, debugLexer);
        auto tokens = lexer.tokenize();

        // التحقق من الأخطاء اللغوية
        bool hasLexicalErrors = false;
        for (const auto &token : tokens)
        {
            if (token.type == TokenType::ERROR)
            {
                std::cerr << "❌ خطأ لغوي في السطر " << token.line
                          << ", العمود " << token.column << ": "
                          << token.value << std::endl;
                hasLexicalErrors = true;
            }
        }

        if (hasLexicalErrors)
        {
            std::cerr << "❌ فشل التحليل اللغوي بسبب وجود أخطاء." << std::endl;
            return 1;
        }

        // إذا طلب المستخدم عرض الرموز فقط
        if (showTokens)
        {
            printTokens(tokens);
            if (!generateAsm && !generateC && !generateIr && !showAST)
            {
                std::cout << "✅ عرض الرموز المميزة تم بنجاح!" << std::endl;
                return 0;
            }
        }

        std::cout << "✅ التحليل اللغوي تم بنجاح! (" << tokens.size() << " رمز)" << std::endl;

        // التحليل النحوي
        Parser parser(tokens);
        auto program = parser.parse();

        if (!program)
        {
            std::cerr << "❌ فشل التحليل النحوي." << std::endl;
            return 1;
        }

        std::cout << "✅ التحليل النحوي تم بنجاح!" << std::endl;

        // عرض شجرة الاشتقاق النحوي إذا طلب المستخدم
        if (showAST)
        {
            std::cout << "🌳 شجرة الاشتقاق النحوي (AST):" << std::endl;
            std::cout << "=============================" << std::endl;

            // استخدام المؤشر الخام مباشرة - لا نحتاج إلى إنشاء unique_ptr جديد
            // لأن program هو بالفعل unique_ptr ويدير الذاكرة
            parser.printAST(program.get());
            parser.printProgramStructure(program.get());

            if (!generateAsm && !generateC && !generateIr)
            {
                std::cout << "✅ عرض الشجرة النحوية تم بنجاح!" << std::endl;
                return 0;
            }
        }
        // الترجمة
        Compiler compiler;
        auto instructions = compiler.compile(std::move(program));

        if (instructions.empty())
        {
            std::cerr << "⚠️  لم يتم توليد أي تعليمات. قد يكون البرنامج فارغاً." << std::endl;
        }

        std::cout << "🔧 جاري توليد المخرجات..." << std::endl;

        // توليد أسماء الملفات الناتجة
        std::string baseName = filename;
        size_t last_dot = filename.find_last_of('.');
        if (last_dot != std::string::npos)
        {
            baseName = filename.substr(0, last_dot);
        }

        // إضافة لاحقة لتجنب الكتابة فوق الملف الأصلي
        baseName = baseName + "_output";

        // توليد المخرجات المطلوبة
        bool generatedAnyOutput = false;

        if (generateIr)
        {
            std::string irFile = baseName + "_intermediate.txt";
            compiler.generateIntermediateCode(irFile);
            std::cout << "📄 تم توليد الكود الوسيط: " << irFile << std::endl;
            generatedAnyOutput = true;
        }

        if (generateC)
        {
            std::string cFile = baseName + ".c";
            compiler.generateCCode(cFile);
            std::cout << "📄 تم توليد كود C: " << cFile << std::endl;
            generatedAnyOutput = true;
        }

        if (generateAsm)
        {
            std::string asmFile = baseName + ".asm";
            compiler.generateAssembly(asmFile);
            std::cout << "📄 تم توليد كود التجميع: " << asmFile << std::endl;
            generatedAnyOutput = true;
        }

        // عرض إحصائيات وتعليمات وسيطة
        if (!instructions.empty())
        {
            compiler.displayInstructions();
        }

        if (generatedAnyOutput)
        {
            std::cout << "🎉 الترجمة تمت بنجاح!" << std::endl;
            std::cout << "📊 الملفات الناتجة:" << std::endl;
            if (generateIr)
                std::cout << "   - " << baseName << "_intermediate.txt (الكود الوسيط)" << std::endl;
            if (generateC)
                std::cout << "   - " << baseName << ".c (كود C)" << std::endl;
            if (generateAsm)
                std::cout << "   - " << baseName << ".asm (كود Assembly)" << std::endl;
        }
        else
        {
            std::cout << "ℹ️  لم يتم توليد أي مخرجات. تحقق من الخيارات المحددة." << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "❌ خطأ استثنائي: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
```


---

## 📄 `./Compiler/src/Parser.cpp`

```cpp
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

```


---

## 📄 `./Compiler/src/AST.h`

```cpp
#ifndef AST_H
#define AST_H

#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include "Lexer.h"

// Forward declarations to resolve circular dependencies
struct VariableNode;
struct IndexAccessNode;
struct TypeNode;

// أنواع العقد في الشجرة التجريدية
enum class NodeType
{
    PROGRAM,
    VARIABLE_DECL,
    CONSTANT_DECL,
    ASSIGNMENT,
    PRINT,
    READ,
    IF,
    WHILE,
    REPEAT,
    FOR,
    BINARY_OP,
    UNARY_OP,
    LITERAL,
    VARIABLE,
    INDEX_ACCESS, // Accessing an array element, e.g., a[i]
    EXPRESSION
};

struct ASTNode
{
    NodeType type;
    virtual ~ASTNode() = default;

    virtual std::string toString() const = 0;
    virtual std::string getTypeName() const = 0;
};

struct ProgramNode : public ASTNode
{
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> declarations;
    std::vector<std::unique_ptr<ASTNode>> statements;

    ProgramNode() { type = NodeType::PROGRAM; }

    // constructor with name
    ProgramNode(const std::string &programName) : name(programName)
    {
        type = NodeType::PROGRAM;
    }

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

    // نوع المتغير (قد يكون Primitive أو Array أو Record)
    std::unique_ptr<ASTNode> typeNode; // will point to a Type node (we reuse ASTNode hierarchy)

    VariableDeclarationNode() { type = NodeType::VARIABLE_DECL; }
    VariableDeclarationNode(const std::string &n)
        : name(n), typeNode(nullptr) { type = NodeType::VARIABLE_DECL; }

    std::string toString() const override
    {
        std::string result = "تعريف متغير: " + name;
        if (typeNode)
        {
            result += " : " + typeNode->toString();
        }
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

    ConstantDeclarationNode() { type = NodeType::CONSTANT_DECL; }
    ConstantDeclarationNode(const std::string &n)
        : name(n) { type = NodeType::CONSTANT_DECL; }

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
    std::unique_ptr<ASTNode> left; // Can be a VariableNode or IndexAccessNode
    std::unique_ptr<ASTNode> value;

    AssignmentNode() { type = NodeType::ASSIGNMENT; }
    AssignmentNode(std::unique_ptr<ASTNode> l) : left(std::move(l))
    {
        type = NodeType::ASSIGNMENT;
    }

    std::string toString() const override;

    std::string getTypeName() const override
    {
        return "AssignmentNode";
    }
};

struct PrintNode : public ASTNode
{
    std::unique_ptr<ASTNode> expression;
    PrintNode() { type = NodeType::PRINT; }

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
    ReadNode() { type = NodeType::READ; }
    ReadNode(const std::string &var) : variableName(var)
    {
        type = NodeType::READ;
    }

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

    IfNode() { type = NodeType::IF; }

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

    WhileNode() { type = NodeType::WHILE; }

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

    RepeatNode() { type = NodeType::REPEAT; }

    std::string toString() const override
    {
        return "حلقة كرر-حتى: جسم(" + std::to_string(body.size()) + " جملة)";
    }

    std::string getTypeName() const override
    {
        return "RepeatNode";
    }
};

struct ForNode : public ASTNode
{
    std::string iteratorName;
    std::unique_ptr<ASTNode> startValue;
    std::unique_ptr<ASTNode> endValue;
    std::unique_ptr<ASTNode> stepValue; // Can be nullptr
    std::vector<std::unique_ptr<ASTNode>> body;

    ForNode()
    {
        type = NodeType::FOR;
    }

    std::string toString() const override
    {
        std::string result = "حلقة تكرار (For): " + iteratorName;
        result += " من [بداية] الى [نهاية]";
        if (stepValue)
        {
            result += " اضف [خطوة]";
        }
        result += " جسم(" + std::to_string(body.size()) + " جملة)";
        return result;
    }

    std::string getTypeName() const override
    {
        return "ForNode";
    }
};

struct BinaryOpNode : public ASTNode
{
    TokenType op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    BinaryOpNode(TokenType operation) : op(operation)
    {
        type = NodeType::BINARY_OP;
    }

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

    UnaryOpNode(TokenType operation) : op(operation)
    {
        type = NodeType::UNARY_OP;
    }

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

    LiteralNode(TokenType literalTypeParam, const std::string &val)
        : literalType(literalTypeParam), value(val)
    {
        this->type = NodeType::LITERAL;
    }

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

    VariableNode(const std::string &n) : name(n)
    {
        type = NodeType::VARIABLE;
    }

    // default constructor
    VariableNode() : name("")
    {
        type = NodeType::VARIABLE;
    }

    std::string toString() const override
    {
        if (name.empty())
        {
            return "متغير: [غير مسمى]";
        }
        return "متغير: " + name;
    }

    std::string getTypeName() const override
    {
        return "VariableNode";
    }
};

struct IndexAccessNode : public ASTNode
{
    std::unique_ptr<ASTNode> variable; // The array variable
    std::unique_ptr<ASTNode> index;    // The index expression

    IndexAccessNode()
    {
        type = NodeType::INDEX_ACCESS;
    }

    std::string toString() const override;

    std::string getTypeName() const override
    {
        return "IndexAccessNode";
    }
};

struct FieldAccessNode : public ASTNode
{
    std::unique_ptr<ASTNode> record; // The record variable
    std::string fieldName;           // The field name

    FieldAccessNode()
    {
        type = NodeType::EXPRESSION;
    }

    std::string toString() const override
    {
        return "وصول لحقل: " + fieldName;
    }

    std::string getTypeName() const override
    {
        return "FieldAccessNode";
    }
};

// Procedure-related nodes
struct ProcedureParameterNode
{
    std::string name;
    std::unique_ptr<TypeNode> type;
    bool isByReference; // true for بالمرجع, false for بالقيمة

    ProcedureParameterNode(const std::string &n, std::unique_ptr<TypeNode> t, bool byRef = false)
        : name(n), type(std::move(t)), isByReference(byRef) {}
};

struct ProcedureDeclarationNode : public ASTNode
{
    std::string name;
    std::vector<ProcedureParameterNode> parameters;
    std::vector<std::unique_ptr<ASTNode>> body;
    std::unique_ptr<TypeNode> returnType; // nullptr if no return type

    ProcedureDeclarationNode() { type = NodeType::EXPRESSION; }
    ProcedureDeclarationNode(const std::string &n) : name(n), returnType(nullptr) { type = NodeType::EXPRESSION; }

    std::string toString() const override
    {
        return "تعريف إجراء: " + name + " مع " + std::to_string(parameters.size()) + " معاملات";
    }

    std::string getTypeName() const override
    {
        return "ProcedureDeclarationNode";
    }
};

struct ProcedureCallNode : public ASTNode
{
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> arguments;

    ProcedureCallNode() { type = NodeType::EXPRESSION; }
    ProcedureCallNode(const std::string &n) : name(n) { type = NodeType::EXPRESSION; }

    std::string toString() const override
    {
        return "استدعاء إجراء: " + name + " مع " + std::to_string(arguments.size()) + " معاملات";
    }

    std::string getTypeName() const override
    {
        return "ProcedureCallNode";
    }
};

struct ReturnNode : public ASTNode
{
    std::unique_ptr<ASTNode> value; // nullptr if no return value

    ReturnNode() { type = NodeType::EXPRESSION; }

    std::string toString() const override
    {
        return "جملة إرجاع";
    }

    std::string getTypeName() const override
    {
        return "ReturnNode";
    }
};

// Type nodes
struct TypeNode : public ASTNode
{
    TypeNode() { type = NodeType::EXPRESSION; }
    virtual ~TypeNode() = default;
};

struct PrimitiveTypeNode : public TypeNode
{
    std::string name; // e.g., 'صحيح', 'خيط'
    PrimitiveTypeNode(const std::string &n) : name(n) {}
    std::string toString() const override { return "نوع: " + name; }
    std::string getTypeName() const override { return "PrimitiveTypeNode"; }
};

struct ArrayTypeNode : public TypeNode
{
    std::unique_ptr<TypeNode> elementType;
    int length;
    ArrayTypeNode() : elementType(nullptr), length(0) {}
    std::string toString() const override
    {
        return "قائمة[" + std::to_string(length) + "] من " + (elementType ? elementType->toString() : "?");
    }
    std::string getTypeName() const override { return "ArrayTypeNode"; }
};

struct FieldDecl
{
    std::string name;
    std::unique_ptr<TypeNode> type;
};

struct RecordTypeNode : public TypeNode
{
    std::vector<FieldDecl> fields;
    RecordTypeNode() {}
    std::string toString() const override
    {
        std::string s = "سجل{";
        for (size_t i = 0; i < fields.size(); ++i)
        {
            s += fields[i].name + ":" + (fields[i].type ? fields[i].type->toString() : "?");
            if (i + 1 < fields.size())
                s += ", ";
        }
        s += "}";
        return s;
    }
    std::string getTypeName() const override { return "RecordTypeNode"; }
};

// دالة مساعدة لعرض AST
class ASTPrinter
{
public:
    static void print(const std::unique_ptr<ASTNode> &node, const std::string &prefix = "", bool isLast = true)
    {
        if (!node)
            return;

        std::cout << prefix;
        std::cout << (isLast ? "└── " : "├── ");
        std::cout << node->toString() << " [" << node->getTypeName() << "]" << std::endl;

        // حساب البادئة للأبناء
        std::string newPrefix = prefix + (isLast ? "    " : "│   ");

        // عرض الأبناء حسب نوع العقدة
        if (auto program = dynamic_cast<ProgramNode *>(node.get()))
        {
            // عرض التعريفات
            for (size_t i = 0; i < program->declarations.size(); i++)
            {
                bool lastDecl = (i == program->declarations.size() - 1) && program->statements.empty();
                print(program->declarations[i], newPrefix, lastDecl);
            }
            // عرض الجمل
            for (size_t i = 0; i < program->statements.size(); i++)
            {
                bool lastStmt = (i == program->statements.size() - 1);
                print(program->statements[i], newPrefix, lastStmt);
            }
        }
        else if (auto ifStmt = dynamic_cast<IfNode *>(node.get()))
        {
            print(ifStmt->condition, newPrefix, false);
            // عرض فرع then
            if (!ifStmt->thenBranch.empty())
            {
                std::cout << newPrefix << "├── then:" << std::endl;
                for (size_t i = 0; i < ifStmt->thenBranch.size(); i++)
                {
                    bool lastThen = (i == ifStmt->thenBranch.size() - 1) && ifStmt->elseBranch.empty();
                    print(ifStmt->thenBranch[i], newPrefix + "│   ", lastThen);
                }
            }
            // عرض فرع else إن وجد
            if (!ifStmt->elseBranch.empty())
            {
                std::cout << newPrefix << "├── else:" << std::endl;
                for (size_t i = 0; i < ifStmt->elseBranch.size(); i++)
                {
                    bool lastElse = (i == ifStmt->elseBranch.size() - 1);
                    print(ifStmt->elseBranch[i], newPrefix + "│   ", lastElse);
                }
            }
        }
        else if (auto whileStmt = dynamic_cast<WhileNode *>(node.get()))
        {
            print(whileStmt->condition, newPrefix, false);
            // عرض جسم الحلقة
            if (!whileStmt->body.empty())
            {
                std::cout << newPrefix << "└── body:" << std::endl;
                for (size_t i = 0; i < whileStmt->body.size(); i++)
                {
                    bool lastBody = (i == whileStmt->body.size() - 1);
                    print(whileStmt->body[i], newPrefix + "    ", lastBody);
                }
            }
        }
        else if (auto repeatStmt = dynamic_cast<RepeatNode *>(node.get()))
        {
            // عرض جسم الحلقة
            if (!repeatStmt->body.empty())
            {
                std::cout << newPrefix << "├── body:" << std::endl;
                for (size_t i = 0; i < repeatStmt->body.size(); i++)
                {
                    bool lastBody = (i == repeatStmt->body.size() - 1);
                    print(repeatStmt->body[i], newPrefix + "│   ", lastBody);
                }
            }
            print(repeatStmt->condition, newPrefix, true);
        }
        else if (auto forStmt = dynamic_cast<ForNode *>(node.get()))
        {
            std::cout << newPrefix << "├── iterator: " << forStmt->iteratorName << std::endl;
            print(forStmt->startValue, newPrefix, false);
            print(forStmt->endValue, newPrefix, false);
            if (forStmt->stepValue)
            {
                print(forStmt->stepValue, newPrefix, false);
            }
            // Print body
            if (!forStmt->body.empty())
            {
                std::cout << newPrefix << "└── body:" << std::endl;
                for (size_t i = 0; i < forStmt->body.size(); i++)
                {
                    bool lastBody = (i == forStmt->body.size() - 1);
                    print(forStmt->body[i], newPrefix + "    ", lastBody);
                }
            }
        }
        else if (auto binaryOp = dynamic_cast<BinaryOpNode *>(node.get()))
        {
            print(binaryOp->left, newPrefix, false);
            print(binaryOp->right, newPrefix, true);
        }
        else if (auto unaryOp = dynamic_cast<UnaryOpNode *>(node.get()))
        {
            print(unaryOp->operand, newPrefix, true);
        }
        else if (auto varDecl = dynamic_cast<VariableDeclarationNode *>(node.get()))
        {
            if (varDecl->initialValue)
            {
                print(varDecl->initialValue, newPrefix, true);
            }
        }
        else if (auto constDecl = dynamic_cast<ConstantDeclarationNode *>(node.get()))
        {
            if (constDecl->value)
            {
                print(constDecl->value, newPrefix, true);
            }
        }
        else if (auto assignment = dynamic_cast<AssignmentNode *>(node.get()))
        {
            print(assignment->left, newPrefix, false);
            if (assignment->value)
            {
                print(assignment->value, newPrefix, true);
            }
        }
        else if (auto indexAccess = dynamic_cast<IndexAccessNode *>(node.get()))
        {
            print(indexAccess->variable, newPrefix, false);
            print(indexAccess->index, newPrefix, true);
        }
        else if (auto printStmt = dynamic_cast<PrintNode *>(node.get()))
        {
            if (printStmt->expression)
            {
                print(printStmt->expression, newPrefix, true);
            }
        }
    }

    // overload for raw pointers
    static void print(ASTNode *node, const std::string &prefix = "", bool isLast = true)
    {
        if (!node)
            return;
        print(std::unique_ptr<ASTNode>(node), prefix, isLast);
    }
};

#endif
```


---

## 📄 `./Compiler/src/Compiler.h`

```cpp
#ifndef COMPILER_H
#define COMPILER_H

#include "Parser.h"
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <memory>

enum class InstructionType
{
    LOAD,
    STORE,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    JMP,
    JZ,
    JNZ,
    CALL,
    RET,
    PRINT,
    READ,
    HALT,
    LABEL,
    CMP,
    JE,
    JNE,
    JG,
    JL,
    JGE,
    JLE,
    OR,
    AND,
    LOAD_INDEXED, // op1 = op2[op3] -> target = base[index]
    STORE_INDEXED // op1[op2] = op3 -> base[index] = source
};

struct Instruction
{
    InstructionType type;
    std::string operand1;
    std::string operand2;
    std::string operand3;

    Instruction(InstructionType t, const std::string &op1 = "",
                const std::string &op2 = "", const std::string &op3 = "")
        : type(t), operand1(op1), operand2(op2), operand3(op3) {}

    std::string toString() const
    {
        switch (type)
        {
        case InstructionType::LOAD:
            return "LOAD " + operand1 + ", " + operand2;
        case InstructionType::STORE:
            return "STORE " + operand1 + ", " + operand2;
        case InstructionType::ADD:
            if (operand3.empty())
                return "ADD " + operand1 + ", " + operand2;
            else
                return "ADD " + operand1 + ", " + operand2 + ", " + operand3;
        case InstructionType::SUB:
            if (operand3.empty())
                return "SUB " + operand1 + ", " + operand2;
            else
                return "SUB " + operand1 + ", " + operand2 + ", " + operand3;
        case InstructionType::MUL:
            if (operand3.empty())
                return "MUL " + operand1 + ", " + operand2;
            else
                return "MUL " + operand1 + ", " + operand2 + ", " + operand3;
        case InstructionType::DIV:
            if (operand3.empty())
                return "DIV " + operand1 + ", " + operand2;
            else
                return "DIV " + operand1 + ", " + operand2 + ", " + operand3;
        case InstructionType::MOD:
            if (operand3.empty())
                return "MOD " + operand1 + ", " + operand2;
            else
                return "MOD " + operand1 + ", " + operand2 + ", " + operand3;
        case InstructionType::JMP:
            return "JMP " + operand1;
        case InstructionType::JZ:
            return "JZ " + operand1 + ", " + operand2;
        case InstructionType::JNZ:
            return "JNZ " + operand1 + ", " + operand2;
        case InstructionType::PRINT:
            return "PRINT " + operand1 + ", " + operand2;
        case InstructionType::READ:
            return "READ " + operand1;
        case InstructionType::HALT:
            return "HALT";
        case InstructionType::LABEL:
            return operand1 + ":";
        case InstructionType::CMP:
            return "CMP " + operand1 + ", " + operand2;
        case InstructionType::JE:
            return "JE " + operand1;
        case InstructionType::JNE:
            return "JNE " + operand1;
        case InstructionType::JG:
            return "JG " + operand1;
        case InstructionType::JL:
            return "JL " + operand1;
        case InstructionType::JGE:
            return "JGE " + operand1;
        case InstructionType::JLE:
            return "JLE " + operand1;
        case InstructionType::OR:
            if (operand3.empty())
                return "OR " + operand1 + ", " + operand2;
            else
                return "OR " + operand1 + ", " + operand2 + ", " + operand3;
        case InstructionType::AND:
            if (operand3.empty())
                return "AND " + operand1 + ", " + operand2;
            else
                return "AND " + operand1 + ", " + operand2 + ", " + operand3;
        case InstructionType::LOAD_INDEXED:
            return "LOAD_INDEXED " + operand1 + ", " + operand2 + "[" + operand3 + "]";
        case InstructionType::STORE_INDEXED:
            return "STORE_INDEXED " + operand1 + "[" + operand2 + "]" + ", " + operand3;
        default:
            return "UNKNOWN";
        }
    }
};

class Compiler
{
private:
    std::vector<Instruction> instructions;
    std::unordered_map<std::string, std::string> symbolTable;
    std::unordered_map<std::string, std::string> recordDefs;    // structName -> C struct definition body
    std::unordered_map<std::string, std::string> procedureDefs; // procedureName -> procedure label
    int labelCounter;
    int tempVarCounter;
    std::vector<std::string> stringLiterals;
    std::unordered_map<std::string, std::string> stringToLabel;

    std::string generateLabel();
    std::string generateTempVar();
    void emit(InstructionType type, const std::string &op1 = "",
              const std::string &op2 = "", const std::string &op3 = "");

    std::string getStringLabel(const std::string &literal);

    void compileProgram(ProgramNode *program);
    void compileStatement(ASTNode *statement);
    void compileVariableDeclaration(VariableDeclarationNode *node);
    void compileConstantDeclaration(ConstantDeclarationNode *node);
    void compileAssignment(AssignmentNode *node);
    void compilePrint(PrintNode *node);
    void compileRead(ReadNode *node);
    void compileIf(IfNode *node);
    void compileWhile(WhileNode *node);
    void compileRepeat(RepeatNode *node);
    void compileFor(ForNode *node);
    void compileProcedureDeclaration(ProcedureDeclarationNode *node);
    void compileProcedureCall(ProcedureCallNode *node);
    void compileReturnStatement(ReturnNode *node);
    std::string compileFieldAccess(FieldAccessNode *node);
    std::string compileExpression(ASTNode *expr);
    std::string compileIndexAccess(IndexAccessNode *node);

public:
    Compiler();
    std::vector<Instruction> compile(std::unique_ptr<ProgramNode> program);
    void generateAssembly(const std::string &filename);
    void generateCCode(const std::string &filename);
    void generateIntermediateCode(const std::string &filename);
    void displayInstructions() const;

    // دالة مساعدة جديدة للتحقق من وجود أخطاء
    bool hasErrors() const { return instructions.empty(); }
};

#endif
```


---

## 📄 `./Compiler/src/Lexer.h`

```cpp
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
```


---

## 📄 `./Compiler/src/Parser.h`

```cpp
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
```


---

## 📄 `./Compiler/CMakeLists.txt`

```text
cmake_minimum_required(VERSION 3.10)
project(ArabicCompiler)

# الحد الأدنى من الإعدادات
set(CMAKE_CXX_STANDARD 17)

# ملفات المصدر
set(SOURCE_FILES
    src/Lexer.cpp
    src/Parser.cpp
    src/Compiler.cpp
    src/AST.cpp
    src/main.cpp
)

add_executable(ArabicCompiler ${SOURCE_FILES})

# نسخ الملف التنفيذي
add_custom_command(TARGET ArabicCompiler POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ArabicCompiler> ${CMAKE_BINARY_DIR}/
)
```


---

## 📄 `./Compiler/build.bat`

```batch
@echo off
chcp 65001 >nul
echo ===========================================
echo    مترجم اللغة العربية - بناء المشروع
echo ===========================================

if not exist "build" (
    echo إنشاء مجلد البناء...
    mkdir build
)

cd build

echo تكوين المشروع باستخدام CMake...
cmake ..

if %errorlevel% neq 0 (
    echo ❌ فشل تكوين المشروع!
    exit /b 1
)

echo بناء المشروع...
cmake --build . --config Release

if %errorlevel% equ 0 (
    echo.
    echo ✅ تم بناء المترجم بنجاح!
    echo 📁 الملف التنفيذي: build\ArabicCompiler.exe
    echo.
    echo 🚀 يمكنك الآن استخدام:
    echo   ArabicCompiler.exe examples\hello.arabic
) else (
    echo ❌ فشل بناء المشروع!
    exit /b 1
)

cd ..
pause
```


---

## 📄 `./Compiler/tests/test_grammar_rules.cpp`

```cpp
// اختبارات شاملة لقواعد اللغة العربية
// Comprehensive tests for Arabic language grammar rules

#include <iostream>
#include <cassert>
#include <string>
#include <memory>
#include "../src/Lexer.h"
#include "../src/Parser.h"
#include "../src/Compiler.h"

// Test helper functions
void testLexer(const std::string& code, const std::string& testName) {
    std::cout << "\n=== اختبار Lexer: " << testName << " ===" << std::endl;
    try {
        Lexer lexer(code);
        auto tokens = lexer.tokenize();
        std::cout << "✓ نجح: تم الحصول على " << tokens.size() << " رمز" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ فشل: " << e.what() << std::endl;
    }
}

void testParser(const std::string& code, const std::string& testName) {
    std::cout << "\n=== اختبار Parser: " << testName << " ===" << std::endl;
    try {
        Lexer lexer(code);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        auto program = parser.parse();
        if (program) {
            std::cout << "✓ نجح: تم بناء AST للبرنامج: " << program->name << std::endl;
        } else {
            std::cout << "✗ فشل: لم يتم بناء AST" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "✗ فشل: " << e.what() << std::endl;
    }
}

void testCompiler(const std::string& code, const std::string& testName) {
    std::cout << "\n=== اختبار Compiler: " << testName << " ===" << std::endl;
    try {
        Lexer lexer(code);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        auto program = parser.parse();
        if (program) {
            Compiler compiler;
            auto instructions = compiler.compile(std::move(program));
            std::cout << "✓ نجح: تم توليد " << instructions.size() << " تعليمة" << std::endl;
        } else {
            std::cout << "✗ فشل: لم يتم بناء AST" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "✗ فشل: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  اختبارات شاملة لقواعد اللغة العربية                        ║" << std::endl;
    std::cout << "║  Comprehensive Grammar Rules Tests                         ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;

    // Test 1: Basic Program
    testParser("برنامج اختبار؛", "برنامج بسيط");

    // Test 2: Variable Declaration
    testParser("برنامج اختبار؛ متغير س = 10؛", "تعريف متغير");

    // Test 3: Constant Declaration
    testParser("برنامج اختبار؛ ثابت ط = 3.14؛", "تعريف ثابت");

    // Test 4: Print Statement
    testParser("برنامج اختبار؛ اطبع \"مرحبا\"؛", "جملة طباعة");

    // Test 5: If Statement
    testParser("برنامج اختبار؛ اذا (1 > 0) فان اطبع \"صح\"؛", "جملة شرطية");

    // Test 6: While Loop
    testParser("برنامج اختبار؛ متغير س = 0؛ طالما (س < 10) استمر س = س + 1؛", "حلقة while");

    // Test 7: For Loop
    testParser("برنامج اختبار؛ كرر (س = 1 الى 10) اطبع س؛ نهاية؛", "حلقة for");

    // Test 8: Array Type
    testParser("برنامج اختبار؛ متغير قائمة : قائمة[10] من صحيح؛", "نوع قائمة");

    // Test 9: Record Type
    testParser("برنامج اختبار؛ متغير سجل : سجل { س : صحيح؛ }؛", "نوع سجل");

    // Test 10: Expression with operators
    testParser("برنامج اختبار؛ متغير ن = 5 + 3 * 2؛", "تعبير حسابي");

    std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  انتهت الاختبارات                                          ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;

    return 0;
}


```


---

## 📄 `./IDE/MainForm.cs`

```csharp
using System;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;
using System.Drawing;
using System.Text;
using System.ComponentModel;
using System.Text.RegularExpressions;
using Timer = System.Windows.Forms.Timer;
using System.Text.Json;
using System.Threading.Tasks;
using System.Linq;

namespace ArabicIDE
{
    public partial class MainForm : Form
    {
        // runtime-initialized paths (initialized in constructor)
        private string exeDir = "";
        private string? repoRoot = null;
        private string script = "";
        private ToolStripButton? openButton;
        private ToolStripButton? saveButton;
        private ToolStripButton? saveAsButton;
        private ToolStripButton? newButton;
        private ToolStripButton? compileButton;
        private ToolStripButton? runButton;
        private ToolStripButton? runAllButton;
        private ToolStripButton? debugLexerButton;
        private ComboBox? outputTypeComboBox;
        private RichTextBox? editor;
        private RichTextBox? outputBox;
        private string settingsFile;
        private ToolStripStatusLabel? statusLabel;
        private ToolStripStatusLabel? lineLabel;
        private StatusStrip? statusStrip;

    private string compilerPath = "";
        private string currentFile = "";
        private bool isModified = false;

        private System.Windows.Forms.Timer? syntaxHighlightingTimer; // تم التحديد الصريح للنوع
        private bool isHighlighting = false;

        // Define colors
        private Color keywordColor = Color.Blue;
        private Color identifierColor = Color.Black;
        private Color stringColor = Color.Brown;
        private Color numberColor = Color.Green;
        private Color commentColor = Color.Gray;

        public MainForm()
        {
            // تسجيل ترميز Windows-1256
            Encoding.RegisterProvider(CodePagesEncodingProvider.Instance);
            
            InitializeComponent();
            SetupComponents();
            // initialize runtime paths (exeDir, repoRoot, script)
            exeDir = Path.GetDirectoryName(Application.ExecutablePath) ?? Directory.GetCurrentDirectory();

            // Find repository root by walking up until we find a 'scripts' folder (max 6 levels)
            string dir = exeDir;
            for (int i = 0; i < 6; i++)
            {
                string candidate = Path.Combine(dir, "scripts");
                if (Directory.Exists(candidate))
                {
                    repoRoot = Directory.GetParent(candidate)?.FullName; // parent of scripts
                    break;
                }
                var parent = Directory.GetParent(dir);
                if (parent == null) break;
                dir = parent.FullName;
            }
            // fallback to known relative path (project root assumption)
            if (repoRoot == null)
            {
                repoRoot = Path.GetFullPath(Path.Combine(exeDir, "..", "..", ".."));
            }

            script = Path.Combine(repoRoot, "scripts", "run_end_to_end.ps1");
            // settings file path
            settingsFile = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "ide_settings.json");
            // prefer built compiler in common locations
            var candidates = new[] {
                Path.Combine(repoRoot ?? exeDir, "Compiler", "build", "Release", "ArabicCompiler.exe"),
                Path.Combine(repoRoot ?? exeDir, "Compiler", "build", "ArabicCompiler.exe"),
                Path.Combine(repoRoot ?? exeDir, "Compiler", "ArabicCompiler.exe"),
                Path.Combine(exeDir, "ArabicCompiler.exe"),
                Path.Combine(repoRoot ?? exeDir, "ArabicCompiler.exe")
            };
            compilerPath = candidates.FirstOrDefault(p => File.Exists(p)) ?? "ArabicCompiler.exe";
            LoadSettings();
            UpdateTitle();
            SetupSyntaxHighlighting();
        }

        private void InitializeComponent()
        {
            // هذا لمنع التعارض مع التصميم اليدوي
            SuspendLayout();
            ResumeLayout(false);
        }

        private void SetupComponents()
        {
            this.Text = "بيئة التطوير العربية - Arabic IDE";
            this.Size = new Size(1200, 800);
            this.StartPosition = FormStartPosition.CenterScreen;
            this.Font = new Font("Tahoma", 9);
            // تم إزالة السطر التالي لمنع مشكلة الأيقونة
            // this.Icon = SystemIcons.Application;

            SetupToolStrip();
            SetupMainLayout();
            SetupStatusBar();
            SetupShortcuts();
        }

        private void SetupToolStrip()
        {
            var mainToolStrip = new ToolStrip();
            mainToolStrip.Dock = DockStyle.Top;
            mainToolStrip.GripStyle = ToolStripGripStyle.Hidden;
            mainToolStrip.BackColor = SystemColors.Control;
            mainToolStrip.RenderMode = ToolStripRenderMode.System;

            newButton = CreateToolStripButton("جديد", "📄", "Ctrl+N");
            openButton = CreateToolStripButton("فتح", "📂", "Ctrl+O");
            saveButton = CreateToolStripButton("حفظ", "💾", "Ctrl+S");
            saveAsButton = CreateToolStripButton("حفظ كـ", "💾", "Ctrl+Shift+S");

            newButton!.Click += NewButton_Click;
            openButton!.Click += OpenButton_Click;
            saveButton!.Click += SaveButton_Click;
            saveAsButton!.Click += SaveAsButton_Click;

            mainToolStrip.Items.Add(newButton);
            mainToolStrip.Items.Add(openButton);
            mainToolStrip.Items.Add(saveButton);
            mainToolStrip.Items.Add(saveAsButton);
            mainToolStrip.Items.Add(new ToolStripSeparator());

            runButton = CreateToolStripButton("ترجمة وتنفيذ", "▶️", "F5");
            compileButton = CreateToolStripButton("ترجمة فقط", "🔧", "Ctrl+B");

            debugLexerButton = CreateToolStripButton("تشخيص Lexer", "🐞", "");
            // Make it toggleable
            debugLexerButton.CheckOnClick = true;
            debugLexerButton.CheckedChanged += (s, e) => SaveSettings();

            runAllButton = CreateToolStripButton("تشغيل الأمثلة", "📦", "تشغيل جميع الأمثلة والاطلاع على السجلات");
            runAllButton.Click += RunAllExamples_Click;

            var loadExamplesButton = CreateToolStripButton("تحميل مثال", "📚", "تحميل أحد الأمثلة المتاحة");
            loadExamplesButton.Click += LoadExamples_Click;

            runButton!.Click += RunButton_Click;
            compileButton!.Click += CompileButton_Click;

            mainToolStrip.Items.Add(runButton);
            mainToolStrip.Items.Add(compileButton);
            mainToolStrip.Items.Add(debugLexerButton);
            mainToolStrip.Items.Add(runAllButton);
            mainToolStrip.Items.Add(loadExamplesButton);
            mainToolStrip.Items.Add(new ToolStripSeparator());

            mainToolStrip.Items.Add(new ToolStripLabel("نوع الإخراج:"));
            outputTypeComboBox = new ComboBox();
            outputTypeComboBox.Items.AddRange(new object[] { "كود Assembly", "كود C", "كود وسيط", "جميع المخرجات" });
            outputTypeComboBox.SelectedIndex = 0;
            outputTypeComboBox.Width = 120;
            outputTypeComboBox.DropDownStyle = ComboBoxStyle.DropDownList;

            var comboHost = new ToolStripControlHost(outputTypeComboBox);
            mainToolStrip.Items.Add(comboHost);

            this.Controls.Add(mainToolStrip);
        }

        private void SetupMainLayout()
        {
            var splitContainer = new SplitContainer();
            splitContainer.Dock = DockStyle.Fill;
            splitContainer.Orientation = Orientation.Vertical;
            splitContainer.SplitterDistance = 600;
            splitContainer.SplitterWidth = 5;

            // لوحة المحرر
            var editorPanel = new Panel();
            editorPanel.Dock = DockStyle.Fill;
            editorPanel.Padding = new Padding(5);

            var editorLabel = new Label();
            editorLabel.Dock = DockStyle.Top;
            editorLabel.Text = "المحرر:";
            editorLabel.Font = new Font("Tahoma", 9, FontStyle.Bold);
            editorLabel.TextAlign = ContentAlignment.MiddleRight;
            editorLabel.Height = 25;

            editor = new RichTextBox();
            editor.Dock = DockStyle.Fill;
            editor.Font = new Font("Consolas", 11);
            editor.Text = GetDefaultCode();
            editor.WordWrap = false;
            editor.ScrollBars = RichTextBoxScrollBars.Both;
            editor.TextChanged += Editor_TextChanged;
            editor.KeyDown += Editor_KeyDown;
            editor.SelectionChanged += Editor_SelectionChanged;

            editorPanel.Controls.Add(editor);
            editorPanel.Controls.Add(editorLabel);

            // لوحة الإخراج
            var outputPanel = new Panel();
            outputPanel.Dock = DockStyle.Fill;
            outputPanel.Padding = new Padding(5);

            var outputLabel = new Label();
            outputLabel.Dock = DockStyle.Top;
            outputLabel.Text = "الإخراج:";
            outputLabel.Font = new Font("Tahoma", 9, FontStyle.Bold);
            outputLabel.TextAlign = ContentAlignment.MiddleRight;
            outputLabel.Height = 25;

            outputBox = new RichTextBox();
            outputBox.Dock = DockStyle.Fill;
            outputBox.Font = new Font("Consolas", 9);
            outputBox.BackColor = Color.FromArgb(30, 30, 30);
            outputBox.ForeColor = Color.LightGreen;
            outputBox.ReadOnly = true;
            // Allow detecting file:// links so users can click to open logs
            outputBox.DetectUrls = true;
            outputBox.LinkClicked += OutputBox_LinkClicked;
            outputBox.WordWrap = false;
            outputBox.ScrollBars = RichTextBoxScrollBars.Both;

            outputPanel.Controls.Add(outputBox);
            outputPanel.Controls.Add(outputLabel);

            splitContainer.Panel1.Controls.Add(editorPanel);
            splitContainer.Panel2.Controls.Add(outputPanel);

            this.Controls.Add(splitContainer);
        }

        private void SetupStatusBar()
        {
            statusStrip = new StatusStrip();
            statusStrip.Dock = DockStyle.Bottom;

            statusLabel = new ToolStripStatusLabel();
            statusLabel.Text = "جاهز";
            statusLabel.Spring = true;

            lineLabel = new ToolStripStatusLabel();
            lineLabel.Text = "السطر: 1, العمود: 1";

            statusStrip.Items.Add(statusLabel);
            statusStrip.Items.Add(lineLabel);

            this.Controls.Add(statusStrip);
        }

        private ToolStripButton CreateToolStripButton(string text, string emoji, string tooltip)
        {
            var button = new ToolStripButton();
            button.Text = $"{emoji} {text}";
            button.ToolTipText = $"{text} ({tooltip})";
            button.DisplayStyle = ToolStripItemDisplayStyle.Text;
            button.AutoSize = true;
            return button;
        }

        private string GetDefaultCode()
        {
            return @"// برنامج مثال باللغة العربية
// مثال شامل يوضح الميزات الأساسية

برنامج مثال_شامل ؛

// تعريف الثوابت
ثابت ط = 3.14 ؛
ثابت الحد_الأقصى = 100 ؛

// تعريف المتغيرات
متغير س : عدد صحيح ؛
متغير ناتج : عدد صحيح ؛
متغير أرقام : مصفوفة[5] من عدد صحيح ؛

// تعيين القيم
س = 10 ؛
ناتج = س * 2 + 5 ؛
أرقام[0] = 10 ؛
أرقام[1] = 20 ؛

// جملة شرطية
إذا ناتج > 15 فإن
    اطبع ""الناتج كبير: "" ؛
    اطبع ناتج ؛
وإلا
    اطبع ""الناتج صغير: "" ؛
    اطبع ناتج ؛
نهاية ؛

// حلقة تكرار
لـ (ع = 1 إلى 5)
    اطبع ""العدد: "" ؛
    اطبع ع ؛
نهاية ؛

اطبع ""نهاية البرنامج"" ؛";
        }

        private void SetupShortcuts()
        {
            this.KeyPreview = true;
            this.KeyDown += MainForm_KeyDown;
        }

        private void SetupSyntaxHighlighting()
        {
            syntaxHighlightingTimer = new System.Windows.Forms.Timer(); // استخدام النوع الصريح
            syntaxHighlightingTimer.Interval = 500; // 0.5 second delay
            syntaxHighlightingTimer.Tick += (sender, e) => {
                syntaxHighlightingTimer.Stop();
                HighlightSyntax();
            };
        }

        private void HighlightSyntax()
        {
            if (isHighlighting || editor == null) return;

            isHighlighting = true;

            try
            {
                // Save state
                int selectionStart = editor.SelectionStart;
                int selectionLength = editor.SelectionLength;

                // RichTextBox doesn't expose BeginUpdate/EndUpdate. Use SuspendLayout/ResumeLayout
                // to reduce layout work during syntax highlighting.
                editor.SuspendLayout();

                // 1. Default color
                editor.SelectAll();
                editor.SelectionColor = identifierColor;

                // 2. Comments
                MatchCollection commentMatches = Regex.Matches(editor.Text, @"//[^\n]*");
                foreach (Match m in commentMatches)
                {
                    editor.Select(m.Index, m.Length);
                    editor.SelectionColor = commentColor;
                }

                // 3. Strings
                MatchCollection stringMatches = Regex.Matches(editor.Text, "\"[^\"\n]*\"");
                foreach (Match m in stringMatches)
                {
                    editor.Select(m.Index, m.Length);
                    editor.SelectionColor = stringColor;
                }

                // 4. Keywords
                string[] keywords = {
                    // البرنامج والكتل
                    "برنامج", "نهاية", "إجراء", "إرجاع",
                    // الشروط والحلقات
                    "إذا", "فإن", "وإلا", "بينما", "لـ", "إلى",
                    // الإدخال والإخراج
                    "اطبع", "اقرأ",
                    // المتغيرات والثوابت
                    "متغير", "ثابت",
                    // الأنواع
                    "عدد صحيح", "عدد عشري", "نص", "منطقي", "مصفوفة", "سجل", "حقل",
                    // المعاملات
                    "بالقيمة", "بالمرجع",
                    // العمليات المنطقية
                    "و", "أو", "ليس",
                    // القيم
                    "صح", "خطأ"
                };
                string keywordPattern = "\\b(" + string.Join("|", keywords) + ")\\b";
                MatchCollection keywordMatches = Regex.Matches(editor.Text, keywordPattern);
                foreach (Match m in keywordMatches)
                {
                    editor.Select(m.Index, m.Length);
                    editor.SelectionColor = keywordColor;
                }

                // 5. Numbers
                MatchCollection numberMatches = Regex.Matches(editor.Text, @"\b\d+(\.\d+)?\b");
                foreach (Match m in numberMatches)
                {
                    editor.Select(m.Index, m.Length);
                    editor.SelectionColor = numberColor;
                }

                // Restore state
                editor.Select(selectionStart, selectionLength);
                editor.SelectionColor = identifierColor; // Restore color at cursor
            }
            finally
            {
                editor.ResumeLayout();
                isHighlighting = false;
            }
        }

        private void UpdateTitle()
        {
            string filename = string.IsNullOrEmpty(currentFile) ? "غير محفوظ" : Path.GetFileName(currentFile);
            string modified = isModified ? " *" : "";
            this.Text = $"بيئة التطوير العربية - {filename}{modified}";
        }

        private void UpdateLineInfo()
        {
            if (editor != null && lineLabel != null)
            {
                int index = editor.SelectionStart;
                int line = editor.GetLineFromCharIndex(index) + 1;
                int firstChar = editor.GetFirstCharIndexFromLine(line - 1);
                int column = index - firstChar + 1;
                lineLabel.Text = $"السطر: {line}, العمود: {column}";
            }
        }

        private void Editor_SelectionChanged(object? sender, EventArgs e)
        {
            UpdateLineInfo();
        }

        private bool ConfirmSaveChanges()
        {
            if (!isModified) return true;

            var result = MessageBox.Show(
                "هل تريد حفظ التغييرات في الملف الحالي؟",
                "تأكيد حفظ",
                MessageBoxButtons.YesNoCancel,
                MessageBoxIcon.Question,
                MessageBoxDefaultButton.Button1,
                MessageBoxOptions.RightAlign | MessageBoxOptions.RtlReading
            );

            if (result == DialogResult.Yes)
            {
                return SaveFile();
            }
            else if (result == DialogResult.Cancel)
            {
                return false;
            }
            return true;
        }

        private void NewButton_Click(object? sender, EventArgs e)
        {
            if (!ConfirmSaveChanges()) return;

            currentFile = "";
            editor!.Text = GetDefaultCode();
            isModified = false;
            UpdateTitle();
            statusLabel!.Text = "تم إنشاء ملف جديد";
            UpdateLineInfo();
        }

        private void OpenButton_Click(object? sender, EventArgs e)
        {
            if (!ConfirmSaveChanges()) return;

            using (var dialog = new OpenFileDialog())
            {
                dialog.Filter = "ملفات العربية (*.arabic)|*.arabic|جميع الملفات (*.*)|*.*";
                dialog.Title = "فتح ملف برنامج عربي";
                dialog.CheckFileExists = true;

                if (dialog.ShowDialog() == DialogResult.OK)
                {
                    OpenFile(dialog.FileName);
                }
            }
        }

        private void OpenFile(string filename)
        {
            try
            {
                currentFile = filename;
                
                // محاولة قراءة الملف بترميزات مختلفة
                string content = TryReadFileWithEncoding(filename);
                editor!.Text = content;
                
                isModified = false;
                UpdateTitle();
                statusLabel!.Text = $"تم فتح الملف: {Path.GetFileName(currentFile)}";
                UpdateLineInfo();
            }
            catch (Exception ex)
            {
                MessageBox.Show($"خطأ في فتح الملف: {ex.Message}", "خطأ",
                              MessageBoxButtons.OK, MessageBoxIcon.Error);
                statusLabel!.Text = "فشل فتح الملف";
            }
        }

        private string TryReadFileWithEncoding(string filename)
        {
            // محاولة UTF-8 أولاً
            try
            {
                return File.ReadAllText(filename, Encoding.UTF8);
            }
            catch
            {
                // محاولة Windows-1256
                try
                {
                    return File.ReadAllText(filename, Encoding.GetEncoding(1256));
                }
                catch
                {
                    // استخدام الترميز الافتراضي
                    return File.ReadAllText(filename, Encoding.Default);
                }
            }
        }

        private bool SaveFile()
        {
            if (string.IsNullOrEmpty(currentFile))
            {
                return SaveAsFile();
            }

            try
            {
                // حفظ باستخدام UTF-8
                File.WriteAllText(currentFile, editor!.Text, Encoding.UTF8);
                isModified = false;
                UpdateTitle();
                statusLabel!.Text = $"تم حفظ الملف: {Path.GetFileName(currentFile)}";
                return true;
            }
            catch (Exception ex)
            {
                MessageBox.Show($"خطأ في حفظ الملف: {ex.Message}", "خطأ",
                              MessageBoxButtons.OK, MessageBoxIcon.Error);
                statusLabel!.Text = "فشل حفظ الملف";
                return false;
            }
        }

        private bool SaveAsFile()
        {
            using (var dialog = new SaveFileDialog())
            {
                dialog.Filter = "ملفات العربية (*.arabic)|*.arabic|جميع الملفات (*.*)|*.*";
                dialog.Title = "حفظ ملف برنامج عربي";
                dialog.DefaultExt = ".arabic";
                dialog.FileName = string.IsNullOrEmpty(currentFile) ? "غير_مسمى.arabic" : Path.GetFileName(currentFile);

                if (dialog.ShowDialog() == DialogResult.OK)
                {
                    currentFile = dialog.FileName;
                    return SaveFile();
                }
                return false;
            }
        }

        private void SaveButton_Click(object? sender, EventArgs e)
        {
            SaveFile();
        }

        private void SaveAsButton_Click(object? sender, EventArgs e)
        {
            SaveAsFile();
        }

        private void RunButton_Click(object? sender, EventArgs e)
        {
            CompileAndRun(true);
        }

        private void CompileButton_Click(object? sender, EventArgs e)
        {
            CompileAndRun(false);
        }

        private void CompileAndRun(bool runAfterCompile)
        {
            if (!File.Exists(compilerPath))
            {
                MessageBox.Show(
                    $"المترجم '{compilerPath}' غير موجود في المجلد الحالي!\n\n" +
                    "تأكد من:\n" +
                    "1. بناء المترجم (C++) باستخدام CMake\n" +
                    "2. نسخ ArabicCompiler.exe إلى مجلد التطبيق\n" +
                    "3. أن المترجم في نفس مجلد ArabicIDE.exe",
                    "خطأ - المترجم غير موجود",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Error
                );
                statusLabel!.Text = "المترجم غير موجود";
                return;
            }

            // التأكد من حفظ التغييرات قبل الترجمة
            if (isModified || string.IsNullOrEmpty(currentFile))
            {
                if (!SaveFile())
                {
                    statusLabel!.Text = "تم إلغاء الترجمة";
                    return;
                }
            }

            CompileProgram(editor!.Text, runAfterCompile);
        }

        private void LoadSettings()
        {
            try
            {
                if (File.Exists(settingsFile))
                {
                    string json = File.ReadAllText(settingsFile, Encoding.UTF8);
                    var doc = JsonDocument.Parse(json);
                    if (doc.RootElement.TryGetProperty("debugLexer", out var dj))
                    {
                        bool dbg = dj.GetBoolean();
                        if (debugLexerButton != null)
                            debugLexerButton.Checked = dbg;
                    }
                }
            }
            catch
            {
                // ignore errors
            }
        }

        private void SaveSettings()
        {
            try
            {
                var obj = new { debugLexer = debugLexerButton != null && debugLexerButton.Checked };
                var json = JsonSerializer.Serialize(obj, new JsonSerializerOptions { WriteIndented = true });
                File.WriteAllText(settingsFile, json, Encoding.UTF8);
            }
            catch
            {
                // ignore
            }
        }

        private void CompileProgram(string code, bool runAfterCompile)
        {
            statusLabel!.Text = "جاري الترجمة...";
            outputBox!.Clear();
            Application.DoEvents();

            string tempFile = Path.GetTempFileName();
            string baseName = Path.Combine(Path.GetTempPath(), Path.GetRandomFileName());

            try
            {
                // حفظ الملف المؤقت باستخدام UTF-8
                File.WriteAllText(tempFile, code, Encoding.UTF8);

                string outputOption = GetOutputOption();
                string selectedOutputType = GetSelectedOutputType();

                // تشغيل المترجم
                var result = RunCompiler(tempFile, outputOption);
                DisplayCompilationResult(result, baseName, selectedOutputType);

                if (result.ExitCode == 0)
                {
                    statusLabel.Text = "✅ الترجمة تمت بنجاح!";
                    if (runAfterCompile && selectedOutputType == "c")
                    {
                        TryRunCCode(baseName + ".c", baseName + ".exe");
                    }
                }
                else
                {
                    statusLabel.Text = "❌ فشل الترجمة!";
                }
            }
            catch (Exception ex)
            {
                outputBox.AppendText($"\n\nخطأ استثنائي أثناء الترجمة:\n{ex.Message}");
                statusLabel.Text = "❌ خطأ فادح في الترجمة!";
            }
            finally
            {
                CleanupTempFiles(tempFile, baseName);
            }
        }

        private string GetOutputOption()
        {
            return outputTypeComboBox!.SelectedIndex switch
            {
                0 => "--asm",
                1 => "--c", 
                2 => "--ir",
                3 => "--all",
                _ => "--asm"
            };
        }

        private string GetSelectedOutputType()
        {
            return outputTypeComboBox!.SelectedIndex switch
            {
                0 => "asm",
                1 => "c",
                2 => "ir",
                _ => "asm"
            };
        }

        private (int ExitCode, string Output, string Error) RunCompiler(string inputFile, string options)
        {
            using (var process = new Process())
            {
                string args = $"\"{inputFile}\" {options}";
                if (debugLexerButton != null && debugLexerButton.Checked)
                {
                    args += " --debug-lexer";
                }
                // show which compiler binary is used
                if (outputBox != null)
                {
                    outputBox.AppendText($"\n↪ استخدام المترجم: {compilerPath} {args}\n\n");
                }
                process.StartInfo = new ProcessStartInfo
                {
                    FileName = compilerPath,
                    Arguments = args,
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    CreateNoWindow = true,
                    WorkingDirectory = Path.GetDirectoryName(Application.ExecutablePath) ?? Directory.GetCurrentDirectory(),
                    StandardOutputEncoding = Encoding.UTF8,
                    StandardErrorEncoding = Encoding.UTF8
                };

                process.Start();
                string output = process.StandardOutput.ReadToEnd();
                string error = process.StandardError.ReadToEnd();
                process.WaitForExit(15000); // 15 ثانية

                return (process.ExitCode, output, error);
            }
        }

        private void DisplayCompilationResult((int ExitCode, string Output, string Error) result, string baseName, string outputType)
        {
            var sb = new StringBuilder();
            sb.AppendLine("=== نتائج الترجمة ===");
            sb.AppendLine(result.Output.Trim());

            if (!string.IsNullOrEmpty(result.Error.Trim()))
            {
                sb.AppendLine("\n=== الأخطاء ===");
                sb.AppendLine(result.Error.Trim());
            }

            // عرض الملفات المولدة
            DisplayGeneratedFiles(sb, baseName, outputType);

            outputBox!.Text = sb.ToString();

            // تلوين الأخطاء
            if (!string.IsNullOrEmpty(result.Error.Trim()))
            {
                HighlightErrors();
            }
        }

        private void DisplayGeneratedFiles(StringBuilder sb, string baseName, string outputType)
        {
            string[] extensions = outputType switch
            {
                "asm" => new[] { ".asm" },
                "c" => new[] { ".c" },
                "ir" => new[] { "_intermediate.txt" },
                _ => new[] { ".asm", ".c", "_intermediate.txt" }
            };

            foreach (string ext in extensions)
            {
                string filePath = baseName + ext;
                if (File.Exists(filePath))
                {
                    try
                    {
                        string content = File.ReadAllText(filePath, Encoding.UTF8);
                        sb.AppendLine($"\n=== الكود المولد ({Path.GetFileName(filePath)}) ===");
                        sb.AppendLine(content);
                    }
                    catch (Exception ex)
                    {
                        sb.AppendLine($"\n=== خطأ في قراءة الملف المولد: {ex.Message} ===");
                    }
                }
            }
        }

        private void HighlightErrors()
        {
            int errorIndex = outputBox!.Text.IndexOf("=== الأخطاء ===");
            if (errorIndex >= 0)
            {
                outputBox.Select(errorIndex, outputBox.Text.Length - errorIndex);
                outputBox.SelectionColor = Color.Red;
                outputBox.Select(0, 0); // إلغاء التحديد
            }
        }

        private void TryRunCCode(string cFilePath, string exeFilePath)
        {
            if (!File.Exists(cFilePath))
            {
                outputBox!.AppendText($"\n\n⚠️ ملف C غير موجود للتنفيذ: {Path.GetFileName(cFilePath)}");
                return;
            }

            statusLabel!.Text = "جاري بناء كود C...";
            Application.DoEvents();

            if (CompileCCode(cFilePath, exeFilePath))
            {
                RunExecutable(exeFilePath);
            }
        }

        private bool CompileCCode(string cFile, string exeFile)
        {
            try
            {
                using (var process = new Process())
                {
                    process.StartInfo = new ProcessStartInfo
                    {
                        FileName = "gcc",
                        Arguments = $"\"{cFile}\" -o \"{exeFile}\"",
                        UseShellExecute = false,
                        RedirectStandardOutput = true,
                        RedirectStandardError = true,
                        CreateNoWindow = true,
                        StandardOutputEncoding = Encoding.UTF8,
                        StandardErrorEncoding = Encoding.UTF8
                    };

                    process.Start();
                    string output = process.StandardOutput.ReadToEnd();
                    string error = process.StandardError.ReadToEnd();
                    process.WaitForExit(20000); // 20 seconds

                    if (!string.IsNullOrWhiteSpace(output))
                        outputBox!.AppendText($"\n\n=== مخرجات بناء GCC ===\n{output.Trim()}");
                    if (!string.IsNullOrWhiteSpace(error))
                        outputBox!.AppendText($"\n\n=== أخطاء بناء GCC ===\n{error.Trim()}");

                    return process.ExitCode == 0 && File.Exists(exeFile);
                }
            }
            catch (Win32Exception ex) when (ex.NativeErrorCode == 2)
            {
                MessageBox.Show(
                    "لم يتم العثور على مترجم 'gcc'.\n\n" +
                    "لتنفيذ البرامج، تحتاج إلى:\n" +
                    "1. تثبيت MinGW-w64 أو GCC\n" +
                    "2. إضافته إلى متغير البيئة PATH\n" +
                    "3. إعادة تشغيل البرنامج",
                    "خطأ - GCC غير موجود",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Error
                );
                outputBox!.AppendText("\n\n❌ خطأ: لم يتم العثور على 'gcc'");
                return false;
            }
            catch (Exception ex)
            {
                outputBox!.AppendText($"\n\n❌ خطأ في بناء كود C: {ex.Message}");
                return false;
            }
        }

        private void RunExecutable(string exeFile)
        {
            if (!File.Exists(exeFile)) return;

            statusLabel!.Text = "جاري تنفيذ البرنامج في نافذة جديدة...";
            Application.DoEvents();

            try
            {
                using (var process = new Process())
                {
                    process.StartInfo = new ProcessStartInfo
                    {
                        FileName = exeFile,
                        UseShellExecute = false,
                        RedirectStandardOutput = true,
                        RedirectStandardError = true,
                        CreateNoWindow = true,
                        StandardOutputEncoding = Encoding.UTF8,
                        StandardErrorEncoding = Encoding.UTF8
                    };

                    process.Start();
                    string output = process.StandardOutput.ReadToEnd();
                    string error = process.StandardError.ReadToEnd();
                    process.WaitForExit(10000); // 10 seconds

                    if (!string.IsNullOrWhiteSpace(output))
                        outputBox!.AppendText($"\n\n=== تنفيذ البرنامج ===\n{output}");
                    if (!string.IsNullOrWhiteSpace(error))
                        outputBox!.AppendText($"\n\n=== أخطاء تنفيذ البرنامج ===\n{error}");

                    statusLabel.Text = process.ExitCode == 0 ?
                        "✅ تم تنفيذ البرنامج بنجاح!" :
                        "⚠️ انتهى التنفيذ برمز خطأ";
                }
            }
            catch (Exception ex)
            {
                outputBox!.AppendText($"\n\n❌ خطأ في تشغيل البرنامج: {ex.Message}");
                statusLabel!.Text = "❌ فشل تشغيل البرنامج!";
            }
        }

        private void CleanupTempFiles(string tempFile, string baseName)
        {
            try
            {
                if (File.Exists(tempFile))
                    File.Delete(tempFile);

                string[] extensions = { ".c", ".exe", ".asm", "_intermediate.txt" };
                foreach (string ext in extensions)
                {
                    string file = baseName + ext;
                    if (File.Exists(file))
                        File.Delete(file);
                }
            }
            catch
            {
                // تجاهل أخطاء التنظيف
            }
        }

        private void Editor_TextChanged(object? sender, EventArgs e)
        {
            if (!isModified)
            {
                isModified = true;
                UpdateTitle();
            }

            if (syntaxHighlightingTimer != null)
            {
                syntaxHighlightingTimer.Stop();
                syntaxHighlightingTimer.Start();
            }
        }

        private void Editor_KeyDown(object? sender, KeyEventArgs e)
        {
            UpdateLineInfo();

            if (e.Control && e.KeyCode == Keys.S)
            {
                if (e.Shift)
                    SaveAsButton_Click(sender, e);
                else
                    SaveButton_Click(sender, e);
                e.Handled = true;
            }
            else if (e.Control && e.KeyCode == Keys.O)
            {
                OpenButton_Click(sender, e);
                e.Handled = true;
            }
            else if (e.Control && e.KeyCode == Keys.N)
            {
                NewButton_Click(sender, e);
                e.Handled = true;
            }
            else if (e.Control && e.KeyCode == Keys.B)
            {
                CompileButton_Click(sender, e);
                e.Handled = true;
            }
            else if (e.KeyCode == Keys.F5)
            {
                RunButton_Click(sender, e);
                e.Handled = true;
            }
        }

        private void MainForm_KeyDown(object? sender, KeyEventArgs e)
        {
            if (e.Control && e.KeyCode == Keys.Q)
            {
                this.Close();
            }
        }

        private void LoadExamples_Click(object? sender, EventArgs e)
        {
            try
            {
                string exeDir = Path.GetDirectoryName(Application.ExecutablePath) ?? Directory.GetCurrentDirectory();
                string repoRoot = Path.GetFullPath(Path.Combine(exeDir, ".."));
                string examplesDir = Path.Combine(repoRoot, "Examples");

                if (!Directory.Exists(examplesDir))
                {
                    MessageBox.Show($"مجلد الأمثلة غير موجود: {examplesDir}", "خطأ", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

                var files = Directory.GetFiles(examplesDir, "*.arabic").OrderBy(f => f).ToList();
                if (files.Count == 0)
                {
                    MessageBox.Show("لا توجد أمثلة متاحة في مجلد Examples", "معلومة", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return;
                }

                // Create a simple selection dialog
                var form = new Form();
                form.Text = "اختر مثالاً";
                form.Size = new Size(400, 300);
                form.StartPosition = FormStartPosition.CenterParent;
                form.ShowInTaskbar = false;

                var listBox = new ListBox();
                listBox.Dock = DockStyle.Fill;
                listBox.Items.AddRange(files.Select(f => Path.GetFileName(f)).ToArray());
                listBox.DoubleClick += (s, e) => form.DialogResult = DialogResult.OK;

                var buttonPanel = new Panel();
                buttonPanel.Dock = DockStyle.Bottom;
                buttonPanel.Height = 40;
                buttonPanel.Padding = new Padding(5);

                var okButton = new Button();
                okButton.Text = "فتح";
                okButton.Dock = DockStyle.Right;
                okButton.Width = 80;
                okButton.Click += (s, e) => form.DialogResult = DialogResult.OK;

                var cancelButton = new Button();
                cancelButton.Text = "إلغاء";
                cancelButton.Dock = DockStyle.Right;
                cancelButton.Width = 80;
                cancelButton.Click += (s, e) => form.DialogResult = DialogResult.Cancel;

                buttonPanel.Controls.Add(okButton);
                buttonPanel.Controls.Add(cancelButton);

                form.Controls.Add(listBox);
                form.Controls.Add(buttonPanel);

                if (form.ShowDialog(this) == DialogResult.OK && listBox.SelectedIndex >= 0)
                {
                    string selectedFile = files[listBox.SelectedIndex];
                    OpenFile(selectedFile);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"خطأ في تحميل الأمثلة: {ex.Message}", "خطأ", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void RunAllExamples_Click(object? sender, EventArgs e)
        {
            // Fire-and-forget the async runner but keep UI responsive
            _ = RunAllExamplesAsync();
        }

        private async Task RunAllExamplesAsync()
        {
            if (runAllButton != null)
                runAllButton.Enabled = false;

            try
            {
                statusLabel!.Text = "جاري تشغيل جميع الأمثلة...";
                outputBox!.Clear();

                string exeDir = Path.GetDirectoryName(Application.ExecutablePath) ?? Directory.GetCurrentDirectory();
                string repoRoot = Path.GetFullPath(Path.Combine(exeDir, ".."));
                // run end-to-end script (compile .arabic -> build C -> run)
                string script = Path.Combine(repoRoot, "scripts", "run_end_to_end.ps1");
                string logsDir = Path.Combine(repoRoot, "build_tests_logs");

                if (!File.Exists(script))
                {
                    // If the PowerShell script is missing, fall back to an internal managed runner
                    outputBox.AppendText($"⚠️ لم يتم العثور على السكربت: {script}. سيتم استخدام المُنفّذ الداخلي بدلاً منه.\n");
                    await RunEndToEndInternalAsync(repoRoot, logsDir);
                    statusLabel.Text = "تم تشغيل الأمثلة (المنفذ الداخلي) - انظر الإخراج";
                    return;
                }

                var psi = new ProcessStartInfo
                {
                    FileName = "powershell.exe",
                    Arguments = $"-NoProfile -ExecutionPolicy Bypass -File \"{script}\"",
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    CreateNoWindow = true
                };

                using (var proc = Process.Start(psi))
                {
                    string sout = await proc.StandardOutput.ReadToEndAsync();
                    string serr = await proc.StandardError.ReadToEndAsync();
                    proc.WaitForExit();

                    if (!string.IsNullOrWhiteSpace(sout))
                        outputBox.AppendText("=== سكربت: المخرجات ===\n" + sout + "\n");
                    if (!string.IsNullOrWhiteSpace(serr))
                        outputBox.AppendText("=== سكربت: الأخطاء ===\n" + serr + "\n");

                    string summaryFile = Path.Combine(logsDir, "summary.txt");
                    if (File.Exists(summaryFile))
                    {
                        outputBox.AppendText("\n=== Summary ===\n");

                        // Read lines and append a clickable file:// link per summary entry
                        var lines = File.ReadAllLines(summaryFile, Encoding.UTF8);
                        foreach (var l in lines)
                        {
                            if (string.IsNullOrWhiteSpace(l)) continue;
                            // expect format: name: status
                            var parts = l.Split(':', 2);
                            string name = parts[0].Trim();
                            string status = parts.Length > 1 ? parts[1].Trim() : "";
                            string logFile = Path.Combine(logsDir, name + ".log");

                            if (File.Exists(logFile))
                            {
                                // Use file:// URI so RichTextBox auto-detects it as a link
                                var uri = new Uri(logFile).AbsoluteUri;
                                outputBox.AppendText($"{name}: {status} ");
                                outputBox.AppendText(uri + "\n");
                            }
                            else
                            {
                                outputBox.AppendText($"{name}: {status} (log missing)\n");
                            }
                        }
                    }
                    else
                    {
                        outputBox.AppendText($"\nلم يتم العثور على ملف الملخص: {summaryFile}\n");
                    }

                    statusLabel.Text = "تم تشغيل الأمثلة - انظر الإخراج";
                }
            }
            catch (Exception ex)
            {
                outputBox!.AppendText($"\n\n❌ خطأ أثناء تشغيل الأمثلة: {ex.Message}");
                statusLabel!.Text = "خطأ في تشغيل الأمثلة";
            }
            finally
            {
                if (runAllButton != null)
                    runAllButton.Enabled = true;
            }
        }

        private void OutputBox_LinkClicked(object sender, LinkClickedEventArgs e)
        {
            try
            {
                string link = e.LinkText;
                // Support file:// URIs
                if (link.StartsWith("file://", StringComparison.OrdinalIgnoreCase))
                {
                    var uri = new Uri(link);
                    string path = uri.LocalPath;
                    if (File.Exists(path))
                    {
                        ShowLogViewer(Path.GetFileName(path), File.ReadAllText(path, Encoding.UTF8));
                        return;
                    }
                }

                // Fallback: if it's a local path
                if (File.Exists(link))
                {
                    ShowLogViewer(Path.GetFileName(link), File.ReadAllText(link, Encoding.UTF8));
                    return;
                }

                MessageBox.Show($"لا يمكن فتح الرابط: {link}", "خطأ", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (Exception ex)
            {
                MessageBox.Show($"خطأ عند فتح السجل: {ex.Message}", "خطأ", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void ShowLogViewer(string title, string content)
        {
            var dlg = new Form();
            dlg.Text = "سجل: " + title;
            dlg.Size = new Size(900, 600);
            dlg.StartPosition = FormStartPosition.CenterParent;

            var rtb = new RichTextBox();
            rtb.Dock = DockStyle.Fill;
            rtb.Font = new Font("Consolas", 10);
            rtb.ReadOnly = true;
            rtb.BackColor = Color.FromArgb(20, 20, 20);
            rtb.ForeColor = Color.LightGreen;
            rtb.WordWrap = false;
            rtb.Text = content;

            dlg.Controls.Add(rtb);
            dlg.ShowDialog(this);
        }

        private async Task RunEndToEndInternalAsync(string repoRoot, string logsDir)
        {
            try
            {
                Directory.CreateDirectory(logsDir);

                // Locate compiler
                string[] candidates = new[] {
                    Path.Combine(repoRoot, "Compiler", "ArabicCompiler.exe"),
                    Path.Combine(repoRoot, "Compiler", "build", "Release", "ArabicCompiler.exe"),
                    Path.Combine(repoRoot, "Compiler", "build", "ArabicCompiler.exe"),
                    Path.Combine(repoRoot, "ArabicCompiler.exe")
                };
                string compiler = candidates.FirstOrDefault(p => File.Exists(p));
                if (compiler == null)
                {
                    outputBox!.AppendText("❌ لم يتم العثور على ArabicCompiler.exe في المسارات المتوقعة. تأكد من بناء المترجم أولاً.\n");
                    return;
                }

                var summary = new List<string>();
                string examplesDir = Path.Combine(repoRoot, "Examples");
                var files = Directory.GetFiles(examplesDir, "*.arabic");
                foreach (var src in files)
                {
                    string baseName = Path.GetFileNameWithoutExtension(src);
                    string logFile = Path.Combine(logsDir, baseName + ".log");
                    var lines = new List<string>();
                    lines.Add($"Source: {src}");
                    lines.Add($"Started: {DateTime.UtcNow:o}");

                    try
                    {
                        outputBox.AppendText($"↳ تشغيل المترجم على: {baseName}...\n");
                        // Run ArabicCompiler
                        var psi = new ProcessStartInfo
                        {
                            FileName = compiler,
                            Arguments = $"\"{src}\" --c --asm --ir",
                            UseShellExecute = false,
                            RedirectStandardOutput = true,
                            RedirectStandardError = true,
                            CreateNoWindow = true,
                            StandardOutputEncoding = Encoding.UTF8,
                            StandardErrorEncoding = Encoding.UTF8
                        };
                        using (var p = Process.Start(psi))
                        {
                            string sout = await p.StandardOutput.ReadToEndAsync();
                            string serr = await p.StandardError.ReadToEndAsync();
                            p.WaitForExit();
                            lines.Add($"ArabicCompiler exit code: {p.ExitCode}");
                            lines.Add("ArabicCompiler stdout:");
                            lines.AddRange(sout.Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries));
                            lines.Add("ArabicCompiler stderr:");
                            lines.AddRange(serr.Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries));

                            if (p.ExitCode != 0)
                            {
                                lines.Add("Result: compile-source-failed");
                                summary.Add($"{baseName}: compile-source-failed");
                                File.WriteAllLines(logFile, lines, Encoding.UTF8);
                                continue;
                            }
                        }

                        string baseOut = Path.Combine(Path.GetDirectoryName(src) ?? examplesDir, baseName + "_output");
                        string cfile = baseOut + ".c";
                        if (!File.Exists(cfile))
                        {
                            lines.Add("Result: c-not-generated");
                            summary.Add($"{baseName}: c-not-generated");
                            File.WriteAllLines(logFile, lines, Encoding.UTF8);
                            continue;
                        }

                        // Compile with gcc if available
                        string exeTarget = Path.Combine(logsDir, baseName + ".exe");
                        var gccPsi = new ProcessStartInfo
                        {
                            FileName = "gcc",
                            Arguments = $"\"{cfile}\" -o \"{exeTarget}\"",
                            UseShellExecute = false,
                            RedirectStandardOutput = true,
                            RedirectStandardError = true,
                            CreateNoWindow = true
                        };
                        try
                        {
                            using (var gp = Process.Start(gccPsi))
                            {
                                string gsout = await gp.StandardOutput.ReadToEndAsync();
                                string gserr = await gp.StandardError.ReadToEndAsync();
                                gp.WaitForExit();
                                if (gp.ExitCode != 0 || !File.Exists(exeTarget))
                                {
                                    lines.Add($"gcc failed with exit {gp.ExitCode}");
                                    lines.AddRange(gserr.Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries));
                                    summary.Add($"{baseName}: gcc-failed");
                                    File.WriteAllLines(logFile, lines, Encoding.UTF8);
                                    continue;
                                }
                            }
                        }
                        catch (Win32Exception)
                        {
                            lines.Add("gcc not found in PATH: skipping native build/run");
                            summary.Add($"{baseName}: skipped-no-gcc");
                            File.WriteAllLines(logFile, lines, Encoding.UTF8);
                            continue;
                        }

                        // Run the produced exe
                        try
                        {
                            using (var rp = Process.Start(new ProcessStartInfo { FileName = exeTarget, RedirectStandardOutput = true, RedirectStandardError = true, UseShellExecute = false, CreateNoWindow = true }))
                            {
                                string rout = await rp.StandardOutput.ReadToEndAsync();
                                string rerr = await rp.StandardError.ReadToEndAsync();
                                rp.WaitForExit();
                                lines.Add($"Program exit code: {rp.ExitCode}");
                                lines.Add("Program stdout:");
                                lines.AddRange(rout.Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries));
                                lines.Add("Program stderr:");
                                lines.AddRange(rerr.Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries));
                                if (rp.ExitCode == 0)
                                {
                                    lines.Add("Result: ok");
                                    summary.Add($"{baseName}: ok");
                                }
                                else
                                {
                                    lines.Add("Result: runtime-failed");
                                    summary.Add($"{baseName}: runtime-failed");
                                }
                            }
                        }
                        catch (Exception ex)
                        {
                            lines.Add($"Exception running exe: {ex.Message}");
                            summary.Add($"{baseName}: runtime-exception");
                        }

                        File.WriteAllLines(logFile, lines, Encoding.UTF8);
                    }
                    catch (Exception ex)
                    {
                        lines.Add($"Exception: {ex}");
                        File.WriteAllLines(logFile, lines, Encoding.UTF8);
                        summary.Add($"{baseName}: exception");
                    }
                }

                // Write summary
                string summaryFile = Path.Combine(logsDir, "summary.txt");
                File.WriteAllLines(summaryFile, summary, Encoding.UTF8);

                // Show summary in outputBox
                outputBox!.AppendText("\n=== Summary ===\n");
                foreach (var s in summary)
                {
                    outputBox.AppendText(s + "\n");
                }
            }
            catch (Exception ex)
            {
                outputBox!.AppendText($"\n❌ خطأ في المنفّذ الداخلي: {ex.Message}\n");
            }
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            if (!ConfirmSaveChanges())
            {
                e.Cancel = true;
            }
            base.OnFormClosing(e);
        }
    }
}


```


---

## 📄 `./IDE/Program.cs`

```csharp
using System;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;
using System.Text;

namespace ArabicIDE
{
    internal static class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            try
            {
                // CLI helper: run end-to-end tests headless when requested
                if (args != null && args.Length > 0 && args[0] == "--run-tests")
                {
                    string exeDir = AppDomain.CurrentDomain.BaseDirectory;
                    // find repository root by walking up until we find a 'scripts' folder
                    string dir = exeDir;
                    string repoRoot = null;
                    for (int i = 0; i < 6; i++)
                    {
                        string candidate = Path.Combine(dir, "scripts");
                        if (Directory.Exists(candidate)) { repoRoot = Directory.GetParent(candidate).FullName; break; }
                        var parent = Directory.GetParent(dir);
                        if (parent == null) break;
                        dir = parent.FullName;
                    }
                    if (repoRoot == null) repoRoot = Path.GetFullPath(Path.Combine(exeDir, "..", "..", ".."));

                    string script = Path.Combine(repoRoot, "scripts", "run_end_to_end.ps1");
                    if (!File.Exists(script))
                    {
                        Console.WriteLine($"run-tests: script not found: {script}");
                        return;
                    }

                    var psi = new ProcessStartInfo
                    {
                        FileName = "powershell.exe",
                        Arguments = $"-NoProfile -ExecutionPolicy Bypass -File \"{script}\"",
                        UseShellExecute = false,
                        RedirectStandardOutput = true,
                        RedirectStandardError = true,
                        CreateNoWindow = true
                    };
                    using (var p = Process.Start(psi))
                    {
                        string outp = p.StandardOutput.ReadToEnd();
                        string errp = p.StandardError.ReadToEnd();
                        p.WaitForExit();
                        Console.WriteLine(outp);
                        if (!string.IsNullOrWhiteSpace(errp)) Console.Error.WriteLine(errp);
                    }
                    return;
                }
              
              
                
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                
                // التحقق من المتطلبات الأساسية
                if (!CheckBasicRequirements())
                {
                    return; // إغلاق التطبيق إذا قرر المستخدم ذلك
                }
                
                // تشغيل التطبيق الرئيسي
                Application.Run(new MainForm());
            }
            catch (Exception ex)
            {
                MessageBox.Show($"خطأ فادح أثناء بدء التشغيل: {ex.Message}\n\n" +
                                $"نوع الخطأ: {ex.GetType().Name}",
                                "خطأ - بيئة التطوير العربية",
                                MessageBoxButtons.OK, 
                                MessageBoxIcon.Error);
            }
        }

        // تم تعديلها لترجع قيمة bool
        static bool CheckBasicRequirements()
        {
            //string compilerName = "ArabicCompiler.exe";
            string compilerName = "my_compiler.exe";
            string currentDirectory = Directory.GetCurrentDirectory();
            string compilerPath = Path.Combine(currentDirectory, compilerName);

            if (!File.Exists(compilerPath))
            {
                var result = MessageBox.Show(
                    $"المترجم '{compilerName}' غير موجود في المجلد الحالي.\n\n" +
                    $"المسار الذي تم البحث فيه:\n{currentDirectory}\n\n" +
                    "بدون المترجم، يمكنك استخدام المحرر للقراءة والكتابة فقط، ولكن لن تعمل الترجمة أو التنفيذ.\n\n" +
                    "هل تريد الاستمرار في وضع المحرر فقط؟",
                    "تحذير - المترجم غير موجود",
                    MessageBoxButtons.YesNo,
                    MessageBoxIcon.Warning);

                return (result == DialogResult.Yes); // إرجاع true إذا وافق المستخدم، و false إذا ضغط 'لا'
            }
            
            return true; // المترجم موجود
        }
    }
}
```


---

## 📄 `./IDE/ArabicIDE.csproj`

```xml
<Project Sdk="Microsoft.NET.Sdk">

  <PropertyGroup>
    <OutputType>WinExe</OutputType>
    <TargetFramework>net6.0-windows</TargetFramework>
    <Nullable>enable</Nullable>
    <UseWindowsForms>true</UseWindowsForms>
    <ImplicitUsings>enable</ImplicitUsings>
    <AssemblyTitle>بيئة التطوير العربية</AssemblyTitle>
    <AssemblyDescription>IDE للغة البرمجة العربية</AssemblyDescription>
    <AssemblyVersion>1.0.0.0</AssemblyVersion>
    <FileVersion>1.0.0.0</FileVersion>
    <Copyright>Copyright © 2024</Copyright>
    <PublishSingleFile>false</PublishSingleFile>
    <SelfContained>false</SelfContained>
  </PropertyGroup>

  <ItemGroup>
    <PackageReference Include="System.Text.Encoding.CodePages" Version="7.0.0" />
  </ItemGroup>

  <ItemGroup>
    <Content Include="ArabicCompiler.exe">
      <CopyToOutputDirectory>PreserveNewest</CopyToOutputDirectory>
    </Content>
  </ItemGroup>

</Project>
```


---

## 📄 `./Examples/Ready/غير_مسمى.arabic`

```text
﻿// برنامج مثال باللغة العربية
// مثال شامل يوضح الميزات الأساسية

برنامج مثال_شامل ؛

// تعريف الثوابت
ثابت ط = 3.14 ؛
ثابت الحد_الأقصى = 100 ؛

// تعريف المتغيرات
متغير س : عدد صحيح ؛
متغير ناتج : عدد صحيح ؛
متغير أرقام : مصفوفة[5] من عدد صحيح ؛

// تعيين القيم
س = 10 ؛
ناتج = س * 2 + 5 ؛
أرقام[0] = 10 ؛
أرقام[1] = 20 ؛

// جملة شرطية
إذا ناتج > 15 فإن
    اطبع "الناتج كبير: " ؛
    اطبع ناتج ؛
وإلا
    اطبع "الناتج صغير: " ؛
    اطبع ناتج ؛
نهاية ؛

// حلقة تكرار
لـ (ع = 1 إلى 5)
    اطبع "العدد: " ؛
    اطبع ع ؛
نهاية ؛

اطبع "نهاية البرنامج" ؛
```


---

## 📄 `./Examples/Ready/01_basic_program.arabic`

```text
﻿برنامج برنامج_أساسي ؛

اطبع "مرحبا بك في مترجم اللغة العربية!" ؛
اطبع "هذا برنامج أساسي بسيط" ؛

نهاية.


```


---

## 📄 `./Examples/Ready/02_variables_and_types.arabic`

```text
﻿برنامج اختبار_المتغيرات ؛

متغير عدد : عدد صحيح ؛
متغير نص : نص ؛
متغير عشري : عدد عشري ؛
متغير منطقي : منطقي ؛

عدد = 42 ؛
نص = "مرحبا" ؛
عشري = 3.14 ؛
منطقي = صحيح ؛

اطبع "العدد الصحيح: " ؛
اطبع عدد ؛

اطبع "النص: " ؛
اطبع نص ؛

اطبع "العدد العشري: " ؛
اطبع عشري ؛

اطبع "القيمة المنطقية: " ؛
اطبع منطقي ؛

نهاية.


```


---

## 📄 `./Examples/Ready/04_comparison_operators.arabic`

```text
﻿برنامج مقارنة_الأعداد ؛

متغير أ : عدد صحيح ؛
متغير ب : عدد صحيح ؛
متغير النتيجة : منطقي ؛

أ = 10 ؛
ب = 5 ؛

اطبع "عمليات المقارنة:" ؛

النتيجة = أ > ب ؛
اطبع "أ أكبر من ب: " ؛
اطبع النتيجة ؛

النتيجة = أ < ب ؛
اطبع "أ أصغر من ب: " ؛
اطبع النتيجة ؛

النتيجة = أ == ب ؛
اطبع "أ يساوي ب: " ؛
اطبع النتيجة ؛

النتيجة = أ != ب ؛
اطبع "أ لا يساوي ب: " ؛
اطبع النتيجة ؛

النتيجة = أ >= ب ؛
اطبع "أ أكبر من أو يساوي ب: " ؛
اطبع النتيجة ؛

النتيجة = أ <= ب ؛
اطبع "أ أصغر من أو يساوي ب: " ؛
اطبع النتيجة ؛

نهاية.


```


---

## 📄 `./Examples/Ready/11_simple_procedure.arabic`

```text
﻿برنامج اختبار_الإجراء_البسيط ؛

إجراء السلام () : فراغ ؛
  اطبع "السلام عليكم ورحمة الله وبركاته" ؛
نهاية ؛

اطبع "استدعاء الإجراء:" ؛
السلام () ؛

اطبع "انتهى البرنامج" ؛

نهاية.


```


---

## 📄 `./Examples/Ready/array_test.arabic`

```text
﻿برنامج اختبار_القائمة؛

متغير ارقام : قائمة[10] من صحيح؛
متغير نتيجة : صحيح؛

ارقام[3] = 42؛
نتيجة = ارقام[3]؛

اطبع "القيمة في الفهرس 3 هي: "؛
اطبع نتيجة؛

نهاية؛

```


---

## 📄 `./Examples/Ready/calculator.arabic`

```text
﻿// آلة حاسبة بسيطة
برنامج آلة_حاسبة؛

متغير رقم1 = 15؛
متغير رقم2 = 3؛
متغير ناتج = 0؛

// عملية الجمع
ناتج = رقم1 + رقم2؛
اطبع "الجمع: "؛
اطبع ناتج؛

// عملية الطرح
ناتج = رقم1 - رقم2؛
اطبع "الطرح: "؛
اطبع ناتج؛

// عملية الضرب
ناتج = رقم1 * رقم2؛
اطبع "الضرب: "؛
اطبع ناتج؛

// عملية القسمة
ناتج = رقم1 / رقم2؛
اطبع "القسمة: "؛
اطبع ناتج؛

// عملية الباقي
ناتج = رقم1 % رقم2؛
اطبع "الباقي: "؛
اطبع ناتج؛
```


---

## 📄 `./Examples/Ready/conditions.arabic`

```text
﻿برنامج شروط؛

متغير درجة = 85؛

اذا درجة >= 90 فان
    اطبع "امتياز"؛
وإلا اذا درجة >= 80 فان
    اطبع "جيد جداً"؛
وإلا اذا درجة >= 70 فان
    اطبع "جيد"؛
وإلا اذا درجة >= 60 فان
    اطبع "مقبول"؛
وإلا
    اطبع "راسب"؛
نهاية؛

متغير س = 10؛
متغير ص = 20؛

اذا س > 5 فان
    اطبع "الشرط الأول صحيح"؛
نهاية؛

اذا س == 10 فان
    اطبع "الشرط الثاني صحيح"؛
نهاية؛
```


---

## 📄 `./Examples/Ready/example1.arabic`

```text
﻿// example1.arabic
برنامج أساسي؛

متغير س : صحيح؛
متغير ن : صحيح؛

اطبع("أدخل رقمًا: ")؛
اقرأ(س)؛

ن = س * 2 + 5؛
اطبع("الناتج: ")؛
اطبع(ن)؛
```


---

## 📄 `./Examples/Ready/example2.arabic`

```text
﻿برنامج شرطي؛

متغير العمر = 25؛

اطبع "أدخل عمرك: "؛

اذا العمر >= 18 فان
    اطبع "أنت بالغ"؛
وإلا
    اطبع "أنت قاصر"؛
نهاية؛
```


---

## 📄 `./Examples/Ready/example3.arabic`

```text
﻿// example3.arabic
برنامج حلقة؛

متغير العداد : صحيح؛
متغير المجموع : صحيح؛

المجموع = 0؛
العداد = 1؛

طالما (العداد <= 10) فان
    المجموع = المجموع + العداد؛
    العداد = العداد + 1؛
نهاية طالما؛

اطبع("مجموع الأعداد من 1 إلى 10 هو: ")؛
اطبع(المجموع)؛
```


---

## 📄 `./Tests/test_01_program_structure.arabic`

```text
برنامج اختبار_البرنامج ؛
    اطبع "مرحبا بك" ؛
.


```


---

## 📄 `./Tests/test_02_constants.arabic`

```text
برنامج اختبار_الثوابت ؛
    ثابت
        الحد_الأقصى = 100 ؛
        الاسم = "أحمد" ؛
    متغير
        س : صحيح ؛
    س = الحد_الأقصى ؛
    اطبع س ؛
.


```


---

## 📄 `./Tests/test_03_arrays.arabic`

```text
برنامج اختبار_المصفوفات ؛
    متغير
        أرقام : قائمة[5] من صحيح ؛
        ع : صحيح ؛
    أرقام[0] = 10 ؛
    أرقام[1] = 20 ؛
    أرقام[2] = 30 ؛
    اطبع أرقام[0] ؛
    اطبع أرقام[1] ؛
    اطبع أرقام[2] ؛
.


```


---

## 📄 `./Tests/test_06_if_else.arabic`

```text
برنامج اختبار_الشروط ؛
    متغير
        س : صحيح ؛
    س = 15 ؛
    اذا (س > 10) فان
        اطبع "كبير" ؛
    والا
        اطبع "صغير" ؛
    نهاية ؛
.


```


---

## 📄 `./Tests/test_08_for_loop.arabic`

```text
برنامج اختبار_حلقة_كرر ؛
    متغير
        ع : صحيح ؛
    كرر (ع = 1 الى 5)
        اطبع ع ؛
    نهاية ؛
.


```


---

## 📄 `./scripts/run_all_examples_simple.ps1`

```powershell
$ErrorActionPreference = 'Stop'
$repoRoot = Resolve-Path "$(Split-Path -Parent $MyInvocation.MyCommand.Path)\.." | Select-Object -ExpandProperty Path
$compiler = Join-Path $repoRoot "Compiler\build\Release\ArabicCompiler.exe"
$examples = Join-Path $repoRoot "Examples"
$logs = Join-Path $repoRoot "build_tests_logs"
if (-not (Test-Path $compiler)) { Write-Error "Compiler not found: $compiler"; exit 2 }
New-Item -Path $logs -ItemType Directory -Force | Out-Null

Get-ChildItem -Path $examples -Filter *.arabic -File | ForEach-Object {
    $file = $_.FullName
    $name = $_.BaseName
    $log = Join-Path $logs "$name.log"
    $err = Join-Path $logs "$name.err"
    Write-Host "Running: $name"
    & $compiler $file --all > $log 2> $err
    if (Test-Path (Join-Path $examples "$name`_output.c")) {
        $c = Join-Path $examples "$name`_output.c"
        $exe = Join-Path $logs "$name`_run.exe"
        $gccErr = Join-Path $logs "$name`_gcc.err"
        & gcc $c -o $exe 2> $gccErr
        if ($LASTEXITCODE -eq 0) {
            $runOut = Join-Path $logs "$name`_run.out"
            $runErr = Join-Path $logs "$name`_run.err"
            & $exe > $runOut 2> $runErr
        }
    }
}
Write-Host "Done. Logs in: $logs"

```


---

## 📄 `./scripts/run_end_to_end.ps1`

```powershell
$ErrorActionPreference='Stop'

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Resolve-Path "$scriptDir\.." | Select-Object -ExpandProperty Path
$examplesDir = Join-Path $repoRoot "Examples"
$logsDir = Join-Path $repoRoot "build_tests_logs"
New-Item -Path $logsDir -ItemType Directory -Force | Out-Null

# Try locate ArabicCompiler.exe in common places
$possible = @( 
    Join-Path $repoRoot "Compiler\ArabicCompiler.exe",
    Join-Path $repoRoot "Compiler\build\Release\ArabicCompiler.exe",
    Join-Path $repoRoot "Compiler\build\ArabicCompiler.exe",
    Join-Path $repoRoot "ArabicCompiler.exe"
)
$compiler = $possible | Where-Object { Test-Path $_ } | Select-Object -First 1
if (-not $compiler) {
    Write-Host "ERROR: ArabicCompiler.exe not found. Looked in: `n$($possible -join "`n")" -ForegroundColor Red
    exit 2
}
Write-Host "Using compiler: $compiler"

$summary = @()

Get-ChildItem -Path $examplesDir -Filter '*.arabic' -File | ForEach-Object {
    $src = $_.FullName
    $name = $_.BaseName
    $baseNoExt = [System.IO.Path]::GetFileNameWithoutExtension($src)
    $baseOut = Join-Path $($_.DirectoryName) ($baseNoExt + "_output")

    $logFile = Join-Path $logsDir ($baseNoExt + ".log")
    $gccErr = Join-Path $logsDir ($baseNoExt + ".gcc.err")
    $runOut = Join-Path $logsDir ($baseNoExt + ".run.out")
    $runErr = Join-Path $logsDir ($baseNoExt + ".run.err")

    $lines = @()
    $lines += "Source: $src"
    $lines += "Started: $(Get-Date -Format o)"

    try {
        Write-Host "Compiling source with ArabicCompiler: $name"
        # generate C (and other outputs if desired)
        $proc = Start-Process -FilePath $compiler -ArgumentList "`"$src`" --c --asm --ir" -NoNewWindow -PassThru -Wait -RedirectStandardOutput "$($logFile).stdout" -RedirectStandardError "$($logFile).stderr"
        $exit = $proc.ExitCode
        $lines += "ArabicCompiler exit code: $exit"
        $lines += "ArabicCompiler stdout:";
        $lines += Get-Content -ErrorAction SilentlyContinue -Path "$($logFile).stdout"
        $lines += "ArabicCompiler stderr:";
        $lines += Get-Content -ErrorAction SilentlyContinue -Path "$($logFile).stderr"

        if ($exit -ne 0) {
            $lines += "Result: compile-source-failed"
            $summary += "$name: compile-source-failed"
            $lines | Out-File -FilePath $logFile -Encoding utf8
            continue
        }

        # expect C file at $baseOut + ".c"
        $cfile = $baseOut + ".c"
        if (-not (Test-Path $cfile)) {
            $lines += "Result: c-not-generated"
            $summary += "$name: c-not-generated"
            $lines | Out-File -FilePath $logFile -Encoding utf8
            continue
        }

        # compile C with gcc
        Write-Host "Compiling generated C with gcc: $name"
        $exe = Join-Path $logsDir ($baseNoExt + ".exe")
        & gcc "${cfile}" -o "${exe}" 2> $gccErr
        if ($LASTEXITCODE -ne 0) {
            $lines += "gcc failed with exit $LASTEXITCODE"
            $lines += Get-Content -ErrorAction SilentlyContinue -Path $gccErr
            $summary += "$name: gcc-failed"
            $lines | Out-File -FilePath $logFile -Encoding utf8
            continue
        }

        # run the executable
        Write-Host "Running executable: $name"
        & $exe > $runOut 2> $runErr
        $runExit = $LASTEXITCODE
        $lines += "Program exit code: $runExit"
        $lines += "Program stdout:";
        $lines += Get-Content -ErrorAction SilentlyContinue -Path $runOut
        $lines += "Program stderr:";
        $lines += Get-Content -ErrorAction SilentlyContinue -Path $runErr

        if ($runExit -eq 0) {
            $lines += "Result: ok"
            $summary += "$name: ok"
        }
        else {
            $lines += "Result: runtime-failed"
            $summary += "$name: runtime-failed"
        }

        $lines | Out-File -FilePath $logFile -Encoding utf8
    }
    catch {
        $lines += "Exception: $_"
        $lines | Out-File -FilePath $logFile -Encoding utf8
        $summary += "$name: exception"
    }
}

# write summary
$summaryFile = Join-Path $logsDir "summary.txt"
$summary | Out-File -FilePath $summaryFile -Encoding utf8
Write-Host "Done. Logs in: $logsDir"

```


---

## 📄 `./scripts/run_examples_tests.ps1`

```powershell
$ErrorActionPreference = 'Stop'

$repoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Resolve-Path "$repoRoot\.." | Select-Object -ExpandProperty Path
$compiler = Join-Path $repoRoot "Compiler\build\Release\ArabicCompiler.exe"
$examplesDir = Join-Path $repoRoot "Examples"
$tempDir = Join-Path $repoRoot "build_tests_expected"
$logsDir = Join-Path $repoRoot "build_tests_logs_2" # The one change I'm keeping

Write-Host "Repo root: $repoRoot"
Write-Host "Compiler: $compiler"
Write-Host "Examples: $examplesDir"

if (-not (Test-Path $compiler)) {
    Write-Error "Compiler executable not found at $compiler"
    exit 2
}

# Prepare folders
if (Test-Path $tempDir) { Remove-Item -Recurse -Force $tempDir }
if (Test-Path $logsDir) { Remove-Item -Recurse -Force $logsDir }
New-Item -ItemType Directory -Force -Path $tempDir | Out-Null
New-Item -ItemType Directory -Force -Path $logsDir | Out-Null

# Detect gcc
$gccPath = $null
try {
    $gccVersion = & gcc --version 2>$null
    if ($LASTEXITCODE -eq 0) { $gccPath = "gcc" }
} catch { }

Write-Host "GCC available: $([bool]$gccPath)"

$summary = @()

# Back up expected outputs (if present)
Get-ChildItem -Path $examplesDir -Filter *.arabic -File | ForEach-Object {
    $arabic = $_.FullName
    $base = [System.IO.Path]::GetFileNameWithoutExtension($_.Name)
    $expectedFiles = @("${base}_output.c","${base}_output.asm","${base}_output_intermediate.txt")
    foreach ($ef in $expectedFiles) {
        $src = Join-Path $examplesDir $ef
        if (Test-Path $src) {
            Copy-Item -Force -Path $src -Destination (Join-Path $tempDir $ef)
        }
    }
}

# Run tests
Get-ChildItem -Path $examplesDir -Filter *.arabic -File | ForEach-Object {
    $arabic = $_.FullName
    $base = [System.IO.Path]::GetFileNameWithoutExtension($_.Name)
    Write-Host "\n=== Testing: $($_.Name) ==="

    $logFile = Join-Path $logsDir "${base}.log"
    $errFile = Join-Path $logsDir "${base}.err"

    # Run compiler with all outputs
    Write-Host "Running compiler..."
    & $compiler $arabic --all > $logFile 2> $errFile
    $exit = $LASTEXITCODE

    $out = Get-Content -Raw $logFile
    $err = Get-Content -Raw $errFile

    if ($exit -ne 0) {
        Write-Host "[FAIL] Compiler exited with code $exit"
        Write-Host $err
        $summary += [pscustomobject]@{Example=$_.Name; Result="CompilerError"; Details=$err}
        return
    }

    Write-Host "[OK] Compiler returned 0"

    # Compare generated files with backups (if backups existed)
    $differences = @()
    $expectedFiles = @("${base}_output.c","${base}_output.asm","${base}_output_intermediate.txt")
    foreach ($ef in $expectedFiles) {
        $gen = Join-Path $examplesDir $ef
        $bak = Join-Path $tempDir $ef
        if (Test-Path $bak) {
            if (-not (Test-Path $gen)) {
                $differences += ($ef + ': MISSING')
            } else {
                $left = Get-Content -Raw $bak
                $right = Get-Content -Raw $gen
                if ($left -ne $right) {
                    $diff = Compare-Object -ReferenceObject ($left -split "\r?\n") -DifferenceObject ($right -split "\r?\n") -SyncWindow 0
                    $differences += ($ef + ': DIFFER')
                    $differences += $diff | Out-String
                } else {
                    $differences += ($ef + ': MATCH')
                }
            }
        } else {
            if (Test-Path $gen) {
                $differences += ($ef + ': GENERATED (no baseline)')
            } else {
                $differences += ($ef + ': NOT GENERATED')
            }
        }
    }

    # Try to compile and run C if available
    $runOutput = $null
    $cFile = Join-Path $examplesDir "${base}_output.c"
    if ($gccPath -and (Test-Path $cFile)) {
        $exePath = Join-Path $logsDir "${base}_run.exe"
        Write-Host "Compiling generated C with gcc..."
        & gcc "$cFile" -o "$exePath" 2> "$errFile"
        if ($LASTEXITCODE -ne 0) {
            $compileErr = Get-Content -Raw $errFile
            Write-Host "gcc failed:\n$compileErr"
            $summary += [pscustomobject]@{Example=$_.Name; Result="GccFail"; Details=$compileErr}
        } else {
            Write-Host "Running executable..."
            $runOutput = & "$exePath"
            Write-Host "---- Program output ----"
            Write-Host $runOutput
            Write-Host "------------------------"
            $summary += [pscustomobject]@{Example=$_.Name; Result="OkRun"; Details=$runOutput}
        }
    } else {
        $summary += [pscustomobject]@{Example=$_.Name; Result="CompiledOnly"; Details=(($differences -join "\n"))}
    }

    if ($differences.Count -gt 0) {
        Write-Host "Differences:"
        $differences | ForEach-Object { Write-Host "  $_" }
    }
}

# Print summary
Write-Host "\n=== Summary ==="
$summary | Format-Table -AutoSize

# Cleanup: restore backups
Get-ChildItem -Path $tempDir -File | ForEach-Object {
    $dest = Join-Path $examplesDir $_.Name
    Copy-Item -Force -Path $_.FullName -Destination $dest
}

Write-Host "Backups restored to Examples/ (cleanup)"

exit 0
```


---

## 📄 `./scripts/run_generated_c_examples.ps1`

```powershell
$ErrorActionPreference = 'Stop'

# Paths
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Resolve-Path "$scriptDir\.." | Select-Object -ExpandProperty Path
$examples = Join-Path $repoRoot "Examples"
$logs = Join-Path $repoRoot "build_tests_logs"
New-Item -Path $logs -ItemType Directory -Force | Out-Null

# Helper: write summary lines
$summary = @()

# Check gcc availability
try {
    & gcc --version > $null 2>&1
} catch {
    Write-Host "Warning: gcc not found in PATH. The script will skip compilation/execution but will still report available generated C files." -ForegroundColor Yellow
    $gccAvailable = $false
} finally {
    if ($LASTEXITCODE -eq 0) { $gccAvailable = $true }
}

Get-ChildItem -Path $examples -Filter '*_output.c' -File | ForEach-Object {
    $c = $_.FullName
    $name = $_.BaseName -replace '_output$',''
    $exe = Join-Path $logs "$name`_run.exe"
    $gccErr = Join-Path $logs "$name`_gcc.err"
    $runOut = Join-Path $logs "$name`_run.out"
    $runErr = Join-Path $logs "$name`_run.err"
    $log = Join-Path $logs "$name`.log"

    Write-Host "Processing $($_.Name) -> $name";
    $lines = @();
    $lines += "C file: $c"

    if (-not $gccAvailable) {
        $lines += "gcc not available: skipping compile/run"
        $lines | Out-File -FilePath $log -Encoding utf8
        $summary += "$name: skipped (no gcc)"
        return
    }

    # Compile (capture stderr)
    Write-Host "Compiling $name..."
    & gcc $c -o $exe 2> $gccErr
    if ($LASTEXITCODE -ne 0) {
        $lines += "gcc failed. See $gccErr"
        $lines += Get-Content -ErrorAction SilentlyContinue -Path $gccErr
        $lines | Out-File -FilePath $log -Encoding utf8
        $summary += "$name: compile-failed"
        return
    }

    # Run executable and capture stdout/stderr
    Write-Host "Running $name..."
    try {
        & $exe > $runOut 2> $runErr
        $lines += "--- stdout ---"
        $lines += Get-Content -ErrorAction SilentlyContinue -Path $runOut
        $lines += "--- stderr ---"
        $lines += Get-Content -ErrorAction SilentlyContinue -Path $runErr
        $lines | Out-File -FilePath $log -Encoding utf8
        $summary += "$name: ok"
    } catch {
        $lines += "Execution failed: $_"
        $lines | Out-File -FilePath $log -Encoding utf8
        $summary += "$name: run-failed"
    }
}

# Write summary
$summaryFile = Join-Path $logs "summary.txt"
$summary | Out-File -FilePath $summaryFile -Encoding utf8
Write-Host "Done. Logs and outputs in: $logs"
```


---

## 📄 `./compile_arabic.sh`

```bash
#!/bin/bash
# Arabic Compiler Wrapper Script for macOS
# Usage: ./compile_arabic.sh <file.arabic> [options]

# Set colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
COMPILER="$SCRIPT_DIR/Compiler/ArabicCompiler_mac"

# Check if compiler exists
if [ ! -f "$COMPILER" ]; then
    echo -e "${RED}❌ خطأ: المترجم غير موجود في: $COMPILER${NC}"
    echo -e "${YELLOW}يرجى بناء المترجم أولاً باستخدام:${NC}"
    echo "  cd $SCRIPT_DIR/Compiler"
    echo "  clang++ -std=c++17 -o ArabicCompiler_mac src/*.cpp"
    exit 1
fi

# Check if input file is provided
if [ $# -eq 0 ]; then
    echo -e "${RED}❌ خطأ: يرجى تحديد ملف المصدر${NC}"
    echo -e "${BLUE}الاستخدام:${NC}"
    echo "  $0 <ملف.arabic> [--c|--asm|--ir|--all]"
    echo ""
    echo -e "${BLUE}أمثلة:${NC}"
    echo "  $0 Tests/test_01_program_structure.arabic --c"
    echo "  $0 Tests/test_08_for_loop.arabic --all"
    echo "  $0 program.arabic --c --run"
    exit 1
fi

INPUT_FILE="$1"
shift
OPTIONS="$@"
RUN_AFTER=false

# Check if --run option is specified
if [[ "$OPTIONS" == *"--run"* ]]; then
    RUN_AFTER=true
    OPTIONS="${OPTIONS//--run/}" # Remove --run from options
fi

# Default to --c if no option specified
if [ -z "$OPTIONS" ]; then
    OPTIONS="--c"
fi

# Get the directory and base name of input file
INPUT_DIR="$(cd "$(dirname "$INPUT_FILE")" && pwd)"
INPUT_BASE="$(basename "$INPUT_FILE" .arabic)"
INPUT_FULL="$INPUT_DIR/$(basename "$INPUT_FILE")"

# Change to input directory for output file generation
cd "$INPUT_DIR" || exit 1

# Run the compiler
echo -e "${BLUE}📝 جاري ترجمة: $INPUT_FILE${NC}"
"$COMPILER" "$INPUT_FULL" $OPTIONS

COMPILE_STATUS=$?

if [ $COMPILE_STATUS -eq 0 ]; then
    echo -e "${GREEN}✅ الترجمة تمت بنجاح!${NC}"
    
    # If --run is specified and C code was generated, compile and run
    if [ "$RUN_AFTER" = true ] && [ -f "${INPUT_BASE}_output.c" ]; then
        echo -e "${BLUE}🔨 جاري بناء الكود...${NC}"
        gcc "${INPUT_BASE}_output.c" -o "${INPUT_BASE}_program"
        
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}✅ البناء تم بنجاح!${NC}"
            echo -e "${BLUE}▶️  جاري تشغيل البرنامج...${NC}"
            echo -e "${YELLOW}================== الإخراج ==================${NC}"
            "./${INPUT_BASE}_program"
            echo -e "${YELLOW}==============================================${NC}"
            
            # Clean up executable
            rm -f "${INPUT_BASE}_program"
        else
            echo -e "${RED}❌ فشل البناء!${NC}"
            exit 1
        fi
    fi
else
    echo -e "${RED}❌ فشلت الترجمة!${NC}"
    exit 1
fi


```


---

## 📄 `./run_all_tests.sh`

```bash
#!/bin/bash
# Run all test files for the Arabic Compiler
# This tests the complete pipeline on macOS

GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
TESTS_DIR="$SCRIPT_DIR/Tests"
COMPILER="$SCRIPT_DIR/Compiler/ArabicCompiler_mac"

# Test files
TESTS=(
    "test_01_program_structure.arabic"
    "test_02_constants.arabic"
    "test_03_arrays.arabic"
    "test_06_if_else.arabic"
    "test_07_while_loop.arabic"
    "test_08_for_loop.arabic"
    "test_09_repeat_until.arabic"
    "test_10_expressions.arabic"
)

passed=0
failed=0
total=0

echo -e "${BLUE}════════════════════════════════════════════${NC}"
echo -e "${BLUE}   Arabic Compiler - Test Suite (macOS)    ${NC}"
echo -e "${BLUE}════════════════════════════════════════════${NC}"
echo ""

for test in "${TESTS[@]}"; do
    total=$((total + 1))
    test_file="$TESTS_DIR/$test"
    
    if [ ! -f "$test_file" ]; then
        echo -e "${YELLOW}⊘ SKIP${NC} $test (file not found)"
        continue
    fi
    
    echo -ne "${BLUE}Testing${NC} $test ... "
    
    # Compile the test
    cd "$TESTS_DIR"
    output=$("$COMPILER" "$test_file" --c 2>&1)
    status=$?
    
    if [ $status -eq 0 ]; then
        # Try to compile with GCC
        base_name=$(basename "$test_file" .arabic)
        if [ -f "${base_name}_output.c" ]; then
            gcc_output=$(gcc "${base_name}_output.c" -o "${base_name}_test" 2>&1)
            gcc_status=$?
            
            if [ $gcc_status -eq 0 ]; then
                # Run the executable
                run_output=$("./${base_name}_test" 2>&1)
                run_status=$?
                
                if [ $run_status -eq 0 ]; then
                    echo -e "${GREEN}✓ PASS${NC}"
                    passed=$((passed + 1))
                else
                    echo -e "${RED}✗ FAIL${NC} (runtime error)"
                    failed=$((failed + 1))
                fi
                
                # Clean up
                rm -f "${base_name}_test"
            else
                echo -e "${YELLOW}⚠ WARN${NC} (gcc compilation failed)"
                failed=$((failed + 1))
            fi
            
            # Clean up generated files
            rm -f "${base_name}_output.c"
            rm -f "${base_name}_output.asm"
            rm -f "${base_name}_output_intermediate.txt"
        else
            echo -e "${YELLOW}⚠ WARN${NC} (no C output generated)"
            failed=$((failed + 1))
        fi
    else
        echo -e "${RED}✗ FAIL${NC} (compilation error)"
        failed=$((failed + 1))
    fi
done

echo ""
echo -e "${BLUE}════════════════════════════════════════════${NC}"
echo -e "${BLUE}Results:${NC}"
echo -e "  ${GREEN}Passed:${NC} $passed"
echo -e "  ${RED}Failed:${NC} $failed"
echo -e "  ${BLUE}Total:${NC}  $total"

if [ $failed -eq 0 ]; then
    echo -e "\n${GREEN}🎉 All tests passed!${NC}"
    exit 0
else
    echo -e "\n${YELLOW}⚠️  Some tests failed${NC}"
    exit 1
fi


```


---

## 📄 `./README.md`

```markdown
# Arabic Programming Language IDE - بيئة تطوير متكاملة للغة العربية
# جزء من متطلبات مادة تصميم المترجمات
# الدكتور ألاكاديمي د | خالد محمد الكحسه 
# المساعد العملي م | أحمد الراشدي
![Arabic IDE](https://img.shields.io/badge/Language-Arabic-green)
![.NET](https://img.shields.io/badge/.NET-6.0-blue)
![C++](https://img.shields.io/badge/C++-17-red)

بيئة تطوير متكاملة (IDE) تمكن المبرمجين من كتابة وتنفيذ البرامج بلغة البرمجة العربية. تدعم الترجمة إلى عدة لغات برمجة وتوفر واجهة مستخدم كاملة باللغة العربية.

<img width="1600" height="900" alt="Image" src="https://github.com/user-attachments/assets/aa214c23-f25e-42fa-9f0d-969a19a05426" />
<img width="1600" height="900" alt="Image" src="https://github.com/user-attachments/assets/565947fd-df23-4534-b103-474bf6c22087" />

## ✨ المميزات الرئيسية

### 🎯 واجهة المستخدم
- **واجهة مستخدم كاملة بالعربية** - جميع العناصر والرسائل باللغة العربية
- **محرر نصوص متقدم** - دعم كامل للنصوص العربية مع تنسيق متقدم
- **تخطيط مرن** - نوافذ قابلة للتقسيم والتخصيص
- **شريط أدوات شامل** - وصول سريع للوظائف الأساسية
- **معلومات تفصيلية** - عرض السطر والعمود الحالي

### ⚡ أدوات التطوير
- **ترجمة فورية** - تحويل الكود العربي إلى لغات برمجة مختلفة
- **تعدد المخرجات** - دعم توليد كود Assembly، C، والكود الوسيط
- **تنفيذ مباشر** - تشغيل البرامج المترجمة مباشرة من الواجهة
- **إدارة ملفات متكاملة** - فتح، حفظ، حفظ كـ، وإنشاء ملفات جديدة
- **تلوين الإخراج** - تمييز الأخطاء والتحذيرات بألوان مختلفة

### ⌨️ اختصارات لوحة المفاتيح
| الاختصار | الوظيفة |
|----------|---------|
| `Ctrl + N` | إنشاء ملف جديد |
| `Ctrl + O` | فتح ملف موجود |
| `Ctrl + S` | حفظ الملف |
| `Ctrl + Shift + S` | حفظ الملف باسم جديد |
| `Ctrl + B` | ترجمة الكود فقط |
| `F5` | ترجمة وتنفيذ الكود |

## 🏗️ هيكل المشروع

```
ArabicCompiler/
├── 📁 IDE/                          # مشروع بيئة التطوير (C#)
│   ├── 📄 MainForm.cs              # النافذة الرئيسية للبرنامج
│   ├── 📄 Program.cs               # نقطة بداية التشغيل
│   └── 📄 ArabicIDE.csproj         # إعدادات مشروع .NET
├── 📁 src/                         # المترجم الأساسي (C++)
│   ├── 📄 Lexer.h/cpp             # المحلل اللغوي (Lexical Analyzer)
│   ├── 📄 Parser.h/cpp            # المحلل النحوي (Parser)
│   ├── 📄 Compiler.h/cpp          # المترجم الرئيسي
│   ├── 📄 main.cpp                # البرنامج الرئيسي للمترجم
│   └── 📄 AST.h                   # تعريف شجرة الاشتقاق النحوي
├── 📄 CMakeLists.txt              # إعدادات بناء المترجم
└── 📁 output/                     # الملفات الناتجة عن الترجمة
```

## 📋 متطلبات التشغيل

### للمترجم (C++):
- **CMake** الإصدار 3.10 أو أحدث
- **مترجم C++** يدعم معيار C++17 (GCC، Clang، أو MSVC)
- **أنظمة التشغيل**: Windows، Linux، أو macOS

### لبيئة التطوير (C#):
- **.NET 6.0** أو إصدار أحدث
- **Windows Forms** (مضمن مع .NET)
- **نظام التشغيل**: Windows

## 🚀 خطوات البناء والتشغيل

### 1. بناء المترجم (C++)

```bash
# إنشاء مجلد البناء
mkdir build
cd build

# تكوين المشروع باستخدام CMake
cmake ..

# بناء المشروع
cmake --build .

# نسخ الملف التنفيذي للمترجم
cp ArabicCompiler.exe ../

Or 

build.bat
```

### 2. بناء بيئة التطوير (C#)

```bash
# الانتقال لمجلد بيئة التطوير
cd IDE

# بناء المشروع
dotnet build

# تشغيل التطبيق
dotnet run
```

لإنشاء نسخة قابلة للتوزيع:

```bash
dotnet publish -c Release -r win-x64 --self-contained
```

### 3. التحقق من المتطلبات الإضافية

تأكد من توفر:
- `ArabicCompiler.exe` في المجلد الرئيسي للتطبيق
- `gcc` (اختياري) لتنفيذ كود C المترجم

## 💻 أمثلة استخدام

### مثال برنامج عربي بسيط

```arabic
برنامج مثال؛

متغير س = 10؛
متغير ناتج = 0؛

ناتج = س * 2 + 5؛

اذا ناتج > 15 فان
    اطبع "الناتج كبير: "؛
    اطبع ناتج؛
وإلا
    اطبع "الناتج صغير: "؛
    اطبع ناتج؛
نهاية؛

اطبع "نهاية البرنامج"؛
```

### الكلمات المحجوزة المدعومة

| الكلمة | الوظيفة |
|--------|----------|
| `برنامج` | بداية تعريف البرنامج |
| `متغير` | تعريف متغير جديد |
| `ثابت` | تعريف ثابت |
| `اذا` | بداية الجملة الشرطية |
| `فان` | الجزء التنفيذي عند تحقق الشرط |
| `وإلا` | الجزء التنفيذي عند عدم تحقق الشرط |
| `طالما` | حلقة التكرار while |
| `كرر` | بداية حلقة repeat |
| `حتى` | شرط إنهاء حلقة repeat |
| `اطبع` | أمر الطباعة للإخراج |
| `اقرأ` | أمر القراءة من الإدخال |
| `نهاية` | نهاية الكتلة البرمجية |

## 🎯 أنواع المخرجات المدعومة

### 1. كود Assembly (MIPS)

```asm
.data
س: .word 10
ناتج: .word 0
.text
.globl main
main:
    # كود الترجمة يظهر هنا...
```

### 2. كود C

```c
#include <stdio.h>
int main() {
    int س = 10;
    int ناتج = 0;
    // كود الترجمة يظهر هنا...
    return 0;
}
```

### 3. الكود الوسيط

```
LOAD t0, 10
STORE س, t0
LOAD t1, س
MUL t2, t1, 2
ADD t3, t2, 5
STORE ناتج, t3
```

## 🔧 استكشاف الأخطاء وإصلاحها

### المشاكل الشائعة وحلولها:

1. **المترجم غير موجود**
   - **السبب**: لم يتم بناء `ArabicCompiler.exe` بشكل صحيح
   - **الحل**: تأكد من تنفيذ خطوات البناء للمترجم C++

2. **أخطاء في بناء بيئة التطوير**
   - **السبب**: إصدار .NET غير مثبت أو غير متوافق
   - **الحل**: تثبيت .NET 6.0 SDK أو أحدث

3. **مشاكل في تنفيذ البرامج المترجمة**
   - **السبب**: عدم وجود GCC أو إعدادات خاطئة
   - **الحل**: تثبيت MinGW-w64 أو التحقق من إعدادات PATH

### رسائل الخطأ الشائعة:

- **"المترجم غير موجود"**: تأكد من وجود `ArabicCompiler.exe` في المجلد الصحيح
- **"خطأ في الترجمة"**: راجع الكود العربي للتأكد من صحته النحوية
- **"لا يمكن تنفيذ البرنامج"**: تحقق من تثبيت GCC وإعدادات البيئة

## 🤝 المساهمة في المشروع

نرحب بمساهماتكم لتطوير وتحسين هذه البيئة! يمكنكم المساعدة من خلال:

1. **الإبلاغ عن الأخطاء** - اكتشاف ومشاركة المشاكل التقنية
2. **اقتراح ميزات جديدة** - أفكار لتطوير الوظائف الحالية
3. **تحسين الوثائق** - تطوير الدليل والشرح
4. **تطوير المترجم** - إضافة مميزات جديدة للمترجم الأساسي

## 📄 الترخيص

هذا المشروع مفتوح المصدر ويخضع لشروط ترخيص MIT.

## 📞 الدعم والاتصال

للحصول على الدعم التقني أو الإبلاغ عن مشاكل، يرجى استخدام نظام إدارة المشكلات (Issues) في المستودع.

---

**المطور**: فريق DemoSoft
**الإصدار**: 1.0.0  
**تاريخ الإصدار**: ديسمبر 2025  
**الحالة**: قيد التطوير النشط

```


---

## 📄 `./README_macOS.md`

```markdown
# Arabic Compiler on macOS 🍎

## ✅ **Yes, It Works on Your MacBook Pro M2!**

The C++ compiler runs perfectly on macOS. Only the Windows Forms IDE won't work.

---

## 🚀 **Quick Start Guide**

### **What Works:**
✅ Core C++ compiler (fully functional)  
✅ Command-line compilation  
✅ All language features (variables, arrays, loops, procedures, etc.)  
✅ Code generation (C, Assembly, Intermediate)  
✅ Test suite  
✅ GCC compilation and execution  

### **What Doesn't Work:**
❌ C# Windows Forms IDE (Windows only)

---

## 📦 **Setup (Already Done for You!)**

The compiler has been built and is ready at:
```
/Users/salahmac/Desktop/MLProjects/ArabicCompiler/Compiler/ArabicCompiler_mac
```

If you need to rebuild it:
```bash
cd Compiler
clang++ -std=c++17 -o ArabicCompiler_mac src/*.cpp
```

---

## 💻 **Usage**

### **Method 1: Using the Wrapper Script (Recommended)**

The wrapper script `compile_arabic.sh` makes everything easier:

```bash
# Compile to C code
./compile_arabic.sh Tests/test_01_program_structure.arabic --c

# Compile and run immediately
./compile_arabic.sh Tests/test_08_for_loop.arabic --c --run

# Generate all output types (C, Assembly, Intermediate)
./compile_arabic.sh Tests/test_06_if_else.arabic --all

# Just Assembly
./compile_arabic.sh program.arabic --asm

# Just Intermediate code
./compile_arabic.sh program.arabic --ir
```

### **Method 2: Direct Compiler Usage**

```bash
cd Tests
../Compiler/ArabicCompiler_mac test_01_program_structure.arabic --c
gcc test_01_program_structure_output.c -o program
./program
```

---

## 📝 **Compiler Options**

| Option | Description |
|--------|-------------|
| `--c` | Generate C code |
| `--asm` | Generate MIPS Assembly |
| `--ir` | Generate Intermediate representation |
| `--all` | Generate all outputs |
| `--tokens` | Show lexical tokens |
| `--ast` | Show Abstract Syntax Tree |
| `--debug-lexer` | Debug lexer issues |
| `--help` | Show help message |

---

## 🧪 **Testing**

### **Test Individual Examples:**

```bash
# Simple program
./compile_arabic.sh Tests/test_01_program_structure.arabic --c --run

# If-else statements
./compile_arabic.sh Tests/test_06_if_else.arabic --c --run

# For loops
./compile_arabic.sh Tests/test_08_for_loop.arabic --c --run

# Arrays
./compile_arabic.sh Tests/test_03_arrays.arabic --c --run

# Procedures
./compile_arabic.sh Tests/test_05_procedures.arabic --c --run
```

### **Test Results (Verified on M2):**

✅ **test_01_program_structure.arabic** - Prints "مرحبا بك"  
✅ **test_06_if_else.arabic** - If-else conditions work  
✅ **test_08_for_loop.arabic** - Prints 1, 2, 3, 4, 5  
✅ **test_03_arrays.arabic** - Array indexing works (10, 20, 30)  

---

## 📚 **Language Features**

### **1. Variables and Constants**
```arabic
متغير س : صحيح ؛
ثابت ط = 3.14 ؛
```

### **2. Arrays**
```arabic
متغير أرقام : قائمة[5] من صحيح ؛
أرقام[0] = 10 ؛
اطبع أرقام[0] ؛
```

### **3. Control Flow**
```arabic
اذا س > 10 فان
    اطبع "كبير" ؛
والا
    اطبع "صغير" ؛
نهاية ؛
```

### **4. Loops**
```arabic
// For loop
كرر (ع = 1 الى 5)
    اطبع ع ؛
نهاية ؛

// While loop
طالما س < 10 فان
    س = س + 1 ؛
نهاية ؛
```

### **5. Procedures**
```arabic
إجراء الجمع (بالقيمة أ : صحيح ؛ بالقيمة ب : صحيح) ؛
    اطبع أ + ب ؛
نهاية ؛
```

---

## 🔧 **Troubleshooting**

### **1. Permission Denied on compile_arabic.sh**
```bash
chmod +x compile_arabic.sh
```

### **2. Compiler Not Found**
Rebuild it:
```bash
cd Compiler
clang++ -std=c++17 -o ArabicCompiler_mac src/*.cpp
```

### **3. GCC Not Installed**
Install via Homebrew (requires sudo):
```bash
# Fix Homebrew permissions first if needed:
sudo chown -R $(whoami) /opt/homebrew

# Then install gcc:
brew install gcc
```

Or use the default compiler:
```bash
gcc test_output.c -o program
# or
clang test_output.c -o program
```

---

## 📊 **Project Structure**

```
ArabicCompiler/
├── compile_arabic.sh          # ⭐ Convenient wrapper (NEW!)
├── Compiler/
│   ├── ArabicCompiler_mac     # ⭐ macOS executable (BUILT!)
│   ├── src/
│   │   ├── main.cpp
│   │   ├── Lexer.cpp/h
│   │   ├── Parser.cpp/h
│   │   ├── Compiler.cpp/h
│   │   └── AST.cpp/h
│   └── CMakeLists.txt
├── Tests/                     # ⭐ 50+ test files
│   ├── test_01_program_structure.arabic
│   ├── test_03_arrays.arabic
│   ├── test_06_if_else.arabic
│   ├── test_08_for_loop.arabic
│   └── ...
├── Examples/                  # Additional examples
└── IDE/                       # ❌ Windows Forms (won't work)
```

---

## 🎯 **Compilation Pipeline**

```
Arabic Source (.arabic)
    ↓
[Lexer] → Tokens
    ↓
[Parser] → Abstract Syntax Tree (AST)
    ↓
[Compiler] → Intermediate Code
    ↓
[Code Generator] → C / Assembly / IR
    ↓
[gcc/clang] → Native Executable
    ↓
[Execute] → Output
```

---

## 💡 **Quick Examples**

### **Hello World**
```arabic
برنامج مرحبا ؛
    اطبع "مرحبا بالعالم" ؛
.
```

Compile and run:
```bash
./compile_arabic.sh hello.arabic --c --run
```

### **Simple Loop**
```arabic
برنامج حلقة ؛
    متغير ع : صحيح ؛
    كرر (ع = 1 الى 10)
        اطبع ع ؛
    نهاية ؛
.
```

---

## 🌟 **Advantages on macOS**

✅ **Native Performance** - Compiled with clang++, optimized for M2  
✅ **Terminal Integration** - Perfect for scripting and automation  
✅ **GCC Available** - Can compile and run generated C code  
✅ **No .NET Required** - Core compiler is pure C++  
✅ **UNIX Tools** - Easy integration with shell scripts  

---

## 📖 **Further Reading**

- Original README: `README.md`
- Grammar Rules: `التقرير (Report)/قواعد اللغة.txt`
- Test Files: All in `Tests/` directory
- Examples: Check `Examples/` folder

---

## 🎓 **Summary**

Your MacBook Pro M2 can fully compile and run Arabic programs! The compiler works perfectly with all features:
- ✅ Lexical analysis
- ✅ Parsing
- ✅ Code generation (C, Assembly, Intermediate)
- ✅ Full language support (loops, arrays, procedures, etc.)

**You're all set! Start coding in Arabic! 🚀**


```


---

## 📄 `./LICENSE`

```text
MIT License

Copyright (c) 2025 DemoSoft Team
Developed by:
Tareq Al-Omari, Ayman Qamhan,
Hazem Al-Omari, Diaa Al-Hadrami, Ali Al-Qawas

+967715299909
tareq.software.devloper@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

```


---

## 📄 Grammar Rules (قواعد اللغة)

```text
جميع قواعد اللغة (Grammar Rules) كما هي مذكورة نصاً في ملف قواعد اللغة.pdf:

1. البرنامج والكتلة الرئيسية (صفحة 1)

برنامج : برنامج <اسم برنامج> <فاصلة منقوطة> <كتلة برمجية> <نقطة> 


<كتلة برمجية> :- [ <جزء_التعريفات> ] <قائمة التعليمات> 


<جزء_التعريفات> : [ <تعريف الثوابت> ] [ <تعريف الانواع> ] [ <تعريف المتغيرات> ] [ <تعريف_الاجراءات> ] 

2. التعريفات (صفحات 1 و 2)
الثوابت

<تعريف_الثوابت> : ثابت <تعريف_ثابت> ( <تعريف_ثابت> )* 


<تعريف_ثابت> : <اسم_ثابت> <علامة يساوي> <قيمة ثابت> <فاصلة_منقوطة> 

الأنواع

<تعريف الانواع> : نوع <تعريف نوع> ( <تعريف_نوع> )* 


<تعريف_نوع> : <اسم_نوع> <علامة يساوي> <نوع_مركب> <فاصلة_منقوطة> 


<نوع_مركب> : <نوع_قائمة> | <نوع سجل> 


<نوع قائمة> : قائمة <قوس مربع يمين> <عدد> <قوس_مربع يسار> من <نوع بيانات> 


<نوع_سجل> : سجل <قوس_مجموعة_يمين> <قائمة حقول> <قوس_مجموعة_يسار> 


<قائمة حقول> :- <تعريف_حقل> ( <فاصلة منقوطة> <تعريف حقل> )* 


<تعريف_حقل> : <اسم_حقل> ( <فاصلة> <معرف> )* <نقطتين> <نوع بيانات> 

المتغيرات

<تعريف المتغيرات> :- متغير <تعريف_متغير> ( <تعريف_متغير> )* 


<تعريف_متغير> : <مجموعة متغيرات> <فاصلة منقوطة> 


<مجموعة_متغيرات> : <اسم_متغير> ( <فاصلة> <اسم متغير> )* <نقطتين> <نوع بيانات> 

الإجراءات (الدوال)

<تعريف الاجراءات> : <تعريف اجراء> ( <تعريف_اجراء> )* 


<تعريف_اجراء> : <رأس_اجراء> <كتلة اجراء> <فاصلة_منقوطة> 


<رأس_اجراء> :- اجراء <اسم اجراء> <قوس_يمين> [ <قائمة_معلمات شكلية> ] <قوس يسار> <فاصلة_منقوطة> 


<كتلة اجراء> :- <كتلة برمجية> 


<قائمة_معلمات شكلية> :- <تعريف_معلمة> ( <فاصلة منقوطة> <تعريف_معلمة> )* 


<تعريف_معلمة> :- [ بالقيمة | بالمرجع ] <مجموعة متغيرات> 

أنواع البيانات

<نوع بيانات> :- صحيح | حقيقي | منطقي | حرفي | خيط_رمزي | <اسم نوع> 

3. التعليمات (صفحة 3)

<قائمة تعليمات> : <قوس_مجموعة يمين> <تعليمة> ( <فاصلة_منقوطة> <تعليمة> )* <قوس_مجموعة_يسار> 


<تعليمة> :- <تعليمة اسناد> | <تعليمة ادخال> | <تعليمة اخراج> | <تعليمة استدعاء> | <تعليمة شرط> | <تعليمة تكرار> | <قائمة_تعليمات> | <فراغ> 


<تعليمة اسناد> : <متغير وصول> <علامة يساوي> <تعبير> 


<تعليمة ادخال> : اقرا <قوس يمين> <متغير وصول> <قوس يسار> 


<تعليمة اخراج> : اطبع <قوس يمين> <قائمة طباعة> <قوس يسار> 


<قائمة طباعة> : <عنصر طباعة> ( <فاصلة> <عنصر طباعة> )* 


<عنصر طباعة> : <متغير وصول> | <حرفي> 


<تعليمة استدعاء> : <اسم_اجراء> <قوس يمين> [ <قائمة_معلمات حقيقية> ] <قوس يسار> 


<قائمة معلمات_حقيقية> :- <معلمة حقيقية> ( <فاصلة> <معلمة حقيقية> )* 


<معلمة_حقيقية> : <تعبير> | <متغير وصول> 

4. التحكم (الشرط والتكرار - صفحة 3 و 4)
جمل الشرط

<تعليمة شرط> :- <تعليمة اذا> | <تعليمة_اذا_و_الا> | <تعليمة_اذا_و_الا_اذا> 


<تعليمة اذا> :- اذا <قوس يمين> <شرط> <قوس يسار> فان <تعليمة> 


<شرط> :- <تعبير> 


<تعليمة _اذا_و_الا> : <تعليمة اذا> والا <تعليمة> 


<تعليمة_اذا_و_الا_اذا> :- <تعليمة اذا> ( <فاصلة منقوطة> والا <تعليمة اذا> )* [ والا <تعليمة> ] 

جمل التكرار

<تعليمة تكرار> :- <تعليمة كرر> | <تعليمة كرر طالما> | <تعليمة كرر حتى> 


<تعليمة كرر> :- كرر <قوس يمين> <مجال_التكرار> <قوس_يسار> <تعليمة> 


<مجال_التكرار> :- <اسم_متغير> <علامة يساوي> <تعبير> الى <تعبير> [ اضف <تعبير> ] 


<تعليمة كرر طالما> : طالما <قوس يمين> <شرط> <قوس يسار> استمر <تعليمة> 


<تعليمة كرر حتى> :- اعد <تعليمة> حتى <قوس يمين> <شرط> <قوس يسار> 

5. التعبيرات (صفحة 4 و 5)

<تعبير> : <تعبير بسيط> [ <معامل_ربط> <تعبير بسيط> ] 


<تعبير بسيط> : [ <معامل اشارة> ] <حد> ( <معامل_جمع> <حد> )* 


<معامل_ربط> : => | < | > | =< | == | =! 


<معامل اشارة> :- + | - 


<معامل_جمع> :- <معامل اشارة> | || 


<حد> := <عامل> ( <معامل_ضرب> <عامل> )* 


<معامل_ضرب> : * | / | \ | % | && 


<عامل> : <قيمة ثابت> | <متغير وصول> | <قوس يمين> <تعبير> <قوس_يسار> | ! <عامل> 

6. الوصول للمتغيرات والقيم (صفحة 5 و 6)
الوصول

<متغير وصول> : <اسم_متغير> | <اسم_نوع> <مختار> 


<مختار> : <مختار مفهرس> | <حقل_مختار> 


<مختار مفهرس> : <قوس_مربع يمين> <تعبير> <قوس_مربع يسار> 


<حقل_مختار> :- <نقطة> <اسم_حقل> 

القيم (الثوابت)

<قيمة ثابت> :- <رقمي> | <حرفي> | <منطقي> | <اسم_ثابت> 


<رقمي> : <رقم صحيح> | <رقم حقيقي> 


<رقم صحيح> : <رقم> ( <رقم> )* 


<رقم_حقيقي> : <رقم صحيح> <نقطة> <رقم_صحيح> 


<حرفي> : <سلسلة رمزية> | <رمز مفرد> 


<سلسلة رمزية> : <علامة تنصيص مزدوجة> ( <رمز> )* <علامة تنصيص_مزدوجة> 


<رمز مفرد> : <علامة تنصيص يمين> ( <رمز> ) <علامة تنصيص يسار> 


<منطقي> :- صح | خطأ 

7. المعرفات والرموز الأساسية (صفحة 6 و 7)
المعرفات

<اسم برنامج> :- <معرف> 


<اسم_ثابت> :- <معرف> 


<اسم_نوع> :- <معرف> 


<اسم_حقل> :- <معرف> 


<اسم_متغير> :- <معرف> 


<اسم_اجراء> := <معرف> 


<معرف> : <حرف> ( <حرف> | <رقم> )* 


<رقم> : 0 | 1 | .. 


<حرف> := ا | ب | .. | ي 


<رمز> : كل الرموز المستخدمة في النظام 

الرموز الطرفية (Terminals)

<نقطة> :- . 


<نقطتين> :- : 


<فاصلة_منقوطة> :- ؛ 


<فاصلة> := , 


<علامة يساوي> := = 


<قوس_مجموعة_يمين> := { 


<قوس_مجموعة_يسار> :- } 


<قوس_مربع يمين> :- [ 




<قوس_مربع يسار> := ] 



<قوس يمين> :- ( 




<قوس يسار> :- ) 




<علامة تنصيص_مزدوجة> :- " 




<علامة تنصيص يمين> :- ' 




<علامة تنصيص يسار> := '
```


---

## End of Codebase

This consolidated file contains all essential source code for the Arabic Programming Language Compiler.

**Components Included**:
- ✅ Lexer, Parser, AST, Compiler (C++)
- ✅ IDE (C#)
- ✅ Build system (CMake)
- ✅ Test cases
- ✅ Example programs
- ✅ Scripts and documentation

**Usage**: This file can be provided to AI assistants for comprehensive analysis, bug fixing, feature development, or documentation generation.

