#pragma once

#include "common/allocators/dynamic_pool.hpp"
#include "common/allocators/static_pool.hpp"
#include "horizon/const.hpp"

namespace Hydra::HW::TegraX1::CPU {
class Memory;
class MMUBase;
class ThreadBase;
} // namespace Hydra::HW::TegraX1::CPU

namespace Hydra::HW {
class Bus;
}

namespace Hydra::Horizon {

namespace Services {
class ServiceBase;
}

class SharedMemory {
  public:
    SharedMemory() = default;

    void MapToRange(range<uptr> range) { ranges.Allocate() = range; }

    void UnmapFromRange(uptr base) {
        // TODO: implement
        LOG_WARNING(HorizonKernel, "Not implemented");
    }

  private:
    // TODO: use dynamic pool?
    Allocators::StaticPool<range<uptr>, 8> ranges;
};

class Kernel {
  public:
    static Kernel& GetInstance();

    Kernel(HW::Bus& bus_, HW::TegraX1::CPU::MMUBase* mmu_);
    ~Kernel();

    void ConfigureThread(HW::TegraX1::CPU::ThreadBase* thread);
    void ConfigureMainThread(HW::TegraX1::CPU::ThreadBase* thread);

    void LoadROM(Rom* rom);

    void ConnectServiceToPort(const std::string& port_name,
                              Services::ServiceBase* service) {
        service_ports[port_name] = service;
    }

    bool SupervisorCall(HW::TegraX1::CPU::ThreadBase* thread, u64 id);

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
    Result svcSignalProcessWideKey(uptr addr, i32 v);
    Result svcConnectToNamedPort(Handle* out, const std::string& name);
    Result svcSendSyncRequest(Handle session_handle);
    Result svcBreak(BreakReason reason, uptr buffer_ptr, usize buffer_size);
    Result svcOutputDebugString(const char* str, usize len);
    Result svcGetInfo(u64* out, InfoType info_type, Handle handle,
                      u64 info_sub_type);

    // Getters
    HW::Bus& GetBus() const { return bus; }

    HW::TegraX1::CPU::Memory* GetRomMemory() const { return rom_mem; }
    // HW::MMU::Memory* GetBssMemory() const { return bss_mem; }
    HW::TegraX1::CPU::Memory* GetStackMemory() const { return stack_mem; }
    HW::TegraX1::CPU::Memory* GetKernelMemory() const { return kernel_mem; }
    HW::TegraX1::CPU::Memory* GetTlsMemory() const { return tls_mem; }

    // Helpers
    Services::ServiceBase* GetService(Handle handle) const {
        return service_pool.GetObject(handle);
    }

    void SetService(Handle handle, Services::ServiceBase* service);

    Handle AddService(Services::ServiceBase* service);

    Handle CreateSharedMemory();

  private:
    HW::Bus& bus;
    HW::TegraX1::CPU::MMUBase* mmu;

    u32 rom_text_offset{0};

    // Memory

    // Static
    HW::TegraX1::CPU::Memory* stack_mem;
    HW::TegraX1::CPU::Memory* kernel_mem;
    HW::TegraX1::CPU::Memory* tls_mem;
    HW::TegraX1::CPU::Memory* aslr_mem;

    // Dynamic
    HW::TegraX1::CPU::Memory* rom_mem = nullptr;
    // HW::MMU::Memory* bss_mem;
    HW::TegraX1::CPU::Memory* heap_mem;

    // Shared
    Allocators::DynamicPool<SharedMemory> shared_memories;

    // Services
    std::map<std::string, Services::ServiceBase*> service_ports;
    // TODO: what's the maximum number of services?
    Allocators::StaticPool<Services::ServiceBase*, 64> service_pool;
    u8 service_scratch_buffer[0x200];
    u8 service_scratch_buffer_objects[0x100];
    u8 service_scratch_buffer_move_handles[0x100];
    u8 service_scratch_buffer_copy_handles[0x100];
};

} // namespace Hydra::Horizon
