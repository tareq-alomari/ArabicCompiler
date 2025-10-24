#ifndef AST_H
#define AST_H

#include <string>
#include <memory>
#include <vector>
#include "Lexer.h"

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
    BINARY_OP,
    UNARY_OP,
    LITERAL,
    VARIABLE,
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

    VariableDeclarationNode(const std::string &n)
        : name(n) { type = NodeType::VARIABLE_DECL; }

    std::string toString() const override
    {
        std::string result = "تعريف متغير: " + name;
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
    std::string variableName;
    std::unique_ptr<ASTNode> value;

    AssignmentNode(const std::string &var) : variableName(var)
    {
        type = NodeType::ASSIGNMENT;
    }

    std::string toString() const override
    {
        return "تعيين: " + variableName + " = [تعبير]";
    }

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

    LiteralNode(TokenType type, const std::string &val)
        : literalType(type), value(val)
    {
        // التصحيح: تعيين نوع العقدة الصحيح
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

    std::string toString() const override
    {
        return "متغير: " + name;
    }

    std::string getTypeName() const override
    {
        return "VariableNode";
    }
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
            std::cout << newPrefix << "├── then:" << std::endl;
            for (size_t i = 0; i < ifStmt->thenBranch.size(); i++)
            {
                bool lastThen = (i == ifStmt->thenBranch.size() - 1) && ifStmt->elseBranch.empty();
                print(ifStmt->thenBranch[i], newPrefix + "│   ", lastThen);
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
            std::cout << newPrefix << "└── body:" << std::endl;
            for (size_t i = 0; i < whileStmt->body.size(); i++)
            {
                bool lastBody = (i == whileStmt->body.size() - 1);
                print(whileStmt->body[i], newPrefix + "    ", lastBody);
            }
        }
        else if (auto repeatStmt = dynamic_cast<RepeatNode *>(node.get()))
        {
            // عرض جسم الحلقة
            std::cout << newPrefix << "├── body:" << std::endl;
            for (size_t i = 0; i < repeatStmt->body.size(); i++)
            {
                bool lastBody = (i == repeatStmt->body.size() - 1);
                print(repeatStmt->body[i], newPrefix + "│   ", lastBody);
            }
            print(repeatStmt->condition, newPrefix, true);
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
    }
};

#endif