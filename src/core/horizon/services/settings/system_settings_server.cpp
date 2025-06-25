#include "core/horizon/services/settings/system_settings_server.hpp"

#include "core/horizon/const.hpp"

namespace hydra::horizon::services::settings {

namespace {

struct DeviceNickName {
    char name[0x80];
};

} // namespace

DEFINE_SERVICE_COMMAND_TABLE(ISystemSettingsServer, 3, GetFirmwareVersion, 23,
                             GetColorSetId, 77, GetDeviceNickName)

result_t ISystemSettingsServer::GetFirmwareVersion(
    OutBuffer<BufferAttr::HipcPointer> out_buffer) {
    out_buffer.writer->Write(FIRMWARE_VERSION);
    return RESULT_SUCCESS;
}

result_t ISystemSettingsServer::GetColorSetId(ColorSetId* out_id) {
    // TODO: make this configurable
    *out_id = ColorSetId::BasicWhite;
    return RESULT_SUCCESS;
}

result_t ISystemSettingsServer::GetDeviceNickName(
    OutBuffer<BufferAttr::MapAlias> out_buffer) {
    // TODO: make this configurable
    out_buffer.writer->Write<DeviceNickName>({
        .name = "Hydra device",
    });
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::settings
