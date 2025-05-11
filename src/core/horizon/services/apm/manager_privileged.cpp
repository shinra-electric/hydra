#include "core/horizon/services/apm/manager_privileged.hpp"

#include "core/horizon/services/apm/session.hpp"

namespace Hydra::Horizon::Services::Apm {

DEFINE_SERVICE_COMMAND_TABLE(IManagerPrivileged, 0, OpenSession)

result_t IManagerPrivileged::OpenSession(add_service_fn_t add_service) {
    add_service(new ISession());
    return RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::Apm
