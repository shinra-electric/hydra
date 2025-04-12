#pragma once

#include "core/horizon/services/nvdrv/ioctl/channel_base.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

class NvHostGpu : public ChannelBase {
  public:
    void QueryEvent(u32 event_id_u32, HandleId& out_handle_id,
                    NvResult& out_result) override;

  private:
    void SubmitGpfifo(SubmitGpfifoData& data, NvResult& out_result) override;
    void AllocObjCtx(AllocObjCtxData& data, NvResult& out_result) override;
    void ZCullBind(ZCullBindData& data, NvResult& out_result) override;
    void SetErrorNotifier(SetErrorNotifierData& data,
                          NvResult& out_result) override;
    void SetPriority(SetPriorityData& data, NvResult& out_result) override;
    void GetErrorNotification(GetErrorNotificationData& data,
                              NvResult& out_result) override;
    void AllocGpfifoEx(AllocGpfifoExData& data, NvResult& out_result) override;
};

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
