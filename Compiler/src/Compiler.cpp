#include "Compiler.h"
#include <sstream>
#include <iomanip>
#include <iostream>

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

    if (program)
    {
        compileProgram(program.get());
        emit(InstructionType::HALT);
    }

    return instructions;
}

void Compiler::compileProgram(ProgramNode *program)
{
    // معالجة التعريفات أولاً
    for (auto &declaration : program->declarations)
    {
        compileStatement(declaration.get());
    }

    // ثم معالجة الجمل
    for (auto &statement : program->statements)
    {
        compileStatement(statement.get());
    }
}

void Compiler::compileStatement(ASTNode *statement)
{
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
}

void Compiler::compileVariableDeclaration(VariableDeclarationNode *node)
{
    // تخزين نوع المتغير في جدول الرموز
    symbolTable[node->name] = "متغير";

    if (node->initialValue)
    {
        std::string temp = compileExpression(node->initialValue.get());
        emit(InstructionType::STORE, node->name, temp);
    }
    else
    {
        // تهيئة بالقيمة الافتراضية
        std::string temp = generateTempVar();
        emit(InstructionType::LOAD, temp, "0");
        emit(InstructionType::STORE, node->name, temp);
    }
}

void Compiler::compileConstantDeclaration(ConstantDeclarationNode *node)
{
    // تخزين الثابت في جدول الرموز
    symbolTable[node->name] = "ثابت";

    std::string temp = compileExpression(node->value.get());
    emit(InstructionType::STORE, node->name, temp);
}

void Compiler::compileAssignment(AssignmentNode *node)
{
    std::string temp = compileExpression(node->value.get());
    emit(InstructionType::STORE, node->variableName, temp);
}

void Compiler::compilePrint(PrintNode *node)
{
    // طباعة السلاسل النصية مباشرة بدون إنشاء مؤقتات
    if (auto literal = dynamic_cast<LiteralNode *>(node->expression.get()))
    {
        if (literal->type == TokenType::STRING_LITERAL)
        {
            std::string label = getStringLabel(literal->value);
            emit(InstructionType::PRINT, "STRING", label);
            return;
        }
    }

    std::string temp = compileExpression(node->expression.get());
    emit(InstructionType::PRINT, "VALUE", temp);
}

void Compiler::compileRead(ReadNode *node)
{
    emit(InstructionType::READ, node->variableName);
}

void Compiler::compileIf(IfNode *node)
{
    std::string elseLabel = generateLabel();
    std::string endLabel = generateLabel();

    std::string conditionTemp = compileExpression(node->condition.get());
    emit(InstructionType::JZ, conditionTemp, elseLabel);

    // فرع then
    for (auto &stmt : node->thenBranch)
    {
        compileStatement(stmt.get());
    }

    if (!node->elseBranch.empty())
    {
        emit(InstructionType::JMP, endLabel);
    }

    emit(InstructionType::LABEL, elseLabel);

    // فرع else
    for (auto &stmt : node->elseBranch)
    {
        compileStatement(stmt.get());
    }

    if (!node->elseBranch.empty())
    {
        emit(InstructionType::LABEL, endLabel);
    }
}

void Compiler::compileWhile(WhileNode *node)
{
    std::string startLabel = generateLabel();
    std::string endLabel = generateLabel();

    emit(InstructionType::LABEL, startLabel);

    std::string conditionTemp = compileExpression(node->condition.get());
    emit(InstructionType::JZ, conditionTemp, endLabel);

    for (auto &stmt : node->body)
    {
        compileStatement(stmt.get());
    }

    emit(InstructionType::JMP, startLabel);
    emit(InstructionType::LABEL, endLabel);
}

void Compiler::compileRepeat(RepeatNode *node)
{
    std::string startLabel = generateLabel();
    std::string endLabel = generateLabel();

    emit(InstructionType::LABEL, startLabel);

    for (auto &stmt : node->body)
    {
        compileStatement(stmt.get());
    }

    std::string conditionTemp = compileExpression(node->condition.get());
    emit(InstructionType::JZ, conditionTemp, startLabel);
}

