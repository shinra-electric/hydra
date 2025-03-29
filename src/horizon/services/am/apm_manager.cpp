#include "horizon/services/am/apm_manager.hpp"

#include "horizon/services/am/session.hpp"

namespace Hydra::Horizon::Services::Am {

DEFINE_SERVICE_COMMAND_TABLE(IApmManager, 0, OpenSession)

void IApmManager::OpenSession(REQUEST_COMMAND_PARAMS) {
    add_service(new ISession());
}

} // namespace Hydra::Horizon::Services::Am
