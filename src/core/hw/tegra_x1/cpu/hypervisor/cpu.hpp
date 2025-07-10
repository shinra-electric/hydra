#pragma once

#include "core/hw/tegra_x1/cpu/cpu.hpp"
#include "core/hw/tegra_x1/cpu/hypervisor/memory.hpp"
#include "core/hw/tegra_x1/cpu/hypervisor/pa_mapper.hpp"
#include "core/hw/tegra_x1/cpu/hypervisor/page_table.hpp"

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

    Memory kernel_mem;
    PageTable kernel_page_table;

  public:
    CONST_REF_GETTER(pa_mapper, GetPAMapper);
    CONST_REF_GETTER(kernel_page_table, GetKernelPageTable);
};

} // namespace hydra::hw::tegra_x1::cpu::hypervisor
