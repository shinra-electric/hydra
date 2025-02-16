#pragma once

#include "horizon/services/nvdrv/ioctl/fd_base.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

class NvMap : public FdBase {
  public:
    void Ioctl(IOCTL_PARAMS) override;

  private:
    // Ioctls
    DECLARE_IOCTL(Create, readonly<u32> size; writeonly<Handle> handle;, handle)
};

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
