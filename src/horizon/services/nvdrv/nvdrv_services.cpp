#include "horizon/services/nvdrv/nvdrv_services.hpp"

namespace Hydra::Horizon::Services::NvDrv {

void INvDrvServices::RequestImpl(Readers& readers, Writers& writers,
                                 std::function<void(ServiceBase*)> add_service,
                                 Result& result, u32 id) {
    LOG_WARNING(HorizonServices, "Unknown request {}", id);
}

} // namespace Hydra::Horizon::Services::NvDrv
