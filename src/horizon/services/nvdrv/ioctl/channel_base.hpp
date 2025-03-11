#pragma once

#include "horizon/services/nvdrv/ioctl/fd_base.hpp"
#include "hw/tegra_x1/gpu/const.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

class ChannelBase : public FdBase {
  public:
    void Ioctl(IOCTL_PARAMS) override;

  protected:
    // Ioctls
    DECLARE_IOCTL(SetNvMapFd, readonly<u32> fd_id;);
    DECLARE_VIRTUAL_IOCTL(
        SubmitGpfifo, readonly<u64> gpfifo; readonly<u32> num_entries; union {
            writeonly<u32> detailed_error;
            readonly<HW::TegraX1::GPU::GpfifoFlags> flags;
        };
        readwrite<HW::TegraX1::GPU::Fence> out_fence;
        readonly<HW::TegraX1::GPU::GpfifoEntry> entries[];
        , detailed_error, out_fence);
    DECLARE_VIRTUAL_IOCTL(GetErrorNotification, writeonly<u64> timestamp;
                          writeonly<u32> info32; writeonly<u16> info16;
                          writeonly<u16> status;
                          , timestamp, info32, info16, status);
};

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
