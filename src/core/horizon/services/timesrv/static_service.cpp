#include "core/horizon/services/timesrv/static_service.hpp"

#include "core/horizon/services/timesrv/steady_clock.hpp"
#include "core/horizon/services/timesrv/system_clock.hpp"
#include "core/horizon/services/timesrv/time_zone_service.hpp"

namespace Hydra::Horizon::Services::TimeSrv {

DEFINE_SERVICE_COMMAND_TABLE(IStaticService, 0, GetStandardUserSystemClock, 1,
                             GetStandardNetworkSystemClock, 2,
                             GetStandardSteadyClock, 3, GetTimeZoneService, 4,
                             GetStandardLocalSystemClock, 5,
                             GetEphemeralNetworkSystemClock)

void IStaticService::GetStandardUserSystemClock(REQUEST_COMMAND_PARAMS) {
    add_service(new ISystemClock(SystemClockType::StandardUser));
}

void IStaticService::GetStandardNetworkSystemClock(REQUEST_COMMAND_PARAMS) {
    add_service(new ISystemClock(SystemClockType::StandardNetwork));
}

void IStaticService::GetStandardSteadyClock(REQUEST_COMMAND_PARAMS) {
    add_service(new ISteadyClock());
}

void IStaticService::GetTimeZoneService(REQUEST_COMMAND_PARAMS) {
    add_service(new ITimeZoneService());
}

void IStaticService::GetStandardLocalSystemClock(REQUEST_COMMAND_PARAMS) {
    add_service(new ISystemClock(SystemClockType::StandardLocal));
}

void IStaticService::GetEphemeralNetworkSystemClock(REQUEST_COMMAND_PARAMS) {
    add_service(new ISystemClock(SystemClockType::EphemeralNetwork));
}

} // namespace Hydra::Horizon::Services::TimeSrv
