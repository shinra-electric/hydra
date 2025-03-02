#include "horizon/services/nvdrv/ioctl/channel_base.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

DEFINE_IOCTL_TABLE(ChannelBase,
                   DEFINE_IOCTL_TABLE_ENTRY(0x48, 0x01, SetNvMapFd))

void ChannelBase::SetNvMapFd(SetNvMapFdData& data, NvResult& result) {
    LOG_WARNING(HorizonServices, "Not implemented");

    // TODO: what's the purpose of knowing the nvmap fd?
}

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
