#ifndef BREAK_STAT_H
#define BREAK_STAT_H

#include "expression_node.h"

class BreakStat final : public ExpressionNode
{
public:

    BreakStat() = default;

    void accept(AstVisitor* p_Visitor) override;
};

#endif //BREAK_STAT_H