#include "horizon/services/nvdrv/ioctl/nvhost_as_gpu.hpp"

#include "hw/tegra_x1/gpu/gpu.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

DEFINE_IOCTL_TABLE(NvHostAsGpu, DEFINE_IOCTL_TABLE_ENTRY(0x41, 0x02, AllocSpace,
                                                         0x06, MapBufferEx))

void NvHostAsGpu::AllocSpace(AllocSpaceData& data, NvResult& result) {
    data.offset =
        HW::TegraX1::GPU::GPU::GetInstance().AllocatePrivateAddressSpace(
            data.pages * data.page_size, data.flags);
}

void NvHostAsGpu::MapBufferEx(MapBufferExData& data, NvResult& result) {
    auto& gpu = HW::TegraX1::GPU::GPU::GetInstance();

    if (data.offset != 0x0) {
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

    data.offset = gpu.MapBufferToAddressSpace(map.addr + data.buffer_offset,
                                              size, data.flags);
    // LOG_DEBUG(HorizonServices, "OFFSET: 0x{:08x}", data.offset.Get());
}

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
