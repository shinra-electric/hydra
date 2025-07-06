#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/const.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir {
class Builder;
}

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder {

enum class BlockStatus {
    Unvisited,
    Visited,
};

struct Block {
    BlockStatus status{BlockStatus::Unvisited};
    label_t return_sync_point;
};

struct DecoderContext {
    const DecompilerContext& decomp_context;
    Reader code_reader;
    ir::Builder& builder;
};

class Decoder {
  public:
    Decoder(const DecoderContext& context_) : context{context_} {}

    void Decode();

  private:
    DecoderContext context;

    std::queue<label_t> to_visit_queue;
    std::map<label_t, Block> blocks;
    Block* crnt_block{nullptr};

    void ParseNextInstruction();

    // Helpers
    void Jump(u32 target) {
        context.code_reader.Seek(target * sizeof(instruction_t));
    }
    u32 GetPC() const {
        return context.code_reader.Tell() / sizeof(instruction_t);
    }

    void EndBlock() {
        crnt_block->status = BlockStatus::Visited;
        crnt_block = nullptr;
        while (!to_visit_queue.empty()) {
            crnt_block = &blocks[to_visit_queue.front()];
            to_visit_queue.pop();
            if (crnt_block->status == BlockStatus::Unvisited)
                break;
        }
    }

    void SetReturnSyncPoint(label_t label,
                            label_t return_sync_point = invalid<u32>()) {
        auto& block = blocks[label];
        if (block.status == BlockStatus::Unvisited)
            to_visit_queue.push(label);

        if (return_sync_point != invalid<u32>()) {
            if (block.return_sync_point == invalid<u32>()) {
                block.return_sync_point = return_sync_point;
            } else {
                ASSERT_DEBUG(block.return_sync_point == return_sync_point,
                             ShaderDecompiler,
                             "Sync points for block {} don't match ({} != {})",
                             label, block.return_sync_point, return_sync_point);
            }
        }
    }
};

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder
