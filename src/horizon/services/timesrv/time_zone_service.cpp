#include "horizon/services/timesrv/time_zone_service.hpp"

namespace Hydra::Horizon::Services::TimeSrv {

void ITimeZoneService::RequestImpl(REQUEST_IMPL_PARAMS) {
    LOG_WARNING(HorizonServices, "Unknown request {}", id);
}

} // namespace Hydra::Horizon::Services::TimeSrv
