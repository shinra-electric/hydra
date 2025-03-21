#include "horizon/services/nvdrv/ioctl/nvhost_as_gpu.hpp"

#include "hw/tegra_x1/gpu/gpu.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

DEFINE_IOCTL_TABLE(NvHostAsGpu, DEFINE_IOCTL_TABLE_ENTRY(0x41, 0x02, AllocSpace,
                                                         0x06, MapBufferEx))

void NvHostAsGpu::AllocSpace(AllocSpaceData& data, NvResult& result) {
    uptr gpu_addr = invalid<uptr>();
    if (any(data.flags & AllocSpaceFlags::FixedOffset))
        gpu_addr = data.align_or_offset;

    data.offset =
        HW::TegraX1::GPU::GPU::GetInstance().AllocatePrivateAddressSpace(
            static_cast<usize>(data.pages) * static_cast<usize>(data.page_size),
            gpu_addr);
}

void NvHostAsGpu::MapBufferEx(MapBufferExData& data, NvResult& result) {
    auto& gpu = HW::TegraX1::GPU::GPU::GetInstance();

    if (any(data.flags & MapBufferFlags::Modify)) {
        LOG_WARNING(
            HorizonServices,
            "Address space modifying is not implemented (address: 0x{:08x})",
            data.offset.Get());
        return;
    }

    const auto& map = gpu.GetMap(data.nvmap_id);

    usize size = data.mapping_size;
    if (size == 0x0)
        size = map.size; // TODO: correct?

    uptr gpu_addr = invalid<uptr>();
    if (any(data.flags & MapBufferFlags::FixedOffset))
        gpu_addr = data.offset;

    data.offset = gpu.MapBufferToAddressSpace(map.addr + data.buffer_offset,
                                              size, gpu_addr);
    // LOG_DEBUG(HorizonServices, "OFFSET: 0x{:08x}", data.offset.Get());
}

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
