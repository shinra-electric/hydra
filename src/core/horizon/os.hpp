#pragma once

#include "core/horizon/display_driver.hpp"
#include "core/horizon/input_manager.hpp"
#include "core/horizon/state_manager.hpp"

namespace Hydra::HW::Display {
class DisplayBase;
}

namespace Hydra::Horizon {

namespace Services::Sm {
class IUserInterface;
}

class OS {
  public:
    static OS& GetInstance();

    OS(HW::Bus& bus, HW::TegraX1::CPU::MMUBase* mmu_);
    ~OS();

    // Getters
    Kernel::Kernel& GetKernel() { return kernel; }
    StateManager& GetStateManager() { return state_manager; }
    DisplayDriver& GetDisplayDriver() { return display_driver; }
    InputManager& GetInputManager() { return input_manager; }

    bool IsInHandheldMode() const {
        // TODO: make this configurable
        return true;
    }

  private:
    HW::TegraX1::CPU::MMUBase* mmu;

    Kernel::Kernel kernel;

    // Services
    Services::Sm::IUserInterface* sm_user_interface;

    // Managers
    StateManager state_manager;
    DisplayDriver display_driver;
    InputManager input_manager;
};

} // namespace Hydra::Horizon
