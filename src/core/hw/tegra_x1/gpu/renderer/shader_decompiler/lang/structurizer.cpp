#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/lang/structurizer.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::Lang {

namespace {

// Returns merge block
CfgBasicBlock* TransformLoop(CfgBasicBlock* entry_block,
                             CfgBasicBlock* looping_block) {
    // Find the merge block
    // TODO: collect merge block candidates and find one that all of them can
    // jump to
    CfgBasicBlock* merge_block = nullptr;
    looping_block->Walk([&merge_block, entry_block](CfgBasicBlock* b) {
        if (b->CanJumpTo(entry_block)) {
            merge_block = b;
            return false;
        }

        return true;
    });

    if (!merge_block) {
        LOG_ERROR(ShaderDecompiler, "Failed to find merge block for loop");
        return nullptr;
    }

    // Replace all jumps to the target with a continue statement and
    // jumps to merge block with a break statement
    looping_block->Walk([entry_block, merge_block](CfgBasicBlock* b) {
        // TODO: create new block instead of modifying the existing one
        if (b->CanDirectlyJumpTo(entry_block))
            b->edge.type = CfgBlockEdgeType::Continue;
        else if (b->CanDirectlyJumpTo(merge_block))
            b->edge.type = CfgBlockEdgeType::Break;

        return true;
    });

    return merge_block;
}

CfgBlock* ResolveBlock(CfgBasicBlock* block);

CfgNode* ResolveBlockImpl(CfgBasicBlock* block) {
    switch (block->edge.type) {
    case CfgBlockEdgeType::Branch: {
        const auto target = block->edge.branch.target;
        bool is_loop = false;
        target->Walk([&is_loop, block](CfgBasicBlock* b) {
            if (b == block) {
                is_loop = true;
                return false;
            }

            return true;
        });

        if (is_loop) {
            LOG_INFO(ShaderDecompiler, "Do-while loop detected");

            // Do-while loop
            auto merge_block = TransformLoop(block, target);

            auto code_block = new CfgCodeBlock(block->code_range);
            auto target_resolved = ResolveBlock(target);
            auto while_block = new CfgWhileBlock(
                true, block->edge.branch_conditional.pred_cond,
                new CfgBlock({code_block, target_resolved}));

            return new CfgStructuredNodeWithEdge(while_block,
                                                 ResolveBlock(merge_block));
        }

        return new CfgStructuredNodeWithEdge(
            new CfgCodeBlock(block->code_range),
            ResolveBlock(block->edge.branch.target));
    }
    case CfgBlockEdgeType::BranchConditional: {
        auto pred_cond = block->edge.branch_conditional.pred_cond;
        auto block_true = block->edge.branch_conditional.target_true;
        auto block_false = block->edge.branch_conditional.target_false;

        auto code_block = new CfgCodeBlock(block->code_range);

        bool is_loop = false;
        block_true->Walk([&is_loop, block](CfgBasicBlock* b) {
            if (b == block) {
                is_loop = true;
                return false;
            }

            return true;
        });

        if (!is_loop) {
            // Check the false branch as well
            block_false->Walk([&is_loop, &pred_cond, &block_true, &block_false,
                               block](CfgBasicBlock* b) {
                if (b == block) {
                    is_loop = true;

                    // Flip the condition and swap the target blocks
                    pred_cond.not_ = !pred_cond.not_;
                    std::swap(block_true, block_false);

                    return false;
                }

                return true;
            });
        }

        if (is_loop) {
            LOG_INFO(ShaderDecompiler, "While loop detected");

            // While loop
            auto merge_block = TransformLoop(block, block_true);

            auto resolved_block_true = ResolveBlockImpl(block_true);
            auto resolved_block_false = ResolveBlockImpl(block_false);
            if (!resolved_block_true || !resolved_block_false) {
                LOG_ERROR(ShaderDecompiler,
                          "Failed to resolve branch conditional");
                return nullptr;
            }

            // We emit this weird if-else and do-while combo to deal with the
            // fact that we cannot have a proper prologue (so the prologue is
            // emitted twice: once before the if-else and once at the end of the
            // do-while loop)
            auto code_block = new CfgCodeBlock(block->code_range);
            auto while_block = new CfgWhileBlock(
                true, block->edge.branch_conditional.pred_cond,
                new CfgBlock({resolved_block_true, code_block}));
            auto if_else_block =
                new CfgIfElseBlock(block->edge.branch_conditional.pred_cond,
                                   while_block, resolved_block_false);

            auto base_block = new CfgBlock({code_block, if_else_block});
            return new CfgStructuredNodeWithEdge(base_block,
                                                 ResolveBlock(merge_block));
        }

        auto resolved_block_true = ResolveBlockImpl(block_true);
        auto resolved_block_false = ResolveBlockImpl(block_false);
        if (!resolved_block_true || !resolved_block_false) {
            LOG_ERROR(ShaderDecompiler, "Failed to resolve branch conditional");
            return nullptr;
        }

        CfgNode* node_true = resolved_block_true;
        CfgNode* node_false = resolved_block_false;
        CfgNode* merge_node = nullptr;
        if (auto block_true_with_edge =
                dynamic_cast<CfgStructuredNodeWithEdge*>(resolved_block_true)) {
            node_true = block_true_with_edge->node;
            merge_node = block_true_with_edge->branch_target;
        }

        if (auto block_false_with_edge =
                dynamic_cast<CfgStructuredNodeWithEdge*>(
                    resolved_block_false)) {
            node_false = block_false_with_edge->node;
            if (merge_node) {
                auto block_true_with_edge =
                    static_cast<CfgStructuredNodeWithEdge*>(
                        resolved_block_true);

                // TODO: the special cases need to be handled more generically

                // Special case when the true block branches to the false block
                if (merge_node->IsSameAs(resolved_block_false)) {
                    auto if_block =
                        new CfgIfBlock(block->edge.branch_conditional.pred_cond,
                                       block_true_with_edge->node);
                    auto base_block = new CfgBlock({code_block, if_block});
                    return new CfgStructuredNodeWithEdge(
                        base_block, block_false_with_edge->node);
                }

                // Special case when the false block branches to the true block
                if (block_false_with_edge->branch_target->IsSameAs(
                        resolved_block_true)) {
                    // Invert the condition
                    auto pred_cond = block->edge.branch_conditional.pred_cond;
                    pred_cond.not_ = !pred_cond.not_;

                    auto if_block =
                        new CfgIfBlock(pred_cond, block_false_with_edge->node);
                    auto base_block = new CfgBlock({code_block, if_block});
                    return new CfgStructuredNodeWithEdge(
                        base_block, block_true_with_edge->node);
                }

                // Verify that the branch targets match
                if (!block_false_with_edge->branch_target->IsSameAs(
                        merge_node)) {
                    LOG_ERROR(ShaderDecompiler, "Branch targets do not match");
                    resolved_block_true->Log();
                    resolved_block_false->Log();
                    // TODO: uncomment
                    // return nullptr;
                }
            } else {
                merge_node = block_false_with_edge->branch_target;
            }
        }

        auto if_else_block = new CfgIfElseBlock(
            block->edge.branch_conditional.pred_cond, node_true, node_false);
        auto base_block = new CfgBlock({code_block, if_else_block});
        if (merge_node)
            return new CfgStructuredNodeWithEdge(base_block, merge_node);
        else
            return base_block;
    }
    case CfgBlockEdgeType::Exit:
        return new CfgCodeBlock(block->code_range);
    case CfgBlockEdgeType::Break:
        return new CfgCodeBlock(block->code_range, LastStatement::Break);
    case CfgBlockEdgeType::Continue:
        return new CfgCodeBlock(block->code_range, LastStatement::Continue);
    }

    return nullptr;
}

CfgBlock* ResolveBlock(CfgBasicBlock* block) {
    auto resolved_block = ResolveBlockImpl(block);
    if (!resolved_block) {
        LOG_ERROR(ShaderDecompiler, "Failed to resolve block");
        return nullptr;
    }

    if (auto code_block = dynamic_cast<CfgCodeBlock*>(resolved_block)) {
        return new CfgBlock({code_block});
    } else if (auto block_with_edge =
                   dynamic_cast<CfgStructuredNodeWithEdge*>(resolved_block)) {
        return new CfgBlock(
            {block_with_edge->node, block_with_edge->branch_target});
    } else if (auto block = dynamic_cast<CfgBlock*>(resolved_block)) {
        return block;
    } else {
        LOG_ERROR(ShaderDecompiler, "Unknown resolved block type");
        return nullptr;
    }
}

} // namespace

CfgBlock* Structurize(CfgBasicBlock* entry_bb) {
    return ResolveBlock(entry_bb);
}

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::Lang
