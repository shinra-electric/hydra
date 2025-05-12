#include "core/horizon/services/spl/general_interface.hpp"

namespace Hydra::Horizon::Services::Spl {

DEFINE_SERVICE_COMMAND_TABLE(IGeneralInterface, 0, GetConfig)

result_t IGeneralInterface::GetConfig(ConfigItem item, u64* out_config) {
    LOG_DEBUG(HorizonServices, "Config item: {}", item);

    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    *out_config = 0;
    return RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::Spl
