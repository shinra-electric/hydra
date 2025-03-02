#pragma once

#include "horizon/services/nvdrv/ioctl/fd_base.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

class NvHostAsGpu : public FdBase {
  public:
    void Ioctl(IOCTL_PARAMS) override;

  private:
    // Ioctls
    DECLARE_IOCTL(
        AllocSpace, readonly<u32> pages; readonly<u32> page_size;
        readonly<u32> flags; readonly<u32> pad; union {
            writeonly<u64> offset;
            readonly<u64> align;
        },
                                                offset);
};

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
