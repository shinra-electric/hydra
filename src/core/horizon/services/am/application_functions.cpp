#include "core/horizon/services/am/application_functions.hpp"

#include "core/horizon/services/am/storage.hpp"
#include "core/horizon/state_manager.hpp"

namespace Hydra::Horizon::Services::Am {

DEFINE_SERVICE_COMMAND_TABLE(IApplicationFunctions, 1, PopLaunchParameter, 20,
                             EnsureSaveData, 22, SetTerminateResult, 40,
                             NotifyRunning)

void IApplicationFunctions::PopLaunchParameter(REQUEST_COMMAND_PARAMS) {
    const auto kind = readers.reader.Read<LaunchParameterKind>();
    LOG_DEBUG(HorizonServices, "Kind: {}", kind);

    add_service(
        new IStorage(StateManager::GetInstance().PopLaunchParameter(kind)));
}

void IApplicationFunctions::EnsureSaveData(REQUEST_COMMAND_PARAMS) {
    const auto user_id = readers.reader.Read<u128>();
    LOG_DEBUG(HorizonServices, "User ID: {}", user_id);

    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    // NOTE: writing anything other than 0x0 causes the game to launch the
    // dataErase LibraryApplet
    writers.writer.Write<u64>(0x0);
}

void IApplicationFunctions::SetTerminateResult(REQUEST_COMMAND_PARAMS) {
    auto res = readers.reader.Read<Kernel::Result>();
    LOG_DEBUG(HorizonServices, "Result: 0x{:x}", res);
}

} // namespace Hydra::Horizon::Services::Am
