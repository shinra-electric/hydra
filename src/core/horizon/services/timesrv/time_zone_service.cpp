#include "core/horizon/services/timesrv/time_zone_service.hpp"

namespace hydra::horizon::services::timesrv {

DEFINE_SERVICE_COMMAND_TABLE(ITimeZoneService, 0, GetDeviceLocationName, 4,
                             LoadTimeZoneRule, 101, ToCalendarTimeWithMyRule)

result_t ITimeZoneService::GetDeviceLocationName(LocationName* out_name) {
    LOG_FUNC_STUBBED(Services);

    // HACK
    std::memcpy(out_name, "UTC\0", 4);
    return RESULT_SUCCESS;
}

result_t ITimeZoneService::LoadTimeZoneRule(
    LocationName location_name,
    OutBuffer<BufferAttr::MapAlias> out_rule_buffer) {
    LOG_DEBUG(Services, "Location: {}", location_name.name);

    LOG_FUNC_STUBBED(Services);

    // HACK
    out_rule_buffer.writer->Write(TimeZoneRule{});
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::timesrv
