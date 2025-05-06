#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/analyzer/cfg.hpp"
#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/observer_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Analyzer {

class CfgBuilder : public ObserverBase {
  public:
    CfgBuilder();

    void SetPredCond(const PredCond pred_cond_) override {
        pred_cond = pred_cond_;
    }
    void ClearPredCond() override { pred_cond = nullptr; }
    void BlockChanged() override;

    // Operations
    void OpSetSync(u32 target) override;
    void OpSync() override;
    void OpBranch(u32 target) override;
    void OpExit() override;

    // Getters
    const CFG& GetCFG() const { return cfg; }

  private:
    CFG cfg;
    CfgNode* crnt_node;

    u32 sync_point{invalid<u32>()};
    nullable<PredCond> pred_cond;

    CfgNode* GetBranchTarget(u32 label, u32 return_sync_point) {
        auto& node = cfg.GetNode(label);
        node.return_sync_point = return_sync_point;
        return &node;
    }

    void EndNode(const CfgNodeEdge& edge) {
        cfg.FinishNode(*crnt_node, pc + 1, edge);
        crnt_node = nullptr;
    }
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Analyzer
