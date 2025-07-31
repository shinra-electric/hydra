#pragma once

#include "core/horizon/services/nvdrv/ioctl/channel_base.hpp"

namespace hydra::horizon::services::nvdrv::ioctl {

class NvDispCtrl : public ChannelBase {
  public:
    NvResult Ioctl(IoctlContext& context, u32 type, u32 nr) override;

  private:
    // Ioctls
    NvResult NumOutputs(u32* out_num_outputs);
    NvResult GetDisplayProperties(std::array<u8, 20> unknown_x0);
};

} // namespace hydra::horizon::services::nvdrv::ioctl
