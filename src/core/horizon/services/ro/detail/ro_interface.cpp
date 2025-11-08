#include "core/horizon/services/ro/detail/ro_interface.hpp"

#include "core/horizon/kernel/process.hpp"
#include "core/hw/tegra_x1/cpu/mmu.hpp"

namespace hydra::horizon::services::ro::detail {

DEFINE_SERVICE_COMMAND_TABLE(IRoInterface, 0, MapManualLoadModuleMemory, 2,
                             RegisterModuleInfo, 4, RegisterProcessHandle)

result_t IRoInterface::MapManualLoadModuleMemory(kernel::Process* process,
                                                 u64 pid_reserved, u64 nro_addr,
                                                 u64 nro_size, u64 bss_addr,
                                                 u64 bss_size, u64* out_addr) {
    LOG_FUNC_NOT_IMPLEMENTED(Services);
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::ro::detail
