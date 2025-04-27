#pragma once

#include "common/common.hpp"

namespace Hydra::Horizon::Services::Settings {

enum class SettingDataType {
    String,
    Integer,
    Boolean,
};

struct SettingValue {
    SettingDataType type;
    union {
        const char* s;
        i32 i;
        bool b;
    };

    static SettingValue String(const char* value) {
        return {SettingDataType::String, {.s = value}};
    }

    static SettingValue Integer(i32 value) {
        return {SettingDataType::Integer, {.i = value}};
    }

    static SettingValue Boolean(bool value) {
        return {SettingDataType::Boolean, {.b = value}};
    }
};

extern std::map<std::string, SettingValue> nx_settings;

} // namespace Hydra::Horizon::Services::Settings
