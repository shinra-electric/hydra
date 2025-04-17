#pragma once

#include "core/horizon/services/nvdrv/ioctl/fd_base.hpp"
#include "core/hw/tegra_x1/gpu/const.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

class ChannelBase : public FdBase {
  public:
    void Ioctl(IOCTL_PARAMS) override;

  protected:
    u64 user_data;

    // Ioctls
    DECLARE_IOCTL(SetUserData, readonly<u64> data;);
    DECLARE_IOCTL(GetUserData, writeonly<u64> data;, data);
    DECLARE_IOCTL(SetNvMapFd, readonly<u32> fd_id;);
    DECLARE_VIRTUAL_IOCTL(
        SubmitGpfifo, readonly<u64> gpfifo; readonly<u32> num_entries; union {
            writeonly<u32> detailed_error;
            readonly<HW::TegraX1::GPU::GpfifoFlags> flags;
        };
        readwrite<HW::TegraX1::GPU::Fence> out_fence;
        readonly<HW::TegraX1::GPU::GpfifoEntry> entries[];
        , detailed_error, out_fence);
    DECLARE_VIRTUAL_IOCTL(AllocObjCtx, readonly<u32> class_num;
                          readonly<u32> flags; writeonly<u64> obj_id;, obj_id);
    DECLARE_VIRTUAL_IOCTL(ZCullBind, readonly<gpu_vaddr_t> addr;
                          readonly<u32> mode; readonly<u32> reserved;, );
    DECLARE_VIRTUAL_IOCTL(SetErrorNotifier, readonly<u64> offset;
                          readonly<u64> size; readonly<u32> mem;
                          readonly<u32> reserved;, );
    DECLARE_VIRTUAL_IOCTL(SetPriority, readonly<u32> priority;, );
    DECLARE_VIRTUAL_IOCTL(GetErrorNotification, writeonly<u64> timestamp;
                          writeonly<u32> info32; writeonly<u16> info16;
                          writeonly<u16> status;
                          , timestamp, info32, info16, status);
    DECLARE_VIRTUAL_IOCTL(AllocGpfifoEx, readonly<u32> num_entries;
                          readonly<u32> num_jobs; readonly<u32> flags;
                          writeonly<HW::TegraX1::GPU::Fence> fence;
                          readonly<u32> reserved[3];, fence);
};

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
