#include "core/horizon/services/am/apm_manager.hpp"

#include "core/horizon/services/am/session.hpp"

namespace hydra::horizon::services::am {

DEFINE_SERVICE_COMMAND_TABLE(IApmManager, 0, OpenSession)

result_t IApmManager::OpenSession(RequestContext* ctx) {
    AddService(*ctx, new ISession());
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::am
