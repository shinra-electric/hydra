#include "core/horizon/services/am/session.hpp"

namespace Hydra::Horizon::Services::Am {

DEFINE_SERVICE_COMMAND_TABLE(ISession, 0, SetPerformanceConfiguration)

result_t ISession::SetPerformanceConfiguration(PerformanceMode mode, u32 config) {
    LOG_DEBUG(HorizonServices, "Mode: {}, configuration: 0x{:08x}", mode,
              config);

    LOG_FUNC_STUBBED(HorizonServices);
    return RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::Am
