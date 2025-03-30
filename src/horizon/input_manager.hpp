#pragma once

#include "horizon/const.hpp"

namespace Hydra::Horizon {

namespace HID {
struct SharedMemory;
}

class InputManager {
  public:
    InputManager();

    // Getters
    const HandleId GetSharedMemoryId() const { return shared_memory_id; }

  private:
    HandleId shared_memory_id;

    // Helpers
    HID::SharedMemory* GetHidSharedMemory() const;
};

} // namespace Hydra::Horizon
