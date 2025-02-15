#include "horizon/services/timesrv/steady_clock.hpp"

namespace Hydra::Horizon::Services::TimeSrv {

void ISteadyClock::RequestImpl(REQUEST_IMPL_PARAMS) {
    LOG_WARNING(HorizonServices, "Unknown request {}", id);
}

} // namespace Hydra::Horizon::Services::TimeSrv