std::string Compiler::compileExpression(ASTNode *expr)
{
    if (auto binaryOp = dynamic_cast<BinaryOpNode *>(expr))
    {
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
            // CMP لا يُنتج قيمة مباشرة، لذا نحاكي تقييمًا بوليانيًا 0/1
            std::string trueLabel = generateLabel();
            std::string endLabel = generateLabel();
            // افتراضيًا النتيجة 0
            emit(InstructionType::LOAD, resultTemp, "0");
            emit(InstructionType::CMP, leftTemp, rightTemp);

            // قفزة إلى trueLabel عند تحقق الشرط
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
            throw std::runtime_error("معامل غير مدعوم في التعبير");
        }

        return resultTemp;
    }
    else if (auto unaryOp = dynamic_cast<UnaryOpNode *>(expr))
    {
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
            throw std::runtime_error("معامل أحادي غير مدعوم");
        }

        return resultTemp;
    }
    else if (auto literal = dynamic_cast<LiteralNode *>(expr))
    {
        std::string temp = generateTempVar();
        // إذا كانت قيمة نصية، نحفظها كوسم سلسلة، وإلا قيمة مباشرة
        if (literal->type == TokenType::STRING_LITERAL)
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

    throw std::runtime_error("نوع تعبير غير مدعوم");
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
        // تبسيط: لا نهرب السلاسل بالكامل هنا
        for (char ch : stringLiterals[i])
        {
            if (ch == '"')
                file << "\\\"";
            else if (ch == '\\')
                file << "\\\\";
            else if (ch == '\n')
                file << "\\n";
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
                                  ((instr.operand2[0] == '-' || instr.operand2[0] == '+') && instr.operand2.size() > 1));
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
            // operand1 يحدد النوع: VALUE أو STRING
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
            // خزن أسماء المتغيرات للمقارنة لاحقًا
            lastCmpLeft = instr.operand1;
            lastCmpRight = instr.operand2;
            file << "# CMP " << instr.operand1 << ", " << instr.operand2 << std::endl;
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
            file << "    bgt $t1, $t2, " << instr.operand1 << std::endl; // مافي MIPS قياسي لكن اغلب المجمعات تدعم
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

    file << "int main() {" << std::endl;

    // تعريف المتغيرات
    for (const auto &symbol : symbolTable)
    {
        file << "    int " << symbol.first << " = 0;" << std::endl;
    }

    // تعريف المتغيرات المؤقتة
    for (int i = 0; i < tempVarCounter; i++)
    {
        file << "    int t" << i << " = 0;" << std::endl;
    }

    file << std::endl;

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
                file << "printf(\"%s\", " << instr.operand2 << ");";
            }
            else
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
            // لا شيء مباشر في C، يعتمد على قفزات لاحقة
            file << "/* CMP " << instr.operand1 << ", " << instr.operand2 << " */";
            break;
        case InstructionType::JE:
            file << "if (" << instr.operand1 << ") goto " << instr.operand1 << ";"; // سيتم استبداله أثناء البناء المنطقي
            break;
        case InstructionType::JNE:
            file << "/* JNE label */";
            break;
        case InstructionType::JG:
            file << "/* JG label */";
            break;
        case InstructionType::JL:
            file << "/* JL label */";
            break;
        case InstructionType::JGE:
            file << "/* JGE label */";
            break;
        case InstructionType::JLE:
            file << "/* JLE label */";
            break;
        case InstructionType::LABEL:
            file << instr.operand1 << ":";
            break;
        case InstructionType::HALT:
            file << "return 0;";
            break;
        default:
            file << "// " << instr.toString();
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

    file.close();
    std::cout << "✅ تم توليد الكود الوسيط في: " << filename << std::endl;
}

void Compiler::displayInstructions() const
{
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
}