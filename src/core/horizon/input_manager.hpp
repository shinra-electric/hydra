#pragma once

#include "core/horizon/const.hpp"
#include "core/horizon/hid.hpp"

namespace hydra::horizon {

namespace HID {
struct SharedMemory;
}

class InputManager {
  public:
    InputManager();

    // Device connection
    void ConnectNpad(hid::NpadIdType type, hid::NpadStyleSet style_set,
                     hid::NpadAttributes attributes);

    // Events

    // Npad
    void SetNpadButtons(hid::NpadIdType type, hid::NpadButtons buttons);
    void SetNpadAnalogStickStateL(hid::NpadIdType type,
                                  hid::AnalogStickState analog_stick);
    void SetNpadAnalogStickStateR(hid::NpadIdType type,
                                  hid::AnalogStickState analog_stick);

    // Touch
    void UpdateTouchStates();
    u32 BeginTouch();
    void SetTouchState(hid::TouchState state);
    void EndTouch(u32 finger_id);

    // Getters
    const handle_id_t GetSharedMemoryId() const { return shared_memory_id; }

  private:
    handle_id_t shared_memory_id;

    // State
    usize touch_count{0};
    u16 available_finger_mask{0xffff};

    // Helpers
    hid::SharedMemory* GetHidSharedMemory() const;
};

} // namespace hydra::horizon
