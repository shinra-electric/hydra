#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/analyzer/cfg_builder.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Analyzer {

CfgBuilder::CfgBuilder() { crnt_node = &cfg.VisitNode(0); }

void CfgBuilder::BlockChanged() {
    ASSERT_DEBUG(!crnt_node, ShaderDecompiler,
                 "Starting a new block without finishing the previous one");
    crnt_node = &cfg.VisitNode(pc);
}

void CfgBuilder::OpSetSync(u32 target) { sync_point = target; }

void CfgBuilder::OpSync() {
    const auto target = crnt_node->return_sync_point;
    ASSERT_DEBUG(target != invalid<u32>(), ShaderDecompiler,
                 "Invalid sync point");

    CfgNodeEdge edge;
    edge.type = CfgNodeEdgeType::Branch;
    edge.branch.target = &cfg.GetNode(target);
    EndNode(edge);
}

void CfgBuilder::OpBranch(u32 target) {
    ASSERT_DEBUG(sync_point != invalid<u32>(), ShaderDecompiler,
                 "Invalid sync point");

    CfgNodeEdge edge;
    if (pred_cond) {
        edge.type = CfgNodeEdgeType::BranchConditional;
        edge.branch_conditional.target_true =
            GetBranchTarget(target, sync_point);
        edge.branch_conditional.target_false =
            GetBranchTarget(pc + 1, sync_point);
    } else {
        edge.type = CfgNodeEdgeType::Branch;
        edge.branch.target = GetBranchTarget(target, sync_point);
    }
    EndNode(edge);
}

void CfgBuilder::OpExit() {
    CfgNodeEdge edge;
    edge.type = CfgNodeEdgeType::Exit;
    EndNode(edge);
}

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Analyzer
