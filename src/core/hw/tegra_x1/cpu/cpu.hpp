#pragma once

#define CPU_INSTANCE hw::tegra_x1::cpu::ICpu::GetInstance()

namespace hydra::hw::tegra_x1::cpu {

class IMemory;
class IMmu;
class IThread;

class ICpu {
  public:
    static ICpu& GetInstance();

    ICpu();
    virtual ~ICpu();

    virtual IMmu* CreateMmu() = 0;
    virtual IThread* CreateThread(IMmu* mmu, IMemory* tls_mem) = 0;
    virtual IMemory* AllocateMemory(usize size) = 0;
};

} // namespace hydra::hw::tegra_x1::cpu
