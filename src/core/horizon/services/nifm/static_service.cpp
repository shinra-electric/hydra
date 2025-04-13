#include "core/horizon/services/nifm/static_service.hpp"

#include "core/horizon/services/nifm/general_service.hpp"

namespace Hydra::Horizon::Services::Nifm {

DEFINE_SERVICE_COMMAND_TABLE(IStaticService, 4, CreateGeneralServiceOld)

void IStaticService::CreateGeneralServiceOld(REQUEST_COMMAND_PARAMS) {
    add_service(new IGeneralService());
}

} // namespace Hydra::Horizon::Services::Nifm
