#pragma once

namespace hydra::horizon::services::settings {

enum class SettingDataType {
    String,
    Integer,
    Boolean,
};

struct SettingValue {
    SettingDataType type;
    union {
        std::string_view s;
        i32 i;
        bool b;
    };

    SettingValue(std::string_view value)
        : type(SettingDataType::String), s(value) {}
    SettingValue(i32 value) : type(SettingDataType::Integer), i(value) {}
    SettingValue(bool value) : type(SettingDataType::Boolean), b(value) {}
};

extern std::map<std::string, SettingValue> nx_settings;

const SettingValue* get_settings_value(const std::string& name,
                                       const std::string& item_key);

} // namespace hydra::horizon::services::settings
