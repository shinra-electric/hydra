#include "horizon/services/nvdrv/ioctl/nvhost_gpu.hpp"
#include "common/logging/log.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

enum class EventId {
    SmException_BptIntReport = 1,
    SmException_BptPauseReport,
    ErrorNotifierEvent,
};

void NvHostGpu::QueryEvent(u32 event_id_u32, HandleId& out_handle_id,
                           NvResult& out_result) {
    LOG_WARNING(HorizonServices, "Not implemented");

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

void NvHostGpu::GetErrorNotification(GetErrorNotificationData& data,
                                     NvResult& out_result) {
    LOG_WARNING(HorizonServices, "Not implemented");

    data.timestamp = 0; // TODO
    data.info32 = 0;    // TODO
    data.info16 = 0;    // TODO
    data.status = 0xffff;
}

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
