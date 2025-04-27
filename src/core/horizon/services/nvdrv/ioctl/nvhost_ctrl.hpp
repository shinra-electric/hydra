#pragma once

#include "core/horizon/services/nvdrv/ioctl/fd_base.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

typedef writeonly_array<u8, 0x101> writeonly_array_u8_0x101;

class NvHostCtrl : public FdBase {
  public:
    void Ioctl(IOCTL_PARAMS) override;

  private:
    // Ioctls
    DECLARE_IOCTL(GetConfig, char name[0x41]; char key[0x41];
                  writeonly_array_u8_0x101 value;, value);
    DECLARE_IOCTL(SyncptWaitEvent, readonly<u32> id; readonly<u32> tresh;
                  writeonly<i32> timeout; writeonly<u32> value;, value);
};

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
