#pragma once

#include "core/hw/tegra_x1/cpu/cpu.hpp"
#include "core/hw/tegra_x1/cpu/dynarmic/const.hpp"

namespace hydra::horizon {
class OS;
}

namespace hydra::hw::tegra_x1::cpu {
class IMmu;
}

namespace hydra::hw::tegra_x1::cpu::dynarmic {

class Mmu;
class Thread;

class Cpu : public ICpu {
  public:
    IMmu* CreateMmu() override;
    IThread* CreateThread(IMmu* mmu, IMemory* tls_mem) override;
    IMemory* AllocateMemory(usize size) override;
};

} // namespace hydra::hw::tegra_x1::cpu::dynarmic
