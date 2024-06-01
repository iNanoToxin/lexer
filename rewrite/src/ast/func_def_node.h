#ifndef FUNC_DEF_NODE_H
#define FUNC_DEF_NODE_H

#include <utility>

#include "ast_node.h"

class FuncDefNode final : public AstNode
{
public:
    std::shared_ptr<AstNode> name;
    std::shared_ptr<AstNode> body;

    explicit FuncDefNode() : AstNode(AstKind::FuncDefNode) {}

    static std::shared_ptr<FuncDefNode> create(std::shared_ptr<AstNode> p_Name, std::shared_ptr<AstNode> p_Body)
    {
        std::shared_ptr<FuncDefNode> node = std::make_shared<FuncDefNode>();
        node->name = std::move(p_Name);
        node->body = std::move(p_Body);

        if (node->name != nullptr)
        {
            node->name->parent = node;
        }
        if (node->body != nullptr)
        {
            node->body->parent = node;
        }
        return node;
    }
    static std::shared_ptr<FuncDefNode> cast(const std::shared_ptr<AstNode>& p_Node)
    {
        return std::dynamic_pointer_cast<FuncDefNode>(p_Node);
    }
    void accept(AstVisitor& p_Visitor) override;
};

#endif //FUNC_DEF_NODE_H
