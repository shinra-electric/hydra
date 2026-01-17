#include "core/horizon/services/am/window_controller.hpp"

#include "core/horizon/kernel/process.hpp"

namespace hydra::horizon::services::am {

DEFINE_SERVICE_COMMAND_TABLE(IWindowController, 1, GetAppletResourceUserId, 10,
                             AcquireForegroundRights)

result_t IWindowController::GetAppletResourceUserId(kernel::Process* process,
                                                    u64* out_aruid) {
    *out_aruid = process->GetAppletState().GetAppletResourceUserId();
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::am
