#pragma once

#include "core/horizon/services/nvdrv/ioctl/fd_base.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

class NvHostCtrl : public FdBase {
  public:
    void Ioctl(IOCTL_PARAMS) override;

  private:
    // Ioctls
    // TODO: array
    DECLARE_IOCTL(GetConfig, readonly<char> name[0x41];
                  readonly<char> key[0x41]; writeonly<char /*[0x101]*/> value;
                  , value);
    DECLARE_IOCTL(SyncptWaitEvent, readonly<u32> id; readonly<u32> tresh;
                  writeonly<i32> timeout; writeonly<u32> value;, value);
};

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
