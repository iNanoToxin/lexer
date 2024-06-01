#include "numeric_for_stat_node.h"
#include "visitor/ast_visitor.h"

void NumericForStatNode::accept(AstVisitor& p_Visitor)
{
    p_Visitor.visit(cast(shared_from_this()));
}
