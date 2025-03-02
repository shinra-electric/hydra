#pragma once

#include "horizon/services/nvdrv/ioctl/fd_base.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

class ChannelBase : public FdBase {
  public:
    void Ioctl(IOCTL_PARAMS) override;

  private:
    // Ioctls
    DECLARE_IOCTL(SetNvMapFd, readonly<u32> fd_id;);
};

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
