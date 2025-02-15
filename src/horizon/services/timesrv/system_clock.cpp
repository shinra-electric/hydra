#include "horizon/services/timesrv/system_clock.hpp"

namespace Hydra::Horizon::Services::TimeSrv {

void ISystemClock::RequestImpl(REQUEST_IMPL_PARAMS) {
    LOG_WARNING(HorizonServices, "Unknown request {}", id);
}

} // namespace Hydra::Horizon::Services::TimeSrv
