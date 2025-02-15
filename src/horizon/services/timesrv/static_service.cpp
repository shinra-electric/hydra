#include "horizon/services/timesrv/static_service.hpp"

#include "horizon/os.hpp"

namespace Hydra::Horizon::Services::TimeSrv {

void IStaticService::RequestImpl(REQUEST_IMPL_PARAMS) {
    switch (id) {
    case 0:
        add_service(GET_SERVICE_EXPLICIT(TimeSrv, standard_user_system_clock));
        break;
    case 1:
        add_service(
            GET_SERVICE_EXPLICIT(TimeSrv, standard_network_system_clock));
        break;
    case 2:
        add_service(GET_SERVICE_EXPLICIT(TimeSrv, steady_clock));
        break;
    case 3:
        add_service(GET_SERVICE_EXPLICIT(TimeSrv, time_zone_service));
        break;
    case 4:
        add_service(GET_SERVICE_EXPLICIT(TimeSrv, standard_local_system_clock));
        break;
    case 5:
        add_service(
            GET_SERVICE_EXPLICIT(TimeSrv, ephemeral_network_system_clock));
        break;
    default:
        LOG_WARNING(HorizonServices, "Unknown request {}", id);
        break;
    }
}

} // namespace Hydra::Horizon::Services::TimeSrv
