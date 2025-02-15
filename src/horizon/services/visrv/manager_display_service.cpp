#include "horizon/services/visrv/manager_display_service.hpp"

namespace Hydra::Horizon::Services::ViSrv {

void IManagerDisplayService::RequestImpl(
    Readers& readers, Writers& writers,
    std::function<void(ServiceBase*)> add_service, Result& result, u32 id) {
    LOG_WARNING(HorizonServices, "Unknown request {}", id);
}

} // namespace Hydra::Horizon::Services::ViSrv
