#pragma once

#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/services/nvdrv/ioctl/channel_base.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

class NvHostGpu : public ChannelBase {
  public:
    NvHostGpu()
        : sm_exception_bpt_int_report_event(new Kernel::Event()),
          sm_exception_bpt_pause_report_event(new Kernel::Event()),
          error_notifier_event(new Kernel::Event()) {}

    void QueryEvent(u32 event_id_u32, handle_id_t& out_handle_id,
                    NvResult& result) override;

  private:
    Kernel::HandleWithId<Kernel::Event> sm_exception_bpt_int_report_event;
    Kernel::HandleWithId<Kernel::Event> sm_exception_bpt_pause_report_event;
    Kernel::HandleWithId<Kernel::Event> error_notifier_event;

    // Ioctls
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
