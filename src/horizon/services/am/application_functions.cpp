#include "horizon/services/am/application_functions.hpp"

#include "horizon/services/am/storage.hpp"
#include "horizon/state_manager.hpp"

namespace Hydra::Horizon::Services::Am {

DEFINE_SERVICE_COMMAND_TABLE(IApplicationFunctions, 1, PopLaunchParameter, 40,
                             NotifyRunning)

void IApplicationFunctions::PopLaunchParameter(REQUEST_COMMAND_PARAMS) {
    const auto kind = readers.reader.Read<LaunchParameterKind>();
    LOG_DEBUG(HorizonServices, "Kind: {}", kind);

    add_service(
        new IStorage(StateManager::GetInstance().PopLaunchParameter(kind)));
}

} // namespace Hydra::Horizon::Services::Am
