#ifndef REPEAT_STAT_NODE_H
#define REPEAT_STAT_NODE_H

#include <utility>

#include "expression_node.h"

class RepeatStatNode final : public ExpressionNode
{
public:
    std::shared_ptr<ExpressionNode> block;
    std::shared_ptr<ExpressionNode> condition;

    explicit RepeatStatNode() : ExpressionNode(AstKind::RepeatStatNode) {}

    static std::shared_ptr<RepeatStatNode> create(std::shared_ptr<ExpressionNode> p_Block, std::shared_ptr<ExpressionNode> p_Condition)
    {
        std::shared_ptr<RepeatStatNode> node = std::make_shared<RepeatStatNode>();
        node->block = std::move(p_Block);
        node->condition = std::move(p_Condition);

        if (node->block != nullptr)
        {
            node->block->parent = node;
        }
        if (node->condition != nullptr)
        {
            node->condition->parent = node;
        }
        return node;
    }
    static std::shared_ptr<RepeatStatNode> cast(const std::shared_ptr<AstNode>& p_Node)
    {
        return std::dynamic_pointer_cast<RepeatStatNode>(p_Node);
    }
    void accept(AstVisitor& p_Visitor) override;
};

#endif //REPEAT_STAT_NODE_H
