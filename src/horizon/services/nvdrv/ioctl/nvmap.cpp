#include "horizon/services/nvdrv/ioctl/nvmap.hpp"
#include "common/logging/log.hpp"
#include "horizon/services/nvdrv/ioctl/ioctl_base.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

DEFINE_IOCTL_TABLE(NvMap, 1, Create)

void NvMap::Create(CreateData& data, NvResult& result) {
    LOG_DEBUG(HorizonServices, "Size: {}", data.size.Get());
    LOG_WARNING(HorizonServices, "Not implemented");
}

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
