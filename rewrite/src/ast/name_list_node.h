#ifndef NAME_LIST_NODE_H
#define NAME_LIST_NODE_H

#include <utility>
#include <vector>
#include "expression_node.h"

class NameListNode final : public ExpressionNode
{
public:
    std::vector<ExpressionNode*> list;

    explicit NameListNode(std::vector<ExpressionNode*> p_List) : list(std::move(p_List)) {}

    void accept(AstVisitor* p_Visitor) override;
    void destroy() override;
};

#endif //NAME_LIST_NODE_H