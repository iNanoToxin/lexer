#include "func_def_node.h"
#include "ast/visitors/ast_visitor.h"

std::shared_ptr<FuncDefNode> FuncDefNode::create(const std::shared_ptr<AstNode>& p_Name, const std::shared_ptr<AstNode>& p_Body)
{
    std::shared_ptr<FuncDefNode> node = std::make_shared<FuncDefNode>();
    node->name = p_Name;
    node->body = p_Body;

    if (node->name != nullptr)
    {
        node->name->setParent(node);
    }
    if (node->body != nullptr)
    {
        node->body->setParent(node);
    }
    return node;
}

void FuncDefNode::accept(AstVisitor& p_Visitor)
{
    p_Visitor.visitNode(cast<FuncDefNode>());
}
