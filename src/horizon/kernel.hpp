#pragma once

#include "common/allocators/dynamic_pool.hpp"
#include "common/allocators/static_pool.hpp"
#include "horizon/const.hpp"
#include "horizon/filesystem/filesystem.hpp"

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

class KernelHandle {
  public:
    virtual ~KernelHandle() = default;
};

class SharedMemory {
  public:
    SharedMemory() = default;

    void MapToRange(const range<uptr> range_) { range = range_; }

    // Getters
    const range<uptr> GetRange() const { return range; }

  private:
    range<uptr> range;
};

class TransferMemory : public KernelHandle {
  public:
    TransferMemory(uptr addr_, u64 size_, Permission permission_)
        : addr{addr_}, size{size_}, permission{permission_} {}

  private:
    uptr addr;
    u64 size;
    Permission permission;
};

constexpr usize ARG_COUNT = 2;

class Kernel {
  public:
    static Kernel& GetInstance();

    Kernel(HW::Bus& bus_, HW::TegraX1::CPU::MMUBase* mmu_);
    ~Kernel();

    void ConfigureThread(HW::TegraX1::CPU::ThreadBase* thread);
    void ConfigureMainThread(HW::TegraX1::CPU::ThreadBase* thread);

    // Loading
    HW::TegraX1::CPU::Memory* CreateExecutableMemory(usize size,
                                                     uptr& out_base);
    void SetEntryPoint(uptr entry_point_) { entry_point = entry_point_; }
    void SetArg(u32 index, u64 value) {
        ASSERT_DEBUG(index < ARG_COUNT, HorizonKernel, "Invalid arg index {}",
                     index);
        args[index] = value;
    }

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
    Result svcMapSharedMemory(HandleId handle_id, uptr addr, usize size,
                              Permission permission);
    Result svcCreateTransferMemory(HandleId* out_handle_id, uptr addr, u64 size,
                                   Permission permission);
    Result svcCloseHandle(HandleId handle_id);
    Result svcResetSignal(HandleId handle_id);
    Result svcWaitSynchronization(u64& handle_index, HandleId* handle_ids,
                                  i32 handles_count, i64 timeout);
    Result svcArbitrateLock(u32 wait_tag, uptr mutex_addr, u32 self_tag);
    Result svcArbitrateUnlock(uptr mutex_addr);
    Result svcWaitProcessWideKeyAtomic(uptr mutex_addr, uptr var_addr,
                                       u32 self_tag, i64 timeout);
    Result svcSignalProcessWideKey(uptr addr, i32 v);
    Result svcConnectToNamedPort(HandleId* out_handle_id,
                                 const std::string& name);
    Result svcSendSyncRequest(HandleId handle_id);
    Result svcBreak(BreakReason reason, uptr buffer_ptr, usize buffer_size);
    Result svcOutputDebugString(const char* str, usize len);
    Result svcGetInfo(u64* out, InfoType info_type, HandleId handle_id,
                      u64 info_sub_type);

    // Getters
    HW::Bus& GetBus() const { return bus; }

    Filesystem::Filesystem& GetFilesystem() { return filesystem; }

    HW::TegraX1::CPU::Memory* GetStackMemory() const { return stack_mem; }
    HW::TegraX1::CPU::Memory* GetKernelMemory() const { return kernel_mem; }
    HW::TegraX1::CPU::Memory* GetTlsMemory() const { return tls_mem; }

    // Helpers
    KernelHandle* GetHandle(HandleId handle_id) const {
        return handle_pool.GetObject(handle_id);
    }

    void SetHandle(HandleId handle_id, KernelHandle* handle);

    HandleId AddHandle(KernelHandle* handle);

    HandleId CreateSharedMemory();

    const SharedMemory GetSharedMemory(HandleId handle_id) const {
        return shared_memory_pool.GetObject(handle_id);
    }

  private:
    HW::Bus& bus;
    HW::TegraX1::CPU::MMUBase* mmu;

    uptr entry_point{0x0};
    u64 args[ARG_COUNT] = {0x0};

    Filesystem::Filesystem filesystem;

    // Memory

    // Static
    HW::TegraX1::CPU::Memory* stack_mem;
    HW::TegraX1::CPU::Memory* kernel_mem;
    HW::TegraX1::CPU::Memory* tls_mem;
    HW::TegraX1::CPU::Memory* aslr_mem;

    // Dynamic
    uptr executable_mem_base = 0x80000000;
    std::vector<HW::TegraX1::CPU::Memory*> executable_memories;
    // HW::MMU::Memory* bss_mem;
    HW::TegraX1::CPU::Memory* heap_mem;

    // Handles
    Allocators::DynamicPool<KernelHandle*> handle_pool;
    Allocators::DynamicPool<SharedMemory> shared_memory_pool;

    // Services
    std::map<std::string, Services::ServiceBase*> service_ports;
    u8 service_scratch_buffer[0x200];
    u8 service_scratch_buffer_objects[0x100];
    u8 service_scratch_buffer_move_handles[0x100];
    u8 service_scratch_buffer_copy_handles[0x100];
};

} // namespace Hydra::Horizon
