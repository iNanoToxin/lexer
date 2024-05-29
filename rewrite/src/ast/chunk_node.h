#ifndef CHUNK_NODE_H
#define CHUNK_NODE_H

#include "expression_node.h"

class ChunkNode final : public ExpressionNode
{
public:
    ExpressionNode* block;

    explicit ChunkNode(ExpressionNode* p_Block) : block(p_Block) {}

    void accept(AstVisitor* p_Visitor) override;
    void destroy() override;
};

#endif //CHUNK_NODE_H