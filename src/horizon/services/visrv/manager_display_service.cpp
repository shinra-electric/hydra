#include "horizon/services/visrv/manager_display_service.hpp"
#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services::ViSrv {

void IManagerDisplayService::RequestImpl(REQUEST_IMPL_PARAMS) {
    LOG_WARNING(HorizonServices, "Unknown request {}", id);
}

} // namespace Hydra::Horizon::Services::ViSrv
