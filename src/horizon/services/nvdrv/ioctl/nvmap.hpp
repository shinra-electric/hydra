#pragma once

#include "horizon/services/nvdrv/ioctl/fd_base.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

class NvMap : public FdBase {
  public:
    void Ioctl(IOCTL_PARAMS) override;

  private:
    // Ioctls
    DECLARE_IOCTL(Create, readonly<u32> size; writeonly<HandleId> handle_id;
                  , handle_id);
    DECLARE_IOCTL(Alloc, readonly<HandleId> handle_id; readonly<u32> heapmask;
                  readonly<u32> flags; readwrite<u32> alignment;
                  readonly<u8> kind; readonly<u8> pad[7]; readonly<uptr> addr;
                  , alignment);
    DECLARE_IOCTL(Free, readonly<HandleId> handle_id; readonly<u32> pad;
                  writeonly<u64> addr; writeonly<u64> size;
                  writeonly<u32> flags;, addr, size, flags);
    DECLARE_IOCTL(GetId, writeonly<u32> id; readonly<HandleId> handle_id;, id);
};

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
