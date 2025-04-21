#include "core/horizon/services/settings/system_settings_server.hpp"

#include "core/horizon/const.hpp"

namespace Hydra::Horizon::Services::Settings {

namespace {

enum class ColorSetId : i32 {
    BasicWhite,
    BasicBlack,
};

}

DEFINE_SERVICE_COMMAND_TABLE(ISystemSettingsServer, 3, GetFirmwareVersion, 23,
                             GetColorSetId)

void ISystemSettingsServer::GetFirmwareVersion(REQUEST_COMMAND_PARAMS) {
    writers.recv_list_writers[0].Write(FIRMWARE_VERSION);
}

void ISystemSettingsServer::GetColorSetId(REQUEST_COMMAND_PARAMS) {
    // TODO: make this configurable
    writers.writer.Write(ColorSetId::BasicWhite);
}

} // namespace Hydra::Horizon::Services::Settings
