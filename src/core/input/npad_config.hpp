#pragma once

#include "core/input/const.hpp"

namespace hydra::input {

class NpadConfig {
  public:
    NpadConfig(horizon::hid::NpadIdType type_);

    void Reset() {
        button_mappings = {};
        analog_mappings = {};
    }

    void LoadDefaults();

    void Serialize();
    void Deserialize();

  private:
    horizon::hid::NpadIdType type;

    std::vector<std::string> device_names;
    std::vector<CodeButtonMapping> button_mappings;
    std::vector<CodeAnalogMapping> analog_mappings;

    std::string GetNpadsPath() const {
        return fmt::format("{}/input_config", CONFIG_INSTANCE.GetAppDataPath());
    }

    std::string GetPath() const {
        return fmt::format("{}/{}.toml", GetNpadsPath(), type);
    }

  public:
    CONST_REF_GETTER(device_names, GetDeviceNames);
    CONST_REF_GETTER(button_mappings, GetButtonMappings);
    CONST_REF_GETTER(analog_mappings, GetAnalogMappings);
};

} // namespace hydra::input
