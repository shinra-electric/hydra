#pragma once

#include "core/horizon/services/nvdrv/ioctl/channel_base.hpp"

namespace hydra::horizon::services::nvdrv::ioctl {

class NvDispDisp : public ChannelBase {
  public:
    NvDispDisp(u32 display_index_) : display_index{display_index_} {}

    NvResult Ioctl(IoctlContext& context, u32 type, u32 nr) override;

  private:
    u32 display_index;

    // Ioctls
    NvResult GetWindow(u32 unknown_x0);
};

} // namespace hydra::horizon::services::nvdrv::ioctl
