#pragma once
#include "ast_node.h"

class NumericForStatNode final : public AstNode
{
public:
    static constexpr AstKind ClassKind = AstKind::NumericForStatNode;

public:
    std::shared_ptr<AstNode> name;
    std::shared_ptr<AstNode> init;
    std::shared_ptr<AstNode> goal;
    std::shared_ptr<AstNode> step;
    std::shared_ptr<AstNode> block;

    explicit NumericForStatNode() : AstNode(AstKind::NumericForStatNode) {}

    static std::shared_ptr<NumericForStatNode> create(const std::shared_ptr<AstNode>& p_Name, const std::shared_ptr<AstNode>& p_Init, const std::shared_ptr<AstNode>& p_Goal, const std::shared_ptr<AstNode>& p_Step, const std::shared_ptr<AstNode>& p_Block);
    void accept(AstVisitor& p_Visitor) override;
};
