#pragma once

#include "core/input/const.hpp"

namespace hydra::input {

class Profile {
  public:
    Profile(horizon::hid::NpadIdType type_, std::string_view name_);

    void Reset() {
        button_mappings = {};
        analog_mappings = {};
    }

    void LoadDefaults();

    void Serialize();
    void Deserialize();

  private:
    horizon::hid::NpadIdType type;
    std::string name;

    std::vector<std::string> device_names;
    std::vector<CodeButtonMapping> button_mappings;
    std::vector<CodeAnalogMapping> analog_mappings;

    // Helpers
    static std::string GetProfilesPath() {
        return fmt::format("{}/input_profiles",
                           CONFIG_INSTANCE.GetAppDataPath());
    }

    std::string GetPath() const {
        return fmt::format("{}/{}.toml", GetProfilesPath(), name);
    }

  public:
    CONST_REF_GETTER(device_names, GetDeviceNames);
    CONST_REF_GETTER(button_mappings, GetButtonMappings);
    CONST_REF_GETTER(analog_mappings, GetAnalogMappings);
};

} // namespace hydra::input
