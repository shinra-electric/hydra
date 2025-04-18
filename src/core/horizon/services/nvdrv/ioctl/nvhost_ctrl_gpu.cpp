#include "core/horizon/services/nvdrv/ioctl/nvhost_ctrl_gpu.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

DEFINE_IOCTL_TABLE(NvHostCtrlGpu,
                   DEFINE_IOCTL_TABLE_ENTRY(0x47, 0x01, ZCullGetCtxSize, 0x02,
                                            ZCullGetInfo, 0x05,
                                            GetCharacteristics, 0x06,
                                            GetTPCMasks, 0x14,
                                            ZbcGetActiveSlotMask))

void NvHostCtrlGpu::QueryEvent(u32 event_id_u32, handle_id_t& out_handle_id,
                               NvResult& out_result) {
    LOG_FUNC_STUBBED(HorizonServices);
}

void NvHostCtrlGpu::ZCullGetCtxSize(ZCullGetCtxSizeData& data,
                                    NvResult& result) {
    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    data.size = 0x20000;
}

void NvHostCtrlGpu::ZCullGetInfo(ZCullGetInfoData& data, NvResult& result) {
    LOG_FUNC_STUBBED(HorizonServices);

    // TODO: what should this be?
    data.info = ZCullInfo{};
}

void NvHostCtrlGpu::GetCharacteristics(GetCharacteristicsData& data,
                                       NvResult& result) {
    ASSERT_DEBUG(data.buf_size != 0x0, HorizonServices,
                 "Invalid buffer size 0x{:08x}", data.buf_size.Get());
    data.buf_size = 0xa0;

    ASSERT_DEBUG(data.buf_addr != 0x0, HorizonServices,
                 "Invalid buffer address 0x{:08x}", data.buf_addr.Get());

    // Write the characteristics
    data.characteristics = {
        .arch = 0x120, // NVGPU_GPU_ARCH_GM200
        .impl = 0xb,   // NVGPU_GPU_IMPL_GM20B (or 0xE NVGPU_GPU_IMPL_GM20B_B)
        .rev = 0xa1,   // Revision A1
        .num_gpc = 0x1,
        .l2_cache_size = 0x40000,
        .on_board_video_memory_size = 0x0, // not used
        .num_tpc_per_gpc = 0x2,
        .bus_type = 0x20, // NVGPU_GPU_BUS_TYPE_AXI
        .big_page_size = 0x20000,
        .compression_page_size = 0x20000,
        .pde_coverage_bit_count = 0x1b,
        .available_big_page_sizes = 0x30000,
        .gpc_mask = 0x1,
        .sm_arch_sm_version = 0x503,  // Maxwell Generation 5.0.3
        .sm_arch_spa_version = 0x503, // Maxwell Generation 5.0.3
        .sm_arch_warp_count = 0x80,
        .gpu_va_bit_count = 0x28,
        .reserved = 0x0,
        .flags = 0x55, // TODO: (HAS_SYNCPOINTS | SUPPORT_SPARSE_ALLOCS |
                       // SUPPORT_CYCLE_STATS | SUPPORT_CYCLE_STATS_SNAPSHOT)
        .twod_class = 0x902d,             // FERMI_TWOD_A
        .threed_class = 0xb197,           // MAXWELL_B
        .compute_class = 0xb1c0,          // MAXWELL_COMPUTE_B
        .gpfifo_class = 0xb06f,           // MAXWELL_CHANNEL_GPFIFO_A
        .inline_to_memory_class = 0xa140, // KEPLER_INLINE_TO_MEMORY_B
        .dma_copy_class = 0xb0b5,         // MAXWELL_DMA_COPY_A
        .max_fbps_count = 0x1,
        .fbp_en_mask = 0x0, // disabled
        .max_ltc_per_fbp = 0x2,
        .max_lts_per_ltc = 0x1,
        .max_tex_per_tpc = 0x0, // not supported
        .max_gpc_count = 0x1,
        .rop_l2_en_mask_0 = 0x21d70, // fuse_status_opt_rop_l2_fbp_r
        .rop_l2_en_mask_1 = 0x0,
        .chipname = 0x6230326D67,       // "gm20b"
        .gr_compbit_store_base_hw = 0x0 // not supported
    };
}

void NvHostCtrlGpu::GetTPCMasks(GetTPCMasksData& data, NvResult& result) {
    LOG_FUNC_STUBBED(HorizonServices);

    ASSERT_DEBUG(data.mask_buffer_size != 0x0, HorizonServices,
                 "Mask buffer size cannot be 0x0");

    // TODO: what should this be?
    data.mask_buffer = 0x1;
}

void NvHostCtrlGpu::ZbcGetActiveSlotMask(ZbcGetActiveSlotMaskData& data,
                                         NvResult& result) {
    LOG_FUNC_STUBBED(HorizonServices);

    data.slot = 0x07;
    data.mask = 0x0; // TODO
}

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
