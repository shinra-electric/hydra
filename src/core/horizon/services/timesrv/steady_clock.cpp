#include "core/horizon/services/timesrv/steady_clock.hpp"

namespace hydra::horizon::services::timesrv {

DEFINE_SERVICE_COMMAND_TABLE(ISteadyClock, 0, GetCurrentTimePoint)

result_t
ISteadyClock::GetCurrentTimePoint(SteadyClockTimePoint* out_time_point) {
    LOG_FUNC_STUBBED(Services);

    // TODO: what units?
    // TODO: should this be since the
    // clock was created?
    *out_time_point = {
        .time_point =
            u64(std::chrono::steady_clock::now().time_since_epoch().count()),
        .clock_source_id = 0xcccccccccccccccc,
    };
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::timesrv
