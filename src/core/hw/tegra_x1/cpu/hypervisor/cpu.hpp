#pragma once

#include "core/hw/tegra_x1/cpu/cpu.hpp"
#include "core/hw/tegra_x1/cpu/hypervisor/pa_mapper.hpp"

namespace hydra::horizon {
class OS;
}

namespace hydra::hw::tegra_x1::cpu {
class IMmu;
}

namespace hydra::hw::tegra_x1::cpu::hypervisor {

class Mmu;
class Thread;

class Cpu : public ICpu {
  public:
    Cpu();
    ~Cpu();

    IMmu* CreateMmu() override;
    IThread* CreateThread(IMmu* mmu, IMemory* tls_mem) override;
    IMemory* AllocateMemory(usize size) override;

  private:
    PAMapper pa_mapper;

  public:
    CONST_REF_GETTER(pa_mapper, GetPAMapper);
};

} // namespace hydra::hw::tegra_x1::cpu::hypervisor
