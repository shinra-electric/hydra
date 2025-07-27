#include "core/horizon/services/nifm/static_service.hpp"

#include "core/horizon/services/nifm/general_service.hpp"

namespace hydra::horizon::services::nifm {

DEFINE_SERVICE_COMMAND_TABLE(IStaticService, 4, CreateGeneralServiceOld, 5,
                             CreateGeneralService)

result_t IStaticService::CreateGeneralServiceOld(RequestContext* ctx) {
    AddService(*ctx, new IGeneralService());
    return RESULT_SUCCESS;
}

result_t IStaticService::CreateGeneralService(RequestContext* ctx) {
    // TODO: some PID stuff
    AddService(*ctx, new IGeneralService());
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::nifm
