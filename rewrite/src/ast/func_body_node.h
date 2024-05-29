#ifndef FUNC_BODY_NODE_H
#define FUNC_BODY_NODE_H

#include "expression_node.h"

class FuncBodyNode final : public ExpressionNode
{
public:
    ExpressionNode* ptr1;
    ExpressionNode* ptr2;

    FuncBodyNode(ExpressionNode* p_Ptr1, ExpressionNode* p_Ptr2) : ptr1(p_Ptr1), ptr2(p_Ptr2) {}

    void accept(AstVisitor* p_Visitor) override;
    void destroy() override;
};

#endif //FUNC_BODY_NODE_H
