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
