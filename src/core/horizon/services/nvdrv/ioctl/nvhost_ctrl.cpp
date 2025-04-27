#include "core/horizon/services/nvdrv/ioctl/nvhost_ctrl.hpp"

#include "core/horizon/services/settings/nx_settings.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

DEFINE_IOCTL_TABLE(NvHostCtrl, DEFINE_IOCTL_TABLE_ENTRY(0x00, 0x1b, GetConfig,
                                                        0x1d, SyncptWaitEvent))

void NvHostCtrl::GetConfig(GetConfigData& data, NvResult& result) {
    LOG_FUNC_STUBBED(HorizonServices);

    // TODO: use a different connecting char?
    const auto key = to_lower(fmt::format("{}:{}", data.name, data.key));
    LOG_DEBUG(HorizonServices, "Key: {}", key);

    auto it = Settings::nx_settings.find(key);
    if (it == Settings::nx_settings.end()) {
        LOG_WARN(HorizonServices, "Key not found: {}", key);
        // TODO: return NvInternalResult::InvalidInput?
        result = NvResult::NotAvailableInProduction;
        return;
    }

    const auto& value = it->second;
    auto ptr = data.value.Get().data();
    switch (value.type) {
    case Settings::SettingDataType::String:
        std::strcpy(reinterpret_cast<char*>(ptr), value.s);
        break;
    case Settings::SettingDataType::Integer:
        *reinterpret_cast<i32*>(ptr) = value.i;
        break;
    case Settings::SettingDataType::Boolean:
        *reinterpret_cast<bool*>(ptr) = value.b;
        break;
    }
}

void NvHostCtrl::SyncptWaitEvent(SyncptWaitEventData& data, NvResult& result) {
    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    data.value = 0;
}

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
