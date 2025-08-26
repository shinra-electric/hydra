#include "core/input/npad_config.hpp"

#include "core/input/keyboard_base.hpp"

namespace hydra::input {

namespace {

struct CodeWithDevice {
    std::string device_name;
    Code code;

    CodeWithDevice() {}
    CodeWithDevice(const std::string_view device_name_, DeviceType device_type_,
                   u32 value_)
        : device_name{device_name_}, code(device_type_, value_) {}

    const std::string_view GetDeviceName() const { return device_name; }

    CONST_REF_GETTER(code, GetCode);
};

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

CodeWithDevice to_code(const std::string_view str) {
    const auto first_slash_pos = str.find("/");
    if (first_slash_pos == std::string::npos) {
        LOG_ERROR(Input, "Invalid input code format: {}", str);
        return {};
    }
    const auto second_slash_pos = str.find("/", first_slash_pos + 1);
    if (second_slash_pos == std::string::npos) {
        LOG_ERROR(Input, "Invalid input code format: {}", str);
        return {};
    }

    // Device name
    const auto device_name = str.substr(0, first_slash_pos);

    // Device type
    const auto device_type_str =
        str.substr(first_slash_pos + 1, second_slash_pos);
    const auto device_type = to_device_type(device_type_str);
    if (device_type == DeviceType::Invalid) {
        LOG_ERROR(Input, "Invalid device type: {}", device_type_str);
        return {};
    }

    // Value
    const auto value_str = str.substr(second_slash_pos + 1);
    const auto value = to_value(device_type, value_str);
    if (value == invalid<u32>()) {
        LOG_ERROR(Input, "Invalid value: {}", value_str);
        return {};
    }

    return CodeWithDevice(device_name, device_type, value);
}

} // namespace

} // namespace hydra::input

namespace toml {

template <>
struct from<hydra::input::CodeWithDevice> {
    template <typename TC>
    static hydra::input::CodeWithDevice from_toml(const basic_value<TC>& v) {
        const auto str = v.as_string();
        return to_code(str);
    }
};

template <>
struct into<hydra::input::CodeWithDevice> {
    template <typename TC>
    static basic_value<TC> into_toml(const hydra::input::CodeWithDevice& obj) {
        return fmt::format(
            "{}/{}/{}", obj.GetDeviceName(), obj.GetCode().GetDeviceType(),
            hydra::input::value_to_string(obj.GetCode().GetDeviceType(),
                                          obj.GetCode().GetValue()));
    }
};

} // namespace toml

namespace hydra::input {

NpadConfig::NpadConfig(horizon::hid::NpadIdType type_) : type{type_} {
    // TODO: toml

    // Buttons
    // HACK
    button_mappings["keyboard"] = {
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
    // TODO
    /*
    const auto analog_l_neg_x = Code(DeviceType::Keyboard, Key::A);
    const auto analog_l_pos_x = Code(DeviceType::Keyboard, Key::D);
    const auto analog_l_pos_y = Code(DeviceType::Keyboard, Key::W);
    const auto analog_l_neg_y = Code(DeviceType::Keyboard, Key::S);
    */
}

} // namespace hydra::input
