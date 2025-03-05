#pragma once

#include "horizon/services/nvdrv/ioctl/fd_base.hpp"
#include "hw/tegra_x1/gpu/const.hpp"

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
    DECLARE_IOCTL(MapBufferEx, readonly<u32> flags;
                  readwrite<HW::TegraX1::GPU::NvKind> kind;
                  readonly<u32> nvmap_id; u32 reserved;
                  readonly<u64> buffer_offset; readonly<u64> mapping_size;
                  readwrite<u64> offset;, kind, offset);
};

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
