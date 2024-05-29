#ifndef TABLE_INDEX_VALUE_NODE_H
#define TABLE_INDEX_VALUE_NODE_H

#include "expression_node.h"

class TableIndexValueNode final : public ExpressionNode
{
public:
    ExpressionNode* index;
    ExpressionNode* value;

    TableIndexValueNode(ExpressionNode* p_Index, ExpressionNode* p_Value) : index(p_Index), value(p_Value) {}

    void accept(AstVisitor* p_Visitor) override;
    void destroy() override;
};

#endif //TABLE_INDEX_VALUE_NODE_H