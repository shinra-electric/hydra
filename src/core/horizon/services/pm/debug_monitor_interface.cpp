#include "core/horizon/services/pm/debug_monitor_interface.hpp"

#include "core/horizon/kernel/process.hpp"

namespace hydra::horizon::services::pm {

// NOTE: the commands are offset by +1 on pre-5.0.0
DEFINE_SERVICE_COMMAND_TABLE(IDebugMonitorInterface, 2, GetProcessId, 4,
                             GetApplicationProcessId)

result_t IDebugMonitorInterface::GetProcessId(u64 program_id, u64* out_pid) {
    LOG_FUNC_WITH_ARGS_STUBBED(Services, "program ID: 0x{:016X}", program_id);

    // HACK
    *out_pid = 0x1029384756;
    return RESULT_SUCCESS;
}

result_t
IDebugMonitorInterface::GetApplicationProcessId(kernel::Process* process,
                                                u64* out_pid) {
    return GetProcessId(process->GetTitleID(), out_pid);
}

} // namespace hydra::horizon::services::pm
