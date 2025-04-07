#include "horizon/services/nvdrv/ioctl/nvhost_gpu.hpp"

#include "hw/tegra_x1/gpu/const.hpp"
#include "hw/tegra_x1/gpu/gpu.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

enum class EventId {
    SmException_BptIntReport = 1,
    SmException_BptPauseReport,
    ErrorNotifierEvent,
};

void NvHostGpu::QueryEvent(u32 event_id_u32, HandleId& out_handle_id,
                           NvResult& out_result) {
    LOG_FUNC_STUBBED(HorizonServices);

    auto event_id = static_cast<EventId>(event_id_u32);
    switch (event_id) {
    case EventId::SmException_BptIntReport:
        // TODO: set out_handle_id
        break;
    case EventId::SmException_BptPauseReport:
        // TODO: set out_handle_id
        break;
    case EventId::ErrorNotifierEvent:
        // TODO: set out_handle_id
        break;
    default:
        out_result = NvResult::NotSupported;
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
