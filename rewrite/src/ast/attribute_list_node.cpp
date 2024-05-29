#include "attribute_list_node.h"
#include "visitor/ast_visitor.h"

void AttributeListNode::accept(AstVisitor* p_Visitor) {
    p_Visitor->visit(this);
}

void AttributeListNode::destroy()
{
    for (ExpressionNode* node : list)
    {
        node->destroy();
    }
    delete this;
}