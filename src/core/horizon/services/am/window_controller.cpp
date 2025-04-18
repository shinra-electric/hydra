#include "core/horizon/services/am/window_controller.hpp"

namespace Hydra::Horizon::Services::Am {

DEFINE_SERVICE_COMMAND_TABLE(IWindowController, 1, GetAppletResourceUserId, 10,
                             AcquireForegroundRights)

void IWindowController::GetAppletResourceUserId(REQUEST_COMMAND_PARAMS) {
    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    writers.writer.Write<u64>(1);
}

} // namespace Hydra::Horizon::Services::Am
