#include "horizon/services/timesrv/static_service.hpp"

#include "horizon/kernel.hpp"
#include "horizon/services/timesrv/steady_clock.hpp"
#include "horizon/services/timesrv/system_clock.hpp"
#include "horizon/services/timesrv/time_zone_service.hpp"

namespace Hydra::Horizon::Services::TimeSrv {

void IStaticService::RequestImpl(REQUEST_IMPL_PARAMS) {
    switch (id) {
    case 0:
        add_service(new ISystemClock(SystemClockType::StandardUser));
        break;
    case 1:
        add_service(new ISystemClock(SystemClockType::StandardNetwork));
        break;
    case 2:
        add_service(new ISteadyClock());
        break;
    case 3:
        add_service(new ITimeZoneService());
        break;
    case 4:
        add_service(new ISystemClock(SystemClockType::StandardLocal));
        break;
    case 5:
        add_service(new ISystemClock(SystemClockType::EphemeralNetwork));
        break;
    default:
        LOG_WARNING(HorizonServices, "Unknown request {}", id);
        break;
    }
}

} // namespace Hydra::Horizon::Services::TimeSrv
