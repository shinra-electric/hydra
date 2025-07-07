#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/analyzer/structurizer.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::analyzer {

namespace {

// Returns merge block
CfgBasicBlock* TransformIfElse(CfgBasicBlock* block) {
    auto block_true = block->edge.branch_conditional.target_true;
    auto block_false = block->edge.branch_conditional.target_false;

    // Find the merge block
    CfgBasicBlock* merge_block = block_true->FindMergeBlock(block_false);

    if (!merge_block) {
        LOG_ERROR(ShaderDecompiler,
                  "Failed to find merge block for if-else block {}",
                  block->label);
        return nullptr;
    }

    // Replace all jumps to the target with a continue statement and
    // jumps to merge block with a break statement
    block->Walk([=](CfgBasicBlock* b) {
        switch (b->edge.type) {
        case CfgBlockEdgeType::Branch:
            if (b->edge.branch.target->IsSameAs(merge_block))
                b->edge.type = CfgBlockEdgeType::None;
            break;
        case CfgBlockEdgeType::BranchConditional:
            if (b->edge.branch_conditional.target_true->IsSameAs(merge_block)) {
                b->edge.branch_conditional.target_true = new CfgBasicBlock{
                    .edge =
                        {
                            .type = CfgBlockEdgeType::None,
                        },
                };
            }
            if (b->edge.branch_conditional.target_false->IsSameAs(
                    merge_block)) {
                b->edge.branch_conditional.target_false = new CfgBasicBlock{
                    .edge =
                        {
                            .type = CfgBlockEdgeType::None,
                        },
                };
            }
            break;
        default:
            break;
        }
        if (b == merge_block)
            b->edge.type = CfgBlockEdgeType::None;

        return true;
    });

    return merge_block;
}

// Returns merge block
CfgBasicBlock* TransformLoop(const CfgBasicBlock* entry_block,
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
        LOG_ERROR(ShaderDecompiler,
                  "Failed to find merge block for loop block {}",
                  entry_block->label);
        return nullptr;
    }

    // Replace all jumps to the target with a continue statement and
    // jumps to merge block with a break statement
    looping_block->Walk([entry_block, merge_block](CfgBasicBlock* b) {
        if (b->CanDirectlyJumpTo(entry_block))
            b->edge.type = CfgBlockEdgeType::Continue;
        else if (b->CanDirectlyJumpTo(merge_block))
            b->edge.type = CfgBlockEdgeType::Break;

        return true;
    });

    return merge_block;
}

CfgBlock* ResolveBlock(const CfgBasicBlock* block);

