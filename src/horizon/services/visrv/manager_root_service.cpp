#include "horizon/services/visrv/manager_root_service.hpp"

#include "horizon/os.hpp"

namespace Hydra::Horizon::Services::ViSrv {

void IManagerRootService::RequestImpl(REQUEST_IMPL_PARAMS) {
    switch (id) {
    case 0: // GetDisplayService
        add_service(GET_SERVICE_EXPLICIT(ViSrv, application_display_service));
        break;
    case 2: // GetDisplayServiceWithProxyNameExchange
        // TODO: should take input u64 and u32
        add_service(GET_SERVICE_EXPLICIT(ViSrv, application_display_service));
        break;
    default:
        LOG_WARNING(HorizonServices, "Unknown request {}", id);
        break;
    }
}

} // namespace Hydra::Horizon::Services::ViSrv
