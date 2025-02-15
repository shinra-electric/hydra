#include "horizon/services/visrv/system_display_service.hpp"

namespace Hydra::Horizon::Services::ViSrv {

void ISystemDisplayService::RequestImpl(
    Readers& readers, Writers& writers,
    std::function<void(ServiceBase*)> add_service, Result& result, u32 id) {
    switch (id) {
    default:
        IApplicationDisplayService::RequestImpl(readers, writers, add_service,
                                                result, id);
        break;
    }
}

} // namespace Hydra::Horizon::Services::ViSrv
