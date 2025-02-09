#include "horizon/services/time/system_clock.hpp"

#include "horizon/cmif.hpp"

namespace Hydra::Horizon::Services::Time {

void SystemClock::Request(Kernel& kernel, Writer& writer,
                          Writer& move_handles_writer, u8* in_ptr) {
    auto cmif_in = Cmif::read_in_header(in_ptr);

    Result* res = Cmif::write_out_header(writer);

    switch (cmif_in.command_id) {
    default:
        printf("Unknown time::system_clock request %u\n", cmif_in.command_id);
        break;
    }

    *res = RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::Time
