#include "horizon/services/time/static_service.hpp"

#include "horizon/cmif.hpp"
#include "horizon/kernel.hpp"
#include "horizon/services/time/steady_clock.hpp"
#include "horizon/services/time/system_clock.hpp"
#include "horizon/services/time/time_zone_service.hpp"

namespace Hydra::Horizon::Services::Time {

void StaticService::Request(Kernel& kernel, Writer& writer,
                            Writer& move_handles_writer, u8* in_ptr) {
    auto cmif_in = Cmif::read_in_header(in_ptr);

    Result* res = Cmif::write_out_header(writer);

    switch (cmif_in.command_id) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        CreateService(kernel, move_handles_writer, cmif_in.command_id);
        break;
    default:
        printf("Unknown time::static_service request %u\n", cmif_in.command_id);
        break;
    }

    *res = RESULT_SUCCESS;
}

void StaticService::CreateService(Kernel& kernel, Writer& move_handles_writer,
                                  u32 id) {
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

        handle = kernel.AddService<SystemClock>(type);
        break;
    }
    case 2:
        handle = kernel.AddService<SteadyClock>();
        break;
    case 3:
        handle = kernel.AddService<TimeZoneService>();
        break;
    default:
        printf("Unknown time service command %u\n", id);
        break;
    }

    // Out
    move_handles_writer.Write(handle);
}

} // namespace Hydra::Horizon::Services::Time
