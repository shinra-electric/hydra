#pragma once

#include "horizon/services/nvdrv/ioctl/channel_base.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

class NvHostGpu : public ChannelBase {
  public:
    void QueryEvent(u32 event_id_u32, HandleId& out_handle_id,
                    NvResult& out_result) override;

  private:
    void GetErrorNotification(GetErrorNotificationData& data,
                              NvResult& out_result) override;
};

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
