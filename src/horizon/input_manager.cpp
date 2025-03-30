#include "horizon/input_manager.hpp"

#include "horizon/hid.hpp"
#include "horizon/kernel.hpp"

#define GET_NPAD_INTERNAL_STATE(type)                                          \
    GetHidSharedMemory()->npad.entries[u32(type)].internal_state

#define SET_NPAD_ENTRY(style_upper, style_lower, entry_dst, entry_src)         \
    if (any(internal_state.style_set & HID::NpadStyleSet::style_upper)) {      \
        internal_state.style_lower##_lifo.storage[0].state.entry_dst =         \
            entry_src;                                                         \
    }

#define SET_NPAD_ENTRIES_SEPARATE(entry_dst, entry_src)                        \
    SET_NPAD_ENTRY(FullKey, full_key, entry_dst, entry_src);                   \
    SET_NPAD_ENTRY(Handheld, handheld, entry_dst, entry_src);                  \
    SET_NPAD_ENTRY(JoyDual, joy_dual, entry_dst, entry_src);                   \
    SET_NPAD_ENTRY(JoyLeft, joy_left, entry_dst, entry_src);                   \
    SET_NPAD_ENTRY(JoyRight, joy_right, entry_dst, entry_src);

#define SET_NPAD_ENTRIES(entry) SET_NPAD_ENTRIES_SEPARATE(entry, entry)

namespace Hydra::Horizon {

InputManager::InputManager() {
    shared_memory_id = Kernel::GetInstance().CreateSharedMemory(0x40000);
}

void InputManager::ConnectNpad(HID::NpadIdType type,
                               HID::NpadStyleSet style_set,
                               HID::NpadAttributes attributes) {
    auto& internal_state = GET_NPAD_INTERNAL_STATE(type);
    internal_state.style_set = style_set;
    SET_NPAD_ENTRIES(attributes);
}

void InputManager::SetNpadButtons(HID::NpadIdType type,
                                  HID::NpadButtons buttons) {
    auto& internal_state = GET_NPAD_INTERNAL_STATE(type);
    SET_NPAD_ENTRIES(buttons);
}

void InputManager::SetNpadAnalogStickStateL(
    HID::NpadIdType type, HID::AnalogStickState analog_stick) {
    auto& internal_state = GET_NPAD_INTERNAL_STATE(type);
    SET_NPAD_ENTRIES_SEPARATE(analog_stick_l, analog_stick);
}

void InputManager::SetNpadAnalogStickStateR(
    HID::NpadIdType type, HID::AnalogStickState analog_stick) {
    auto& internal_state = GET_NPAD_INTERNAL_STATE(type);
    SET_NPAD_ENTRIES_SEPARATE(analog_stick_r, analog_stick);
}

HID::SharedMemory* InputManager::GetHidSharedMemory() const {
    return reinterpret_cast<HID::SharedMemory*>(
        Kernel::GetInstance().GetSharedMemory(shared_memory_id).GetPtr());
}

} // namespace Hydra::Horizon
