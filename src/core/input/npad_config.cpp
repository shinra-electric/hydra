#include "core/input/npad_config.hpp"

#include "core/input/keyboard_base.hpp"

namespace hydra::input {

namespace {

u32 to_value(DeviceType type, const std::string_view value_str) {
    switch (type) {
    case DeviceType::Keyboard:
        return u32(to_key(value_str));
    case DeviceType::Cursor:
        LOG_ERROR(Input, "Cursor device does not support codes");
        return invalid<u32>();
    default:
        unreachable();
    }
}

std::string value_to_string(DeviceType device_type, u32 value) {
    switch (device_type) {
    case DeviceType::Keyboard:
        return fmt::format("{}", Key(value));
    case DeviceType::Cursor:
        LOG_ERROR(Input, "Cursor device does not support codes");
        return {};
    default:
        unreachable();
    }
}

Code to_code(const std::string_view str) {
    const auto slash_pos = str.find("/");
    if (slash_pos == std::string::npos) {
        LOG_ERROR(Input, "Invalid input code format: {}", str);
        return {};
    }

    // Device type
    const auto device_type_str = str.substr(0, slash_pos);
    const auto device_type = to_device_type(device_type_str);
    if (device_type == DeviceType::Invalid) {
        LOG_ERROR(Input, "Invalid device type: {}", device_type_str);
        return {};
    }

    // Value
    const auto value_str = str.substr(slash_pos + 1);
    const auto value = to_value(device_type, value_str);
    if (value == invalid<u32>()) {
        LOG_ERROR(Input, "Invalid value: {}", value_str);
        return {};
    }

    return Code(device_type, value);
}

} // namespace

} // namespace hydra::input

namespace toml {

template <>
struct from<hydra::input::Code> {
    template <typename TC>
    static hydra::input::Code from_toml(const basic_value<TC>& v) {
        const auto str = v.as_string();
        return to_code(str);
    }
};

template <>
struct into<hydra::input::Code> {
    template <typename TC>
    static basic_value<TC> into_toml(const hydra::input::Code& obj) {
        return fmt::format(
            "{}/{}", obj.GetDeviceType(),
            hydra::input::value_to_string(obj.GetDeviceType(), obj.GetValue()));
    }
};

} // namespace toml

namespace hydra::input {

NpadConfig::NpadConfig(horizon::hid::NpadIdType type_) : type{type_} {
    Deserialize();
}

void NpadConfig::LoadDefaults() {
    switch (type) {
    case horizon::hid::NpadIdType::Handheld: // TODO: No1 instead?
        // Devices
        device_names = {"keyboard"};

        // Buttons
        button_mappings = {
            {Code(DeviceType::Keyboard, Key::Enter),
             horizon::hid::NpadButtons::Plus},
            {Code(DeviceType::Keyboard, Key::Tab),
             horizon::hid::NpadButtons::Minus},
            {Code(DeviceType::Keyboard, Key::ArrowLeft),
             horizon::hid::NpadButtons::Left},
            {Code(DeviceType::Keyboard, Key::ArrowRight),
             horizon::hid::NpadButtons::Right},
            {Code(DeviceType::Keyboard, Key::ArrowUp),
             horizon::hid::NpadButtons::Up},
            {Code(DeviceType::Keyboard, Key::ArrowDown),
             horizon::hid::NpadButtons::Down},
            {Code(DeviceType::Keyboard, Key::L), horizon::hid::NpadButtons::A},
            {Code(DeviceType::Keyboard, Key::K), horizon::hid::NpadButtons::B},
            {Code(DeviceType::Keyboard, Key::I), horizon::hid::NpadButtons::X},
            {Code(DeviceType::Keyboard, Key::J), horizon::hid::NpadButtons::Y},
            {Code(DeviceType::Keyboard, Key::U), horizon::hid::NpadButtons::L},
            {Code(DeviceType::Keyboard, Key::O), horizon::hid::NpadButtons::R},
            {Code(DeviceType::Keyboard, Key::Y), horizon::hid::NpadButtons::ZL},
            {Code(DeviceType::Keyboard, Key::P), horizon::hid::NpadButtons::ZR},
        };

        // Analog sticks
        analog_mappings = {
            {Code(DeviceType::Keyboard, Key::A),
             {true, AnalogStickDirection::Left}},
            {Code(DeviceType::Keyboard, Key::D),
             {true, AnalogStickDirection::Right}},
            {Code(DeviceType::Keyboard, Key::W),
             {true, AnalogStickDirection::Up}},
            {Code(DeviceType::Keyboard, Key::S),
             {true, AnalogStickDirection::Down}},
        };

        break;
    default:
        break;
    }
}

void NpadConfig::Serialize() {
    // TODO
    LOG_FUNC_NOT_IMPLEMENTED(Input);
}

void NpadConfig::Deserialize() {
    const std::string path =
        fmt::format("{}/input_config/npads/{}.toml",
                    CONFIG_INSTANCE.GetAppDataPath(), type);

    // Check if exists
    bool exists = std::filesystem::exists(path);
    if (!exists) {
        LoadDefaults();
        Serialize();
        return;
    }

    // Buttons
    // TODO

    // Analog sticks
    // TODO
}

} // namespace hydra::input
