#pragma once

#include "horizon/services/nvdrv/ioctl/channel_base.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

class NvHostGpu : public ChannelBase {
  public:
    NvResult QueryEvent(u32 event_id_u32, HandleId& out_handle_id) override;

  private:
};

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
