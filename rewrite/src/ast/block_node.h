#ifndef BLOCK_NODE_H
#define BLOCK_NODE_H

#include <utility>
#include <vector>
#include "expression_node.h"

class BlockNode final : public ExpressionNode
{
public:
    std::vector<ExpressionNode*> statements;

    explicit BlockNode(std::vector<ExpressionNode*> p_Statements) : statements(std::move(p_Statements)) {}

    void accept(AstVisitor* p_Visitor) override;
    void destroy() override;
};

#endif //BLOCK_NODE_H