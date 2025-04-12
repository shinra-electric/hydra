#include "core/horizon/services/lm/logger.hpp"

namespace Hydra::Horizon::Services::Lm {

DEFINE_SERVICE_COMMAND_TABLE(ILogger, 0, Log)

void ILogger::Log(REQUEST_COMMAND_PARAMS) {
    const auto msg = readers.send_buffers_readers[0].ReadString();
    LOG_INFO(HorizonServices, "Log: {}", msg);
}

} // namespace Hydra::Horizon::Services::Lm
