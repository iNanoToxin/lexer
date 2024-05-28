#ifndef BINARYOPNODE_H
#define BINARYOPNODE_H

#include "expression_node.h"

class BinaryOpNode : public ExpressionNode {
public:
    BinaryOpNode(ExpressionNodePtr left, char op, ExpressionNodePtr right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    void accept(ASTVisitor& visitor) override;

    ExpressionNodePtr left;
    char op;
    ExpressionNodePtr right;
};

#endif