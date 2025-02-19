#pragma once

#include "hw/tegra_x1/cpu/cpu_base.hpp"
#include "hw/tegra_x1/cpu/hypervisor/const.hpp"

namespace Hydra::Horizon {
class OS;
}

namespace Hydra::HW::TegraX1::CPU::Hypervisor {

class MMU;
class Thread;

class CPU : public CPUBase {
  public:
    CPU();
    ~CPU();

    void SetOS(Horizon::OS* os_) { os = os_; };

    ThreadBase* CreateThread() override;

    // Getters
    MMU* GetMMU() const { return mmu; }

  private:
    // TODO: use only handler functions
    Horizon::OS* os;

    MMU* mmu;
};

} // namespace Hydra::HW::TegraX1::CPU::Hypervisor
