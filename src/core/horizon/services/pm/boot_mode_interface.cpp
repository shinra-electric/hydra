#include "core/horizon/services/pm/boot_mode_interface.hpp"

namespace hydra::horizon::services::pm {

DEFINE_SERVICE_COMMAND_TABLE(IBootModeInterface, 0, GetBootMode)

result_t IBootModeInterface::GetBootMode(BootMode* out_mode) {
    *out_mode = BootMode::Normal;
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::pm
