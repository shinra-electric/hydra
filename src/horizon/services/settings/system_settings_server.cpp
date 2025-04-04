#include "horizon/services/settings/system_settings_server.hpp"

namespace Hydra::Horizon::Services::Settings {

DEFINE_SERVICE_COMMAND_TABLE(ISystemSettingsServer, 3, GetFirmwareVersion)

void ISystemSettingsServer::GetFirmwareVersion(REQUEST_COMMAND_PARAMS) {
    writers.recv_list_writers[0].Write(FIRMWARE_VERSION);
}

} // namespace Hydra::Horizon::Services::Settings
