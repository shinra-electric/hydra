#pragma once

#include "core/horizon/const.hpp"
#include "core/horizon/hid.hpp"

namespace Hydra::Horizon {

namespace HID {
struct SharedMemory;
}

class InputManager {
  public:
    InputManager();

    // Device connection
    void ConnectNpad(HID::NpadIdType type, HID::NpadStyleSet style_set,
                     HID::NpadAttributes attributes);

    // Events
    void SetNpadButtons(HID::NpadIdType type, HID::NpadButtons buttons);
    void SetNpadAnalogStickStateL(HID::NpadIdType type,
                                  HID::AnalogStickState analog_stick);
    void SetNpadAnalogStickStateR(HID::NpadIdType type,
                                  HID::AnalogStickState analog_stick);

    // Getters
    const handle_id_t GetSharedMemoryId() const { return shared_memory_id; }

  private:
    handle_id_t shared_memory_id;

    // Helpers
    HID::SharedMemory* GetHidSharedMemory() const;
};

} // namespace Hydra::Horizon
