#include "horizon/services/timesrv/static_service.hpp"

#include "horizon/cmif.hpp"
#include "horizon/kernel.hpp"
#include "horizon/services/timesrv/steady_clock.hpp"
#include "horizon/services/timesrv/system_clock.hpp"
#include "horizon/services/timesrv/time_zone_service.hpp"

namespace Hydra::Horizon::Services::TimeSrv {

void IStaticService::Request(Readers& readers, Writers& writers,
                             std::function<void(ServiceBase*)> add_service) {
    auto cmif_in = readers.reader.Read<Cmif::InHeader>();

    Result* res = Cmif::write_out_header(writers.writer);

    switch (cmif_in.command_id) {
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
        LOG_WARNING(HorizonServices, "Unknown request {}", cmif_in.command_id);
        break;
    }

    *res = RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::TimeSrv
