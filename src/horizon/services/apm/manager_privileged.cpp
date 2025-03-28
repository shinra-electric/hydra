#include "horizon/services/apm/manager_privileged.hpp"

#include "horizon/services/apm/session.hpp"

namespace Hydra::Horizon::Services::Apm {

DEFINE_SERVICE_COMMAND_TABLE(IManagerPrivileged, 0, OpenSession)

void IManagerPrivileged::OpenSession(REQUEST_COMMAND_PARAMS) {
    add_service(new ISession());
}

} // namespace Hydra::Horizon::Services::Apm
