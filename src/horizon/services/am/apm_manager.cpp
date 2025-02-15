#include "horizon/services/am/apm_manager.hpp"

namespace Hydra::Horizon::Services::Am {

void IApmManager::RequestImpl(REQUEST_IMPL_PARAMS) {
    LOG_WARNING(HorizonServices, "Unknown request {}", id);
}

} // namespace Hydra::Horizon::Services::Am
