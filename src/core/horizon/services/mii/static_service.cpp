#include "core/horizon/services/mii/static_service.hpp"

#include "core/horizon/services/mii/database_service.hpp"

namespace hydra::horizon::services::mii {

DEFINE_SERVICE_COMMAND_TABLE(IStaticService, 0, GetDatabaseService)

result_t IStaticService::GetDatabaseService(add_service_fn_t add_service) {
    add_service(new IDatabaseService());
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::mii
