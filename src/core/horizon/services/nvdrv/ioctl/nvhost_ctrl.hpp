#pragma once

#include "core/horizon/services/nvdrv/ioctl/fd_base.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

class NvHostCtrl : public FdBase {
  public:
    void Ioctl(IOCTL_PARAMS) override;

  private:
    // Ioctls
};

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
