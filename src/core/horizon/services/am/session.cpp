#include "core/horizon/services/am/session.hpp"

namespace Hydra::Horizon::Services::Am {

enum class PerformanceMode : i32 {
    Invalid = -1,
    Normal = 0,
    Boost = 1,
};

struct SetPerformanceConfigurationIn {
    PerformanceMode mode;
    u32 config;
};

} // namespace Hydra::Horizon::Services::Am

ENABLE_ENUM_FORMATTING(Hydra::Horizon::Services::Am::PerformanceMode, Invalid,
                       "invalid", Normal, "normal", Boost, "boost");

namespace Hydra::Horizon::Services::Am {

DEFINE_SERVICE_COMMAND_TABLE(ISession, 0, SetPerformanceConfiguration)

void ISession::SetPerformanceConfiguration(REQUEST_COMMAND_PARAMS) {
    const auto in = readers.reader.Read<SetPerformanceConfigurationIn>();
    LOG_DEBUG(HorizonServices, "Mode: {}, configuration: 0x{:08x}", in.mode,
              in.config);

    LOG_FUNC_STUBBED(HorizonServices);
}

} // namespace Hydra::Horizon::Services::Am
