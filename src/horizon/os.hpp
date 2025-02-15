#pragma once

#include "horizon/kernel.hpp"

namespace Hydra::HW::MMU {
class MMUBase;
}

namespace Hydra::HW::Display {
class DisplayBase;
}

namespace Hydra::Horizon {

namespace Services::HosBinder {
class IHOSBinderDriver;
}

class OS {
  public:
    static OS& GetInstance();

    OS(HW::Bus& bus);
    ~OS();

    void SetMMU(HW::MMU::MMUBase* mmu);

    // Getters
    Kernel& GetKernel() { return kernel; }

    Services::HosBinder::IHOSBinderDriver* GetHosBinderDriver() {
        return hos_binder_driver;
    }

  private:
    Kernel kernel;

    // Services
    Services::HosBinder::IHOSBinderDriver* hos_binder_driver;
};

} // namespace Hydra::Horizon
