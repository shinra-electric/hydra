#pragma once

#include "horizon/const.hpp"
#include "hw/cpu/cpu.hpp"

namespace Hydra::HW::MMU {

class Memory;
class MMUBase;

} // namespace Hydra::HW::MMU

namespace Hydra::HW::CPU {

class CPUBase;

} // namespace Hydra::HW::CPU

namespace Hydra::Horizon {

class Kernel {
  public:
    Kernel();
    ~Kernel();

    void SetMMU(HW::MMU::MMUBase* mmu_) { mmu = mmu_; }

    void LoadROM(Rom* rom);

    bool SupervisorCall(HW::CPU::CPUBase* cpu, u64 id);

    // SVCs
    Result svcSetHeapSize(uptr* out, usize size);
    Result svcSetMemoryPermission(uptr addr, usize size, Permission permission);
    Result svcSetMemoryAttribute(uptr addr, usize size, u32 mask, u32 value);
    Result svcQueryMemory(MemoryInfo* out_mem_info, u32* out_page_info,
                          uptr addr);
    void svcExitProcess();
    Result svcMapSharedMemory(Handle handle, uptr addr, usize size,
                              Permission permission);
    Result svcCloseHandle(Handle handle);
    Result svcArbitrateLock(u32 wait_tag, uptr mutex_addr, u32 self_tag);
    Result svcArbitrateUnlock(uptr mutex_addr);
    Result svcWaitProcessWideKeyAtomic(uptr mutex_addr, uptr var_addr,
                                       u32 self_tag, i64 timeout);
    Result svcConnectToNamedPort(Handle* out, const char* name);
    Result svcSendSyncRequest(Handle session_handle);
    Result svcBreak(BreakReason reason, uptr buffer_ptr, usize buffer_size);
    Result svcOutputDebugString(const char* str, usize len);
    Result svcGetInfo(u64* out, Info info);

    // Getters
    HW::MMU::Memory* GetRomMemory() { return rom_mem; }
    HW::MMU::Memory* GetBssMemory() { return bss_mem; }
    HW::MMU::Memory* GetStackMemory() { return stack_mem; }
    HW::MMU::Memory* GetKernelMemory() { return kernel_mem; }
    HW::MMU::Memory* GetTlsMemory() { return tls_mem; }

  private:
    HW::MMU::MMUBase* mmu;

    // Memory

    // ROM
    HW::MMU::Memory* rom_mem;
    HW::MMU::Memory* bss_mem;

    // Custom
    HW::MMU::Memory* stack_mem;
    HW::MMU::Memory* kernel_mem;
    HW::MMU::Memory* tls_mem;
};

} // namespace Hydra::Horizon
