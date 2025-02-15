#include "horizon/services/am/application_proxy.hpp"

#include "horizon/services/am/common_state_getter.hpp"

namespace Hydra::Horizon::Services::Am {

void IApplicationProxy::RequestImpl(REQUEST_IMPL_PARAMS) {
    switch (id) {
    case 0: // GetCommonStateGetter
        add_service(new ICommonStateGetter());
        break;
    default:
        LOG_WARNING(HorizonServices, "Unknown request {}", id);
        break;
    }
}

} // namespace Hydra::Horizon::Services::Am
