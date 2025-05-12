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

          NvResult QueryEvent(u32 event_id_u32, handle_id_t& out_handle_id) override;

  private:
    Kernel::HandleWithId<Kernel::Event> sm_exception_bpt_int_report_event;
    Kernel::HandleWithId<Kernel::Event> sm_exception_bpt_pause_report_event;
    Kernel::HandleWithId<Kernel::Event> error_notifier_event;

    // Ioctls
    NvResult SubmitGpfifo(u64 gpfifo, u32 num_entries, InOut<HW::TegraX1::GPU::GpfifoFlags, u32> inout_flags_and_detailed_error, InOutSingle<HW::TegraX1::GPU::Fence> inout_fence, const HW::TegraX1::GPU::GpfifoEntry* entries) override;
    NvResult AllocObjCtx(u32 class_num, u32 flags, u64* out_obj_id) override;
    NvResult ZCullBind(gpu_vaddr_t addr, u32 mode, u32 reserved) override;
    NvResult SetErrorNotifier(u64 offset, u64 size, u32 mem, u32 reserved) override;
    NvResult GetErrorNotification(u64* out_timestamp, u32* out_info32, u16* out_info16, u64* out_status) override;
    NvResult AllocGpfifoEx(u32 num_entries, u32 num_jobs, u32 flags, HW::TegraX1::GPU::Fence* out_fence, std::array<u32, 3> reserved) override;
};

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
