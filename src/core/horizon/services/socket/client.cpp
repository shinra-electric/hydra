#include "core/horizon/services/socket/client.hpp"

namespace Hydra::Horizon::Services::Socket {

DEFINE_SERVICE_COMMAND_TABLE(IClient, 0, RegisterClient, 1, StartMonitoring, 2,
                             Socket, 14, Connect, 26, Close)

void IClient::RegisterClient(REQUEST_COMMAND_PARAMS) {
    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    writers.writer.Write<u64>(0);
}

} // namespace Hydra::Horizon::Services::Socket
