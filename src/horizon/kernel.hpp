#pragma once

#include "common/allocators/static_pool.hpp"
#include "horizon/const.hpp"
#include "hw/tegra_x1/cpu/cpu.hpp"

namespace Hydra::HW::MMU {
class Memory;
class MMUBase;
} // namespace Hydra::HW::MMU

namespace Hydra::HW::CPU {
class CPUBase;
} // namespace Hydra::HW::CPU

namespace Hydra::Horizon {

namespace Services {
class ServiceBase;
} // namespace Services

class Kernel {
  public:
    Kernel();
    ~Kernel();

    void SetMMU(HW::MMU::MMUBase* mmu_);

    void LoadROM(Rom* rom);

    bool SupervisorCall(HW::CPU::CPUBase* cpu, u64 id);

    // SVCs
    Result svcSetHeapSize(uptr* out, usize size);
    Result svcSetMemoryPermission(uptr addr, usize size, Permission permission);
    Result svcSetMemoryAttribute(uptr addr, usize size, u32 mask, u32 value);
    Result svcQueryMemory(MemoryInfo* out_mem_info, u32* out_page_info,
                          uptr addr);
    void svcExitProcess();
    void svcSleepThread(i64 nano);
    Result svcMapSharedMemory(Handle handle, uptr addr, usize size,
                              Permission permission);
    Result svcCreateTransferMemory(Handle* out, uptr address, u64 size,
                                   Permission permission);
    Result svcCloseHandle(Handle handle);
    Result svcWaitSynchronization(u64& handle_index, Handle* handles_ptr,
                                  i32 handles_count, i64 timeout);
    Result svcArbitrateLock(u32 wait_tag, uptr mutex_addr, u32 self_tag);
    Result svcArbitrateUnlock(uptr mutex_addr);
    Result svcWaitProcessWideKeyAtomic(uptr mutex_addr, uptr var_addr,
                                       u32 self_tag, i64 timeout);
    Result svcConnectToNamedPort(Handle* out, const std::string& name);
    Result svcSendSyncRequest(Handle session_handle);
    Result svcBreak(BreakReason reason, uptr buffer_ptr, usize buffer_size);
    Result svcOutputDebugString(const char* str, usize len);
    Result svcGetInfo(u64* out, Info info);

    // Getters
    HW::MMU::Memory* GetRomMemory() { return rom_mem; }
    // HW::MMU::Memory* GetBssMemory() { return bss_mem; }
    HW::MMU::Memory* GetStackMemory() { return stack_mem; }
    HW::MMU::Memory* GetKernelMemory() { return kernel_mem; }
    HW::MMU::Memory* GetTlsMemory() { return tls_mem; }

    // Helpers
    Services::ServiceBase* GetService(Handle handle) const {
        return service_pool.GetObject(handle);
    }

    void SetService(Handle handle, Services::ServiceBase* service);

    Handle AddService(Services::ServiceBase* service);

  private:
    HW::MMU::MMUBase* mmu;

    // Memory

    // Static
    HW::MMU::Memory* stack_mem;
    HW::MMU::Memory* kernel_mem;
    HW::MMU::Memory* tls_mem;

    // Dynamic
    HW::MMU::Memory* rom_mem = nullptr;
    // HW::MMU::Memory* bss_mem;
    HW::MMU::Memory* heap_mem;

    // Services
    // TODO: what's the maximum number of services?
    Allocators::StaticPool<Services::ServiceBase*, 64> service_pool;
    u8 service_scratch_buffer[0x1000];
    u8 service_scratch_buffer_objects[0x100];
    u8 service_scratch_buffer_move_handles[0x100];
    u8 service_scratch_buffer_copy_handles[0x100];
};

} // namespace Hydra::Horizon
