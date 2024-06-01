#ifndef EXPRESSION_LIST_NODE_H
#define EXPRESSION_LIST_NODE_H

#include <vector>
#include "ast_node.h"

class ExpressionListNode final : public AstNode
{
public:
    std::vector<std::shared_ptr<AstNode>> list;

    explicit ExpressionListNode() : AstNode(AstKind::ExpressionListNode) {}

    static std::shared_ptr<ExpressionListNode> create(std::vector<std::shared_ptr<AstNode>> p_List)
    {
        std::shared_ptr<ExpressionListNode> node = std::make_shared<ExpressionListNode>();
        node->list = std::move(p_List);

        for (const std::shared_ptr<AstNode>& child : node->list)
        {
            if (child != nullptr)
            {
                child->parent = node;
            }
        }
        return node;
    }
    static std::shared_ptr<ExpressionListNode> cast(const std::shared_ptr<AstNode>& p_Node)
    {
        return std::dynamic_pointer_cast<ExpressionListNode>(p_Node);
    }
    void accept(AstVisitor& p_Visitor) override;
};

#endif //EXPRESSION_LIST_NODE_H
