#include "horizon/services/nvdrv/nvdrv_services.hpp"

namespace Hydra::Horizon::Services::NvDrv {

void INvDrvServices::RequestImpl(REQUEST_IMPL_PARAMS) {
    LOG_WARNING(HorizonServices, "Unknown request {}", id);
}

} // namespace Hydra::Horizon::Services::NvDrv
