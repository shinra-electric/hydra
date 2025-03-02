#pragma once

#include "horizon/services/nvdrv/ioctl/fd_base.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

class ChannelBase : public FdBase {
  public:
    void Ioctl(IOCTL_PARAMS) override;

  protected:
    // Ioctls
    DECLARE_IOCTL(SetNvMapFd, readonly<u32> fd_id;);
    DECLARE_VIRTUAL_IOCTL(GetErrorNotification, writeonly<u64> timestamp;
                          writeonly<u32> info32; writeonly<u16> info16;
                          writeonly<u16> status;
                          , timestamp, info32, info16, status) {
        LOG_ERROR(HorizonServices, "Not implemented for this channel");
    }
};

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
