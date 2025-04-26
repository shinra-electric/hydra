#include "core/horizon/services/nvdrv/ioctl/nvhost_gpu.hpp"

#include "core/hw/tegra_x1/gpu/const.hpp"
#include "core/hw/tegra_x1/gpu/gpu.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

namespace {

enum class EventId {
    SmExceptionBptIntReport = 1,
    SmExceptionBptPauseReport,
    ErrorNotifierEvent,
};

}

void NvHostGpu::QueryEvent(u32 event_id_u32, handle_id_t& out_handle_id,
                           NvResult& result) {
    LOG_FUNC_STUBBED(HorizonServices);

    auto event_id = static_cast<EventId>(event_id_u32);
    switch (event_id) {
    case EventId::SmExceptionBptIntReport:
        out_handle_id = sm_exception_bpt_int_report_event.id;
        break;
    case EventId::SmExceptionBptPauseReport:
        out_handle_id = sm_exception_bpt_pause_report_event.id;
        break;
    case EventId::ErrorNotifierEvent:
        out_handle_id = error_notifier_event.id;
        break;
    default:
        result = NvResult::NotSupported;
        break;
    }
}

void NvHostGpu::SubmitGpfifo(SubmitGpfifoData& data, NvResult& out_result) {
    auto ptr = const_cast<HW::TegraX1::GPU::GpfifoEntry*>(
        &(data.entries[0].Get())); // HACK
    HW::TegraX1::GPU::GPU::GetInstance().GetPfifo().SubmitEntries(
        std::vector<HW::TegraX1::GPU::GpfifoEntry>(ptr, ptr + data.num_entries),
        data.flags);

    // HACK
    data.out_fence = HW::TegraX1::GPU::Fence{};
}

void NvHostGpu::AllocObjCtx(AllocObjCtxData& data, NvResult& out_result) {
    LOG_FUNC_STUBBED(HorizonServices);
}

void NvHostGpu::ZCullBind(ZCullBindData& data, NvResult& out_result) {
    LOG_FUNC_STUBBED(HorizonServices);
}

void NvHostGpu::SetErrorNotifier(SetErrorNotifierData& data,
                                 NvResult& out_result) {
    LOG_FUNC_STUBBED(HorizonServices);
}

void NvHostGpu::SetPriority(SetPriorityData& data, NvResult& out_result) {
    LOG_FUNC_STUBBED(HorizonServices);
}

void NvHostGpu::GetErrorNotification(GetErrorNotificationData& data,
                                     NvResult& out_result) {
    LOG_FUNC_STUBBED(HorizonServices);

    data.timestamp = 0;           // TODO
    data.info32 = 0;              // TODO
    data.info16 = 0;              // TODO
    data.status = /*0xffff*/ 0x0; // TODO: correct?
}

void NvHostGpu::AllocGpfifoEx(AllocGpfifoExData& data, NvResult& out_result) {
    LOG_FUNC_STUBBED(HorizonServices);
}

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
