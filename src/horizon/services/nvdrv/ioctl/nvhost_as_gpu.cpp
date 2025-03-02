#include "horizon/services/nvdrv/ioctl/nvhost_as_gpu.hpp"

#include "hw/tegra_x1/gpu/gpu.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

DEFINE_IOCTL_TABLE(NvHostAsGpu, 0x02, AllocSpace)

void NvHostAsGpu::AllocSpace(AllocSpaceData& data, NvResult& result) {
    data.offset = HW::TegraX1::GPU::GPU::GetInstance().CreateAddressSpace(
        data.pages * data.page_size, data.flags);
}

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
