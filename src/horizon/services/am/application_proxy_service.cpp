#include "horizon/services/am/application_proxy_service.hpp"

#include "horizon/kernel.hpp"
#include "horizon/services/am/application_proxy.hpp"

namespace Hydra::Horizon::Services::Am {

void IApplicationProxyService::RequestImpl(
    Readers& readers, Writers& writers,
    std::function<void(ServiceBase*)> add_service, Result& result, u32 id) {
    switch (id) {
    case 0: // OpenApplicationProxy
        add_service(new IApplicationProxy());
        break;
    default:
        LOG_WARNING(HorizonServices, "Unknown request {}", id);
        break;
    }
}

} // namespace Hydra::Horizon::Services::Am
