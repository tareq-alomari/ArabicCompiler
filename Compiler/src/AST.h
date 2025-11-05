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