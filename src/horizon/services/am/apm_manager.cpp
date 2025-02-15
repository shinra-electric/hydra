#include "horizon/services/am/apm_manager.hpp"

namespace Hydra::Horizon::Services::Am {

void IApmManager::RequestImpl(Readers& readers, Writers& writers,
                              std::function<void(ServiceBase*)> add_service,
                              Result& result, u32 id) {
    LOG_WARNING(HorizonServices, "Unknown request {}", id);
}

} // namespace Hydra::Horizon::Services::Am