CfgNode* ResolveBlockImpl(const CfgBasicBlock* block) {
    switch (block->edge.type) {
    case CfgBlockEdgeType::None:
        return new CfgCodeBlock(block->label);
    case CfgBlockEdgeType::Branch: {
        auto target = block->edge.branch.target;
        bool is_loop = false;
        target->Walk([&is_loop, block](CfgBasicBlock* b) {
            if (b == block) {
                is_loop = true;
                return false;
            }

            return true;
        });

        if (is_loop) {
            LOG_DEBUG(ShaderDecompiler, "Do-while loop detected (block: {})",
                      block->label);

            // Do-while loop

            // Clone target to be able to modify it
            target = target->Clone();

            // Find the merge block
            auto merge_block = TransformLoop(block, target);

            // Structurize
            auto code_block = new CfgCodeBlock(block->label);
            auto target_resolved = ResolveBlock(target);
            auto while_block =
                new CfgWhileBlock(true, block->edge.branch_conditional.cond,
                                  new CfgBlock({code_block, target_resolved}));

            return new CfgStructuredNodeWithEdge(while_block,
                                                 ResolveBlock(merge_block));
        }

        return new CfgStructuredNodeWithEdge(
            new CfgCodeBlock(block->label),
            ResolveBlock(block->edge.branch.target));
    }
    case CfgBlockEdgeType::BranchConditional: {
        const auto& cond = block->edge.branch_conditional.cond;
        auto block_true = block->edge.branch_conditional.target_true;
        auto block_false = block->edge.branch_conditional.target_false;

        auto code_block = new CfgCodeBlock(block->label);

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
            block_false->Walk([&is_loop, /*&pred_cond, */ &block_true,
                               &block_false, block](CfgBasicBlock* b) {
                if (b == block) {
                    is_loop = true;

                    // TODO: implement
                    LOG_FATAL(ShaderDecompiler, "Else block loop");
                    // Flip the condition and swap the target blocks
                    // pred_cond.not_ = !pred_cond.not_;
                    // std::swap(block_true, block_false);

                    return false;
                }

                return true;
            });
        }

        if (is_loop) {
            LOG_DEBUG(ShaderDecompiler, "While loop detected (block: {})",
                      block->label);

            // While loop

            // Clone block_true to be able to modify it
            block_true = block_true->Clone();

            // Find the merge block
            auto merge_block = TransformLoop(block, block_true);

            auto resolved_block_true = ResolveBlock(block_true);
            auto resolved_block_false = ResolveBlock(block_false);
            if (!resolved_block_true || !resolved_block_false) {
                LOG_ERROR(ShaderDecompiler,
                          "Failed to resolve branch conditional (block: {})",
                          block->label);
                return nullptr;
            }

            // We emit this weird if-else and do-while combo to deal with the
            // fact that we cannot have a proper prologue (so the prologue is
            // emitted twice: once before the if-else and once at the end of the
            // do-while loop)
            // TODO: this is simply not correct, as continue statements won't
            // jump to the prologue
            auto code_block = new CfgCodeBlock(block->label);
            auto while_block = new CfgWhileBlock(
                true, block->edge.branch_conditional.cond,
                new CfgBlock({resolved_block_true, code_block}));
            auto if_else_block =
                new CfgIfElseBlock(block->edge.branch_conditional.cond,
                                   while_block, resolved_block_false);

            auto base_block = new CfgBlock({code_block, if_else_block});
            return new CfgStructuredNodeWithEdge(base_block,
                                                 ResolveBlock(merge_block));
        }

        // Clone block to be able to modify it
        auto block_cloned = block->Clone();

        // Find the merge block
        auto merge_block = TransformIfElse(block_cloned);

        auto resolved_block_true =
            ResolveBlock(block_cloned->edge.branch_conditional.target_true);
        auto resolved_block_false =
            ResolveBlock(block_cloned->edge.branch_conditional.target_false);
        if (!resolved_block_true || !resolved_block_false) {
            LOG_ERROR(ShaderDecompiler, "Failed to resolve branch conditional");
            return nullptr;
        }

        auto if_else_block =
            new CfgIfElseBlock(block_cloned->edge.branch_conditional.cond,
                               resolved_block_true, resolved_block_false);
        auto base_block = new CfgBlock({code_block, if_else_block});
        if (merge_block) {
            auto merge_node = ResolveBlock(merge_block);
            return new CfgStructuredNodeWithEdge(base_block, merge_node);
        } else {
            return base_block;
        }
    }
    case CfgBlockEdgeType::Exit:
        return new CfgCodeBlock(block->label, LastStatement::Exit);
    case CfgBlockEdgeType::Break:
        return new CfgCodeBlock(block->label, LastStatement::Break);
    case CfgBlockEdgeType::Continue:
        return new CfgCodeBlock(block->label, LastStatement::Continue);
    case CfgBlockEdgeType::Invalid:
        LOG_ERROR(ShaderDecompiler, "Invalid block {}", block->label);
        return nullptr;
    }

    return nullptr;
}

CfgBlock* ResolveBlock(const CfgBasicBlock* block) {
    auto resolved_block = ResolveBlockImpl(block);
    if (!resolved_block) {
        LOG_ERROR(ShaderDecompiler, "Failed to resolve block {}", block->label);
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

CfgBlock* Structurize(const CfgBasicBlock* entry_bb) {
    return ResolveBlock(entry_bb);
}

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::analyzer
