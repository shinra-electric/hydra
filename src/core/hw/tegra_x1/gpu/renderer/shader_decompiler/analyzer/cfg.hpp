#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Analyzer {

struct CfgNodeBase {
    virtual ~CfgNodeBase() = default;

    // Debug
    virtual void Log() const = 0;
};

enum class CfgBlockEdgeType {
    Branch,
    BranchConditional,
    Exit,
};

struct CfgBlock;

struct CfgBlockEdge {
    CfgBlockEdgeType type;
    union {
        struct {
            CfgBlock* target;
        } branch;
        struct {
            PredCond pred_cond;
            CfgBlock* target_true;
            CfgBlock* target_false;
        } branch_conditional;
    };
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Analyzer

ENABLE_ENUM_FORMATTING(Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::
                           Analyzer::CfgBlockEdgeType,
                       Branch, "branch", BranchConditional,
                       "branch conditional", Exit, "exit")

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Analyzer {

enum class CfgBlockStatus {
    Unvisited,
    Visited,
    Finished,
};

struct CfgBlock : public CfgNodeBase {
    CfgBlockStatus status{CfgBlockStatus::Unvisited};
    range<u32> code_range;
    u32 return_sync_point{invalid<u32>()};
    CfgBlockEdge edge;

    void Log() const override {
        if (status == CfgBlockStatus::Unvisited) {
            LOG_DEBUG(ShaderDecompiler, "\tUNVISITED");
            return;
        } else if (status == CfgBlockStatus::Visited) {
            LOG_DEBUG(ShaderDecompiler, "\tVISITED");
            return;
        }

        LOG_DEBUG(ShaderDecompiler, "\tCode: {}", code_range);
        if (return_sync_point != invalid<u32>())
            LOG_DEBUG(ShaderDecompiler, "\tReturn Sync Point: {}",
                      return_sync_point);

        LOG_DEBUG(ShaderDecompiler, "\tEdge: {}", edge.type);
        switch (edge.type) {
        case CfgBlockEdgeType::Branch:
            LOG_DEBUG(ShaderDecompiler, "\t\tTarget: {}",
                      edge.branch.target->code_range.begin);
            break;
        case CfgBlockEdgeType::BranchConditional:
            LOG_DEBUG(ShaderDecompiler, "\t\tif {}p{}",
                      edge.branch_conditional.pred_cond.not_ ? "!" : "",
                      edge.branch_conditional.pred_cond.pred);
            LOG_DEBUG(ShaderDecompiler, "\t\tTarget True: {}",
                      edge.branch_conditional.target_true->code_range.begin);
            LOG_DEBUG(ShaderDecompiler, "\t\tTarget False: {}",
                      edge.branch_conditional.target_false->code_range.begin);
            break;
        case CfgBlockEdgeType::Exit:
            break;
        }
    }
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Analyzer
