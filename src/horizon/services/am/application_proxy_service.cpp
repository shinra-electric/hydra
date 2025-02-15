#include "horizon/services/am/application_proxy_service.hpp"

#include "horizon/os.hpp"

namespace Hydra::Horizon::Services::Am {

void IApplicationProxyService::RequestImpl(REQUEST_IMPL_PARAMS) {
    switch (id) {
    case 0: // OpenApplicationProxy
        add_service(GET_SERVICE_EXPLICIT(Am, application_proxy));
        break;
    default:
        LOG_WARNING(HorizonServices, "Unknown request {}", id);
        break;
    }
}

} // namespace Hydra::Horizon::Services::Am
