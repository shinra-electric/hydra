#include "core/horizon/services/nifm/general_service.hpp"

#include "core/horizon/services/nifm/request.hpp"

namespace Hydra::Horizon::Services::Nifm {

DEFINE_SERVICE_COMMAND_TABLE(IGeneralService, 4, CreateRequest)

void IGeneralService::CreateRequest(REQUEST_COMMAND_PARAMS) {
    const auto requirement_preset = readers.reader.Read<i32>();
    LOG_DEBUG(HorizonServices, "Requirement preset: {}", requirement_preset);

    add_service(new IRequest());

    // HACK
    std::this_thread::sleep_for(std::chrono::seconds(20));
}

} // namespace Hydra::Horizon::Services::Nifm
