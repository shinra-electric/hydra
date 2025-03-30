#pragma once

#include "common/allocators/dynamic_pool.hpp"
#include "common/allocators/static_pool.hpp"
#include "horizon/filesystem/filesystem.hpp"
#include "horizon/shared_memory.hpp"

namespace Hydra::HW::TegraX1::CPU {
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

class TransferMemory : public KernelHandle {
  public:
    TransferMemory(uptr addr_, u64 size_, MemoryPermission perm_)
        : addr{addr_}, size{size_}, perm{perm_} {}

  private:
    uptr addr;
    u64 size;
    MemoryPermission perm;
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
    uptr CreateExecutableMemory(usize size, vaddr& out_base);
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
    Result svcSetHeapSize(usize size, uptr& out_base);
    Result svcSetMemoryPermission(uptr addr, usize size, MemoryPermission perm);
    Result svcSetMemoryAttribute(uptr addr, usize size, u32 mask, u32 value);
    Result svcMapMemory(uptr dst_addr, uptr src_addr, usize size);
    Result svcUnmapMemory(uptr dst_addr, uptr src_addr, usize size);
    Result svcQueryMemory(uptr addr, MemoryInfo& out_mem_info,
                          u32& out_page_info);
    void svcExitProcess();
    void svcSleepThread(i64 nano);
    Result svcGetThreadPriority(HandleId thread_handle_id, u32& out_priority);
    Result svcMapSharedMemory(HandleId shared_mem_handle_id, uptr addr,
                              usize size, MemoryPermission perm);
    Result svcUnmapSharedMemory(HandleId shared_mem_handle_id, uptr addr,
                                usize size);
    Result svcCreateTransferMemory(uptr addr, u64 size, MemoryPermission perm,
                                   HandleId& out_transfer_mem_handle_id);
    Result svcCloseHandle(HandleId handle_id);
    Result svcResetSignal(HandleId handle_id);
    Result svcWaitSynchronization(HandleId* handle_ids, i32 handles_count,
                                  i64 timeout, u64& out_handle_index);
    Result svcArbitrateLock(u32 wait_tag, uptr mutex_addr, u32 self_tag);
    Result svcArbitrateUnlock(uptr mutex_addr);
    Result svcWaitProcessWideKeyAtomic(uptr mutex_addr, uptr var_addr,
                                       u32 self_tag, i64 timeout);
    Result svcSignalProcessWideKey(uptr addr, i32 v);
    void svcGetSystemTick(u64& out_tick);
    Result svcConnectToNamedPort(const std::string& name,
                                 HandleId& out_session_handle_id);
    Result svcSendSyncRequest(HandleId session_handle_id);
    Result svcGetThreadId(HandleId thread_handle_id, u64& out_thread_id);
    Result svcBreak(BreakReason reason, uptr buffer_ptr, usize buffer_size);
    Result svcOutputDebugString(const char* str, usize len);
    Result svcGetInfo(InfoType info_type, HandleId handle_id, u64 info_sub_type,
                      u64& out_info);

    // Getters
    HW::Bus& GetBus() const { return bus; }

    Filesystem::Filesystem& GetFilesystem() { return filesystem; }

    // Helpers
    KernelHandle* GetHandle(HandleId handle_id) const {
        return handle_pool.GetObject(handle_id);
    }

    void SetHandle(HandleId handle_id, KernelHandle* handle);

    HandleId AddHandle(KernelHandle* handle);

    HandleId CreateSharedMemory(usize size);

    const SharedMemory& GetSharedMemory(HandleId handle_id) const {
        return *shared_memory_pool.GetObject(handle_id);
    }

  private:
    HW::Bus& bus;
    HW::TegraX1::CPU::MMUBase* mmu;

    uptr entry_point{0x0};
    u64 args[ARG_COUNT] = {0x0};

    Filesystem::Filesystem filesystem;

    // Memory
    HW::TegraX1::CPU::MemoryBase* stack_mem;
    HW::TegraX1::CPU::MemoryBase* tls_mem;
    HW::TegraX1::CPU::MemoryBase* heap_mem;
    std::vector<HW::TegraX1::CPU::MemoryBase*> executable_mems;

    vaddr executable_mem_base{0x80000000};

    // Handles
    Allocators::DynamicPool<KernelHandle*> handle_pool;
    Allocators::DynamicPool<SharedMemory*> shared_memory_pool;

    // Services
    std::map<std::string, Services::ServiceBase*> service_ports;
    u8 service_scratch_buffer[0x200];
    u8 service_scratch_buffer_objects[0x100];
    u8 service_scratch_buffer_move_handles[0x100];
    u8 service_scratch_buffer_copy_handles[0x100];
};

} // namespace Hydra::Horizon
