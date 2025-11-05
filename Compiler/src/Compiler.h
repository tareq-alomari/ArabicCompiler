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