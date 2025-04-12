#include "core/horizon/services/am/application_functions.hpp"

#include "core/horizon/services/am/storage.hpp"
#include "core/horizon/state_manager.hpp"

namespace Hydra::Horizon::Services::Am {

DEFINE_SERVICE_COMMAND_TABLE(IApplicationFunctions, 1, PopLaunchParameter, 22,
                             SetTerminateResult, 40, NotifyRunning)

void IApplicationFunctions::PopLaunchParameter(REQUEST_COMMAND_PARAMS) {
    const auto kind = readers.reader.Read<LaunchParameterKind>();
    LOG_DEBUG(HorizonServices, "Kind: {}", kind);

    add_service(
        new IStorage(StateManager::GetInstance().PopLaunchParameter(kind)));
}

void IApplicationFunctions::SetTerminateResult(REQUEST_COMMAND_PARAMS) {
    auto res = readers.reader.Read<Result>();
    LOG_DEBUG(HorizonServices, "Result: 0x{:x}", res);
}

} // namespace Hydra::Horizon::Services::Am
