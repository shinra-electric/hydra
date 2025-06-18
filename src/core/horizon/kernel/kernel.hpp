#pragma once

#include "core/horizon/filesystem/filesystem.hpp"
#include "core/horizon/kernel/event.hpp"
#include "core/horizon/kernel/handle_pool.hpp"
#include "core/horizon/kernel/mutex.hpp"
#include "core/horizon/kernel/shared_memory.hpp"
#include "core/horizon/kernel/transfer_memory.hpp"

#define KERNEL_INSTANCE hydra::horizon::kernel::Kernel::GetInstance()

namespace hydra::hw::tegra_x1::cpu {
class MMUBase;
class ThreadBase;
} // namespace hydra::hw::tegra_x1::cpu

namespace hydra::hw {
class Bus;
}

namespace hydra::horizon::kernel {

class ServiceBase;
class Thread;

class Kernel {
  public:
    static Kernel& GetInstance();

    Kernel(hw::Bus& bus_, hw::tegra_x1::cpu::MMUBase* mmu_);
    ~Kernel();

    // Loading
    uptr CreateRomMemory(usize size, MemoryType type, MemoryPermission perm,
                         bool add_guard_page, vaddr_t& out_base);
    // TODO: should the caller be able to specify permissions?
    uptr CreateExecutableMemory(const std::string_view module_name, usize size,
                                MemoryPermission perm, bool add_guard_page,
                                vaddr_t& out_base);

    void ConnectServiceToPort(const std::string& port_name,
                              ServiceBase* service) {
        service_ports[std::string(port_name)] = service;
    }

    bool SupervisorCall(Thread* thread,
                        hw::tegra_x1::cpu::ThreadBase* guest_thread, u64 id);

    // SVCs
    result_t svcSetHeapSize(usize size, uptr& out_base);
    result_t svcSetMemoryPermission(uptr addr, usize size,
                                    MemoryPermission perm);
    result_t svcSetMemoryAttribute(uptr addr, usize size, u32 mask, u32 value);
    result_t svcMapMemory(uptr dst_addr, uptr src_addr, usize size);
    result_t svcUnmapMemory(uptr dst_addr, uptr src_addr, usize size);
    result_t svcQueryMemory(uptr addr, MemoryInfo& out_mem_info,
                            u32& out_page_info);
    void svcExitProcess();
    result_t svcCreateThread(vaddr_t entry_point, vaddr_t args_addr,
                             vaddr_t stack_top_addr, i32 priority,
                             i32 processor_id,
                             handle_id_t& out_thread_handle_id);
    result_t svcStartThread(handle_id_t thread_handle_id);
    void svcExitThread(Thread* current_thread);
    void svcSleepThread(i64 nano);
    result_t svcGetThreadPriority(handle_id_t thread_handle_id,
                                  i32& out_priority);
    result_t svcSetThreadPriority(handle_id_t thread_handle_id, i32 priority);
    result_t svcGetThreadCoreMask(handle_id_t thread_handle_id,
                                  i32& out_core_mask0, u64& out_core_mask1);
    result_t svcSetThreadCoreMask(handle_id_t thread_handle_id, i32 core_mask0,
                                  u64 core_mask1);
    void svcGetCurrentProcessorNumber(u32& out_number);
    result_t svcSignalEvent(handle_id_t event_handle_id);
    result_t svcClearEvent(handle_id_t event_handle_id);
    result_t svcMapSharedMemory(handle_id_t shared_mem_handle_id, uptr addr,
                                usize size, MemoryPermission perm);
    result_t svcUnmapSharedMemory(handle_id_t shared_mem_handle_id, uptr addr,
                                  usize size);
    result_t svcCreateTransferMemory(uptr addr, u64 size, MemoryPermission perm,
                                     handle_id_t& out_transfer_mem_handle_id);
    result_t svcCloseHandle(handle_id_t handle_id);
    result_t svcResetSignal(handle_id_t handle_id);
    result_t svcWaitSynchronization(handle_id_t* handle_ids, i32 handle_count,
                                    i64 timeout, u64& out_handle_index);
    result_t svcCancelSynchronization(handle_id_t thread_handle_id);
    result_t svcArbitrateLock(u32 wait_tag, uptr mutex_addr, u32 self_tag);
    result_t svcArbitrateUnlock(uptr mutex_addr);
    result_t svcWaitProcessWideKeyAtomic(uptr mutex_addr, uptr var_addr,
                                         u32 self_tag, i64 timeout);
    result_t svcSignalProcessWideKey(uptr addr, i32 count);
    void svcGetSystemTick(u64& out_tick);
    result_t svcConnectToNamedPort(const std::string& name,
                                   handle_id_t& out_session_handle_id);
    result_t svcSendSyncRequest(hw::tegra_x1::cpu::MemoryBase* tls_mem,
                                handle_id_t session_handle_id);
    result_t svcGetThreadId(handle_id_t thread_handle_id, u64& out_thread_id);
    result_t svcBreak(BreakReason reason, uptr buffer_ptr, usize buffer_size);
    result_t svcOutputDebugString(const char* str, usize len);
    result_t svcGetInfo(InfoType info_type, handle_id_t handle_id,
                        u64 info_sub_type, u64& out_info);
    result_t svcMapPhysicalMemory(vaddr_t addr, usize size);
    result_t svcSetThreadActivity(handle_id_t thread_handle_id,
                                  ThreadActivity activity);
    result_t svcGetThreadContext3(handle_id_t thread_handle_id,
                                  ThreadContext& out_thread_context);
    result_t svcWaitForAddress(vaddr_t addr, ArbitrationType arbitration_type,
                               u32 value, u64 timeout);

