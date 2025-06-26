#include "core/horizon/services/settings/system_settings_server.hpp"

#include "core/horizon/const.hpp"
#include "core/horizon/services/settings/nx_settings.hpp"

namespace hydra::horizon::services::settings {

namespace {

struct DeviceNickName {
    char name[0x80];
};

} // namespace

DEFINE_SERVICE_COMMAND_TABLE(ISystemSettingsServer, 3, GetFirmwareVersion, 23,
                             GetColorSetId, 38, GetSettingsItemValue, 77,
                             GetDeviceNickName)

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

result_t ISystemSettingsServer::GetSettingsItemValue(
    InBuffer<BufferAttr::HipcPointer> in_name_buffer,
    InBuffer<BufferAttr::HipcPointer> in_item_key_buffer, u64* out_size,
    OutBuffer<BufferAttr::MapAlias> out_buffer) {
    auto name = in_name_buffer.reader->ReadString();
    auto item_key = in_item_key_buffer.reader->ReadString();
    auto key = fmt::format("{}!{}", name, item_key);

    auto it = settings::nx_settings.find(key);
    if (it == settings::nx_settings.end()) {
        LOG_WARN(Services, "Key not found: {}", key);
        // TODO: error
        return RESULT_SUCCESS;
    }

    const auto& value = it->second;
    switch (value.type) {
    case settings::SettingDataType::String:
        out_buffer.writer->WritePtr(value.s.data(), value.s.size());
        *out_size = value.s.size();
        break;
    case settings::SettingDataType::Integer:
        out_buffer.writer->Write(value.i);
        *out_size = sizeof(value.i);
        break;
    case settings::SettingDataType::Boolean:
        out_buffer.writer->Write(value.b);
        *out_size = sizeof(value.b);
        break;
    }

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
