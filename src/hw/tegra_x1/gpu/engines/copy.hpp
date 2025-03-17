#pragma once

#include "hw/tegra_x1/gpu/engines/engine_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Engines {

enum class TransferType {
    None,
    Pipelined,
    NonPipelined,
};

enum class SemaphoreType {
    None,
    ReleaseOneWord,
    ReleaseFourWords,
};

enum class InterruptType {
    None,
    Blocking,
    NonBlocking,
};

enum class MemoryLayout {
    BlockLinear,
    Pitch,
};

enum class SemaphoreReduction {
    Imin,
    Imax,
    Ixor,
    Iand,
    Ior,
    Iadd,
    Inc,
    Dec,
    Fadd = 10,
};

enum class BypassL2 {
    UsePteSetting,
    ForceVolatile,
};

union RegsCopy {
    struct {
        u32 padding_0x0[0xc0];

        u32 not_a_register_0xc0;

        u32 padding_0xc1[0x3f];

        // 0x100
        u32 offset_in_upper;
        u32 offset_in_lower;
        u32 offset_out_upper;
        u32 offset_out_lower;
        u32 pitch_in;
        u32 pitch_out;
        u32 line_length_in;
        u32 line_count;

        u32 padding_0x108[0xb8];

        // 0x1c0
        u32 remap_const_hi;
        u32 remap_const_lo;
        u32 remap_components;
        u32 dst_block_size;
        u32 dst_width;
        u32 dst_height;
        u32 dst_depth;
        u32 dst_layer;
        u32 dst_origin;

        u32 padding_0x1c9;

        // 0x1ca
        u32 src_block_size;
        u32 src_width;
        u32 src_height;
        u32 src_depth;
        u32 src_layer;
        u32 src_origin;

        // TODO
    };

    u32 raw[0x446];
};

class Copy : public EngineBase {
  public:
    void Method(u32 method, u32 arg) override;

  protected:
    void WriteReg(u32 reg, u32 value) override {
        LOG_DEBUG(Engines, "Writing to copy reg 0x{:08x} (value: 0x{:08x})",
                  reg, value);
        regs.raw[reg] = value;
    }

  private:
    RegsCopy regs;

    // Commands
    struct LaunchDMAData {
        TransferType type : 2;
        bool flush_enable : 1;
        SemaphoreType semaphore_type : 2;
        InterruptType interrupt_type : 2;
        MemoryLayout src_memory_layout : 1;
        MemoryLayout dst_memory_layout : 1;
        bool multi_line_enable : 1;
        bool force_rmw_disable : 1;
        bool src_memory_is_physical : 1;
        bool dst_memory_is_physical : 1;
        SemaphoreReduction semaphore_reduction : 4;
        bool semaphore_reduction_unsigned : 1;
        bool semaphore_reduction_enable : 1;
        BypassL2 bypass_l2 : 1;
    };

    void LaunchDMA(const u32 index, const LaunchDMAData data);
};

} // namespace Hydra::HW::TegraX1::GPU::Engines
