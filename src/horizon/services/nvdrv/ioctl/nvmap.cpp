#include "horizon/services/nvdrv/ioctl/nvmap.hpp"

#include "hw/tegra_x1/gpu/gpu.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

DEFINE_IOCTL_TABLE(NvMap, 0x01, Create, 0x04, Alloc, 0x0E, GetId)

void NvMap::Create(CreateData& data, NvResult& result) {
    data.handle = HW::TegraX1::GPU::GPU::GetInstance().CreateMap(data.size);
}

void NvMap::Alloc(AllocData& data, NvResult& result) {
    HW::TegraX1::GPU::GPU::GetInstance().AllocateMap(data.handle, data.addr,
                                                     data.flags == 1);
}

void NvMap::GetId(GetIdData& data, NvResult& result) {
    data.id = HW::TegraX1::GPU::GPU::GetInstance().GetMapId(data.handle);
}

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
