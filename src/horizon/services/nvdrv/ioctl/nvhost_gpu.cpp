#include "horizon/services/nvdrv/ioctl/nvhost_gpu.hpp"
#include "common/logging/log.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

enum class EventId {
    SmException_BptIntReport = 1,
    SmException_BptPauseReport,
    ErrorNotifierEvent,
};

NvResult NvHostGpu::QueryEvent(u32 event_id_u32, HandleId& out_handle_id) {
    LOG_WARNING(HorizonServices, "Not implemented");

    auto event_id = static_cast<EventId>(event_id_u32);
    switch (event_id) {
    case EventId::SmException_BptIntReport:
        // TODO: set out_handle_id
        return NvResult::Success;
    case EventId::SmException_BptPauseReport:
        // TODO: set out_handle_id
        return NvResult::Success;
    case EventId::ErrorNotifierEvent:
        // TODO: set out_handle_id
        return NvResult::Success;
    default:
        return NvResult::NotSupported;
    }
}

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
