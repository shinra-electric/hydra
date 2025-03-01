#pragma once

#include "horizon/services/nvdrv/ioctl/fd_base.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

// From switchbrew
struct GpuCharacteristics {
    u32 arch;
    u32 impl;
    u32 rev;
    u32 num_gpc;
    u64 l2_cache_size;
    u64 on_board_video_memory_size;
    u32 num_tpc_per_gpc;
    u32 bus_type;
    u32 big_page_size;
    u32 compression_page_size;
    u32 pde_coverage_bit_count;
    u32 available_big_page_sizes;
    u32 gpc_mask;
    u32 sm_arch_sm_version;
    u32 sm_arch_spa_version;
    u32 sm_arch_warp_count;
    u32 gpu_va_bit_count;
    u32 reserved;
    u64 flags;
    u32 twod_class;
    u32 threed_class;
    u32 compute_class;
    u32 gpfifo_class;
    u32 inline_to_memory_class;
    u32 dma_copy_class;
    u32 max_fbps_count;
    u32 fbp_en_mask;
    u32 max_ltc_per_fbp;
    u32 max_lts_per_ltc;
    u32 max_tex_per_tpc;
    u32 max_gpc_count;
    u32 rop_l2_en_mask_0;
    u32 rop_l2_en_mask_1;
    u64 chipname;
    u64 gr_compbit_store_base_hw;
};

struct ZCullInfo {
    u32 width_align_pixels;
    u32 height_align_pixels;
    u32 pixel_squares_by_aliquots;
    u32 aliquot_total;
    u32 region_byte_multiplier;
    u32 region_header_size;
    u32 subregion_header_size;
    u32 subregion_width_align_pixels;
    u32 subregion_height_align_pixels;
    u32 subregion_count;
};

class NvHostCtrlGpu : public FdBase {
  public:
    void Ioctl(IOCTL_PARAMS) override;

  private:
    // Ioctls
    DECLARE_IOCTL(ZCullGetCtxSize, writeonly<u32> size, size)
    DECLARE_IOCTL(ZCullGetInfo, writeonly<ZCullInfo> info, info)
    DECLARE_IOCTL(GetCharacteristics, readwrite<u64> buf_size;
                  readonly<uptr> buf_addr;
                  writeonly<GpuCharacteristics> characteristics;
                  , buf_size, characteristics)
};

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
