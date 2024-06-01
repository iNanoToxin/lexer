#include "unary_op_node.h"
#include "visitor/ast_visitor.h"

void UnaryOpNode::accept(AstVisitor& p_Visitor) {
    p_Visitor.visit(cast(shared_from_this()));
}
