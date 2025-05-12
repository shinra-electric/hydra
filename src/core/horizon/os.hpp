#pragma once

#include "core/horizon/display_driver.hpp"
#include "core/horizon/input_manager.hpp"
#include "core/horizon/services/account/user_manager.hpp"
#include "core/horizon/state_manager.hpp"

namespace hydra::hw::display {
class DisplayBase;
}

namespace hydra::horizon {

namespace services::sm {
class IUserInterface;
}

class OS {
  public:
    static OS& GetInstance();

    OS(hw::Bus& bus, hw::tegra_x1::cpu::MMUBase* mmu_);
    ~OS();

    // Getters
    kernel::Kernel& GetKernel() { return kernel; }
    StateManager& GetStateManager() { return state_manager; }
    DisplayDriver& GetDisplayDriver() { return display_driver; }
    InputManager& GetInputManager() { return input_manager; }

    bool IsInHandheldMode() const {
        // TODO: make this configurable
        return true;
    }

  private:
    hw::tegra_x1::cpu::MMUBase* mmu;

    kernel::Kernel kernel;

    // Services
    services::sm::IUserInterface* sm_user_interface;

    // Managers
    StateManager state_manager;
    DisplayDriver display_driver;
    InputManager input_manager;
    services::account::UserManager user_manager;
};

} // namespace hydra::horizon