    // Helpers

    // Handles
    handle_id_t GetRealHandleID(handle_id_t handle_id) const {
        if (handle_id == CURRENT_PROCESS_PSEUDO_HANDLE)
            return current_process_handle_id;

        // TODO: CURRENT_THREAD_PSEUDO_HANDLE

        return handle_id;
    }
    AutoObject* GetHandle(handle_id_t handle_id) const {
        return handle_pool.Get(GetRealHandleID(handle_id));
    }
    handle_id_t AddHandle(AutoObject* handle) {
        return handle_pool.Add(handle);
    }
    void AddProcessHandle(AutoObject* handle) {
        current_process_handle_id = AddHandle(handle);
    }
    void FreeHandle(handle_id_t handle_id) {
        handle_pool.Free(GetRealHandleID(handle_id));
    }

    hw::tegra_x1::cpu::MemoryBase* CreateTlsMemory(vaddr_t& base);

    hw::Bus& GetBus() const { return bus; }
    hw::tegra_x1::cpu::MMUBase* GetMMU() const { return mmu; }

    u64 GetTitleID() const { return title_id; }
    void SetTitleId(const u64 title_id_) { title_id = title_id_; }

  private:
    hw::Bus& bus;
    hw::tegra_x1::cpu::MMUBase* mmu;

    filesystem::Filesystem filesystem;

    u64 title_id{invalid<u64>()};
    handle_id_t current_process_handle_id{INVALID_HANDLE_ID};

    // Memory
    hw::tegra_x1::cpu::MemoryBase* heap_mem;
    std::vector<hw::tegra_x1::cpu::MemoryBase*> executable_mems;

    vaddr_t executable_mem_base{0x40000000};
    vaddr_t tls_mem_base{TLS_REGION_BASE};

    // Handles
    DynamicHandlePool<AutoObject> handle_pool;

    std::mutex sync_mutex;
    // TODO: use a different container?
    std::map<vaddr_t, Mutex> mutex_map;
    std::map<vaddr_t, std::condition_variable> cond_var_map;

    // Services
    std::map<std::string, ServiceBase*> service_ports;
};

template <typename T> struct HandleWithId {
    static_assert(std::is_convertible_v<T*, AutoObject*>,
                  "Type does not inherit from AutoObject");

    T* handle;
    handle_id_t id;

    HandleWithId(T* handle_) : handle{handle_} {
        id = KERNEL_INSTANCE.AddHandle(handle);
    }

    ~HandleWithId() { KERNEL_INSTANCE.FreeHandle(id); }
};

} // namespace hydra::horizon::kernel
