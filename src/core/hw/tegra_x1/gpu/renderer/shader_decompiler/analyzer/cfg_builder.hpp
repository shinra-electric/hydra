#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/cfg.hpp"
#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/observer_base.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::analyzer {

typedef std::function<void(u32)> visit_callback_fn_t;
typedef std::function<void()> exit_callback_fn_t;

class CfgBuilder : public ObserverBase {
  public:
    CfgBuilder();

    void SetPredCond(const PredCond pred_cond_) override {
        pred_cond = pred_cond_;
    }
    void ClearPredCond() override { pred_cond = std::nullopt; }
    void BlockChanged() override;

    // Operations
    void OpSetSync(u32 target) override;
    void OpSync() override;
    void OpBranch(u32 target) override;
    void OpExit() override;

    // Debug
    void LogBlocks() const {
        for (const auto& [label, block] : blocks)
            block->Log();
    }

  private:
    visit_callback_fn_t visit_callback;
    exit_callback_fn_t exit_callback;
    std::queue<u32> to_visit_queue;

    std::map<u32, CfgBasicBlock*> blocks;
    CfgBasicBlock* entry_point_block; // TODO: entry block

    CfgBasicBlock* crnt_block;

    std::optional<PredCond> pred_cond;

    CfgBasicBlock* VisitBlock(u32 label) {
        auto& block = blocks[label];
        if (!block)
            block = new CfgBasicBlock{};

        ASSERT_DEBUG(block->status == CfgBlockStatus::Unvisited,
                     ShaderDecompiler, "Block 0x{:x} already visited", label);
        block->status = CfgBlockStatus::Visited;
        block->code_range = range(label);

        return block;
    }

    void FinishBlock(CfgBasicBlock* block, const u32 end,
                     const CfgBlockEdge& edge) {
        ASSERT_DEBUG(block, ShaderDecompiler, "Invalid block");
        ASSERT_DEBUG(block->status == CfgBlockStatus::Visited, ShaderDecompiler,
                     "Block 0x{:x} finished without being visited",
                     block->code_range.begin);

        block->code_range.end = end;
        block->edge = edge;
        block->status = CfgBlockStatus::Finished;
    }

    void EndBlock(const CfgBlockEdge& edge) {
        FinishBlock(crnt_block, pc, edge);
        crnt_block = nullptr;

        if (!to_visit_queue.empty()) {
            visit_callback(to_visit_queue.front());
            to_visit_queue.pop();
        } else {
            exit_callback();
        }
    }

    void SetReturnSyncPoint(u32 label, u32 return_sync_point) {
        auto& block = blocks[label];
        if (!block) {
            block = new CfgBasicBlock{};
            to_visit_queue.push(label);
        }

        SetReturnSyncPointImpl(block, label, return_sync_point);
    }

    CfgBasicBlock* GetBranchTarget(u32 label,
                                   u32 return_sync_point = invalid<u32>()) {
        auto& block = blocks[label];
        if (!block) {
            block = new CfgBasicBlock{};
            to_visit_queue.push(label);
        }

        if (return_sync_point != invalid<u32>())
            SetReturnSyncPointImpl(block, label, return_sync_point);

        return block;
    }

    void SetReturnSyncPointImpl(CfgBasicBlock* block, u32 label,
                                u32 return_sync_point) {
        if (block->return_sync_point == invalid<u32>()) {
            block->return_sync_point = return_sync_point;
        } else {
            ASSERT_DEBUG(block->return_sync_point == return_sync_point,
                         ShaderDecompiler,
                         "Sync points for block {} don't match ({} != {})",
                         label, block->return_sync_point, return_sync_point);
        }
    }

  public:
    SETTER(visit_callback, SetVisitCallback);
    SETTER(exit_callback, SetExitCallback);
    GETTER(entry_point_block, GetEntryBlock);
    CONST_REF_GETTER(blocks, GetBlocks);
};

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::analyzer
