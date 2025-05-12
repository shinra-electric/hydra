#include "core/horizon/services/nvdrv/ioctl/nvhost_as_gpu.hpp"

#include "core/hw/tegra_x1/gpu/gpu.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

DEFINE_IOCTL_TABLE(NvHostAsGpu,
                   DEFINE_IOCTL_TABLE_ENTRY(NvHostAsGpu, 0x41, 0x01, BindChannel, 0x02,
                                            AllocSpace, 0x05, UnmapBuffer, 0x06,
                                            MapBufferEX, 0x08, GetVaRegions,
                                            0x09, AllocAsEX))

NvResult NvHostAsGpu::BindChannel(u32 fd_id) {
    LOG_FUNC_STUBBED(HorizonServices);
    return NvResult::Success;
}

NvResult NvHostAsGpu::AllocSpace(u32 pages, u32 page_size, aligned<AllocSpaceFlags, 8> flags, InOut<u64, gpu_vaddr_t> align_and_offset) {
    uptr gpu_addr = invalid<uptr>();
    if (any(flags & AllocSpaceFlags::FixedOffset))
        gpu_addr = align_and_offset; // TODO: is it really align?

    align_and_offset =
        HW::TegraX1::GPU::GPU::GetInstance().AllocatePrivateAddressSpace(
            static_cast<usize>(pages) * static_cast<usize>(page_size),
            gpu_addr);
    return NvResult::Success;
}

NvResult NvHostAsGpu::UnmapBuffer(gpu_vaddr_t addr) {
    auto& gpu = HW::TegraX1::GPU::GPU::GetInstance();

    LOG_FUNC_STUBBED(HorizonServices);
    return NvResult::Success;
}

NvResult NvHostAsGpu::MapBufferEX(MapBufferFlags flags, HW::TegraX1::GPU::NvKind kind, handle_id_t nvmap_handle_id, u32 reserved, u64 buffer_offset, u64 mapping_size, InOutSingle<gpu_vaddr_t> inout_addr) {
    auto& gpu = HW::TegraX1::GPU::GPU::GetInstance();

    if (any(flags & MapBufferFlags::Modify)) {
        LOG_NOT_IMPLEMENTED(
            HorizonServices,
            "Address space modifying (address: 0x{:08x}, size: 0x{:08x})",
            *inout_addr.data, mapping_size);
        return NvResult::BadParameter;
    }

    const auto& map = gpu.GetMap(nvmap_handle_id);

    usize size = mapping_size;
    if (size == 0x0)
        size = map.size; // TODO: correct?

    gpu_vaddr_t addr = invalid<uptr>();
    if (any(flags & MapBufferFlags::FixedOffset))
        addr = inout_addr;

    inout_addr =
        gpu.MapBufferToAddressSpace(map.addr + buffer_offset, size, addr);
    return NvResult::Success;
}

NvResult NvHostAsGpu::GetVaRegions(gpu_vaddr_t buffer_addr, InOutSingle<u32> inout_buffer_size, u32 reserved, std::array<VaRegion, 2>* out_va_regions) {
    LOG_FUNC_STUBBED(HorizonServices);

    inout_buffer_size = 2 * sizeof(VaRegion);
    // HACK
    (*out_va_regions)[0] =
        VaRegion{.addr = 0x0, .page_size = HW::TegraX1::GPU::PAGE_SIZE};
    (*out_va_regions)[1] = (*out_va_regions)[0];
    return NvResult::Success;
}

NvResult NvHostAsGpu::AllocAsEX(u32 big_page_size, i32 as_fd, u32 flags, u32 reserved, u64 va_range_start, u64 va_range_end, u64 va_range_split) {
    LOG_DEBUG(HorizonServices,
              "Start: 0x{:08x}, end: 0x{:08x}, split: 0x{:08x}",
              va_range_start, va_range_end, va_range_split);

    // TODO: why does nouveau pass 0x0 for all of these?

    // TODO: what is split for?
    HW::TegraX1::GPU::GPU::GetInstance().AllocatePrivateAddressSpace(
        va_range_end - va_range_start, va_range_start);
    return NvResult::Success;
}

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
