#include "horizon/services/time/static_service.hpp"

#include "horizon/cmif.hpp"
#include "horizon/kernel.hpp"
#include "horizon/services/time/steady_clock.hpp"
#include "horizon/services/time/system_clock.hpp"
#include "horizon/services/time/time_zone_service.hpp"

namespace Hydra::Horizon::Services::Time {

void StaticService::Request(Writers& writers, u8* in_ptr,
                            std::function<void(ServiceBase*)> add_service) {
    auto cmif_in = Cmif::read_in_header(in_ptr);

    Result* res = Cmif::write_out_header(writers.writer);

    switch (cmif_in.command_id) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        CreateService(cmif_in.command_id, add_service);
        break;
    default:
        Logging::log(Logging::Level::Warning,
                     "Unknown time::static_service request {}",
                     cmif_in.command_id);
        break;
    }

    *res = RESULT_SUCCESS;
}

void StaticService::CreateService(
    u32 id, std::function<void(ServiceBase*)> add_service) {
    Handle handle;
    switch (id) {
    case 0:
    case 1:
    case 4:
    case 5: {
        SystemClockType type;
        switch (id) {
        case 0:
            type = SystemClockType::StandardUser;
            break;
        case 1:
            type = SystemClockType::StandardNetwork;
            break;
        case 2:
            type = SystemClockType::StandardLocal;
            break;
        case 3:
            type = SystemClockType::EphemeralNetwork;
            break;
        }

        add_service(new SystemClock());
        break;
    }
    case 2:
        add_service(new SteadyClock());
        break;
    case 3:
        add_service(new TimeZoneService());
        break;
    default:
        Logging::log(Logging::Level::Warning,
                     "Unknown time service CreateService command {}", id);
        break;
    }
}

} // namespace Hydra::Horizon::Services::Time
