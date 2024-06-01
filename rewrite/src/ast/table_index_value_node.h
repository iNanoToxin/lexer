#ifndef TABLE_INDEX_VALUE_NODE_H
#define TABLE_INDEX_VALUE_NODE_H

#include <utility>

#include "ast_node.h"

class TableIndexValueNode final : public AstNode
{
public:
    std::shared_ptr<AstNode> index;
    std::shared_ptr<AstNode> value;

    explicit TableIndexValueNode() : AstNode(AstKind::TableIndexValueNode) {}

    static std::shared_ptr<TableIndexValueNode> create(std::shared_ptr<AstNode> p_Index, std::shared_ptr<AstNode> p_Value)
    {
        std::shared_ptr<TableIndexValueNode> node = std::make_shared<TableIndexValueNode>();
        node->index = std::move(p_Index);
        node->value = std::move(p_Value);

        if (node->index != nullptr)
        {
            node->index->parent = node;
        }
        if (node->value != nullptr)
        {
            node->value->parent = node;
        }
        return node;
    }
    static std::shared_ptr<TableIndexValueNode> cast(const std::shared_ptr<AstNode>& p_Node)
    {
        return std::dynamic_pointer_cast<TableIndexValueNode>(p_Node);
    }
    void accept(AstVisitor& p_Visitor) override;
};

#endif //TABLE_INDEX_VALUE_NODE_H
