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
    CfgBlock* crnt_block;

    u32 sync_point{invalid<u32>()};
    nullable<PredCond> pred_cond;

    CfgBlock* GetBranchTarget(u32 label, u32 return_sync_point) {
        auto block = cfg.GetBlock(label);
        block->return_sync_point = return_sync_point;
        return block;
    }

    void EndBlock(const CfgBlockEdge& edge) {
        cfg.FinishBlock(crnt_block, pc + 1, edge);
        crnt_block = nullptr;
    }
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Analyzer
