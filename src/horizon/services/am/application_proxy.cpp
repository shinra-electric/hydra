#include "horizon/services/am/application_proxy.hpp"

#include "horizon/services/am/common_state_getter.hpp"

namespace Hydra::Horizon::Services::Am {

DEFINE_SERVICE_COMMAND_TABLE(IApplicationProxy, 0, GetCommonStateGetter)

void IApplicationProxy::GetCommonStateGetter(REQUEST_COMMAND_PARAMS) {
    add_service(new ICommonStateGetter());
}

} // namespace Hydra::Horizon::Services::Am
