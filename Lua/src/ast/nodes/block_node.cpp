#include "block_node.h"
#include "ast/visitors/ast_visitor.h"

std::shared_ptr<BlockNode> BlockNode::create(const std::list<std::shared_ptr<AstNode>>& p_Statements)
{
    std::shared_ptr<BlockNode> node = std::make_shared<BlockNode>();
    node->statements = p_Statements;

    for (const std::shared_ptr<AstNode>& statement : node->statements)
    {
        if (statement != nullptr)
        {
            statement->setParent(node);
        }
    }
    return node;
}

void BlockNode::accept(AstVisitor& p_Visitor)
{
    p_Visitor.visitNode(cast<BlockNode>());
}
