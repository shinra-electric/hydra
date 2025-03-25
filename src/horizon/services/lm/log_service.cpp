#include "horizon/services/lm/log_service.hpp"

#include "horizon/services/lm/logger.hpp"

namespace Hydra::Horizon::Services::Lm {

DEFINE_SERVICE_COMMAND_TABLE(ILogService, 0, OpenLogger)

void ILogService::OpenLogger(REQUEST_COMMAND_PARAMS) {
    // TODO: what is this for?
    const u64 process_id = readers.reader.Read<u64>();
    LOG_DEBUG(HorizonServices, "Process ID: {}", process_id);

    add_service(new ILogger());
}

} // namespace Hydra::Horizon::Services::Lm
