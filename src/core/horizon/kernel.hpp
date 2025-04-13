#pragma once

#include "common/allocators/dynamic_pool.hpp"
#include "common/allocators/static_pool.hpp"
#include "core/horizon/filesystem/filesystem.hpp"
#include "core/horizon/shared_memory.hpp"
#include <condition_variable>
#include <type_traits>

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

// TODO: should this be used?
constexpr u32 HANDLE_WAIT_MASK = 0x40000000;

class Mutex {
  public:
    void Lock(u32& value, u32 self_tag) {
        std::unique_lock<std::mutex> lock(mutex);
        // TODO: uncomment?
        // value = value | HANDLE_WAIT_MASK;
        cv.wait(lock, [&] { return (value & ~HANDLE_WAIT_MASK) == 0; });
        value = self_tag | (value & HANDLE_WAIT_MASK);
    }

    void Unlock(u32& value) {
        std::unique_lock<std::mutex> lock(mutex);
        value = (value & HANDLE_WAIT_MASK);
        cv.notify_one();
    }

    // Getters
    std::mutex& GetNativeHandle() { return mutex; }

  private:
    std::mutex mutex;
    std::condition_variable cv;
};

class KernelHandle {
  public:
    virtual ~KernelHandle() = default;
};

class SynchronizationHandle : public KernelHandle {
  public:
    SynchronizationHandle(bool signaled_ = false) : signaled{signaled_} {}

    void Signal() {
        std::unique_lock<std::mutex> lock(mutex);
        signaled = true;
        cv.notify_all();
    }

    void Wait(i64 timeout) {
        std::unique_lock<std::mutex> lock(mutex);
        if (IS_TIMEOUT_INFINITE(timeout)) {
            cv.wait(lock, [this] { return signaled; });
        } else {
            cv.wait_for(lock, std::chrono::nanoseconds(timeout),
                        [this] { return signaled; });
        }

        // TODO: correct?
        signaled = false;
    }

  private:
    std::mutex mutex;
    std::condition_variable cv;
    bool signaled = false;
};

class ThreadHandle : public KernelHandle {
  public:
    ThreadHandle(HW::TegraX1::CPU::MemoryBase* tls_mem_, vaddr_t tls_addr_,
                 vaddr_t entry_point_, vaddr_t args_addr_,
                 vaddr_t stack_top_addr_, i32 priority_)
        : tls_mem{tls_mem_}, tls_addr{tls_addr_}, entry_point{entry_point_},
          args_addr{args_addr_}, stack_top_addr{stack_top_addr_},
          priority{priority_} {}
    ~ThreadHandle() override;

    void Start();

  private:
    HW::TegraX1::CPU::MemoryBase* tls_mem;
    vaddr_t tls_addr;
    vaddr_t entry_point;
    vaddr_t args_addr;
    vaddr_t stack_top_addr;
    i32 priority;

    std::thread* t = nullptr;
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

    void InitializeThread(HW::TegraX1::CPU::ThreadBase* thread,
                          vaddr_t entry_point, vaddr_t tls_addr,
                          vaddr_t stack_top_addr);
    void InitializeMainThread(HW::TegraX1::CPU::ThreadBase* thread);

    // Loading
    // TODO: should the caller be able to specify permissions?
    uptr CreateExecutableMemory(usize size, vaddr_t& out_base,
                                MemoryPermission perm);
    void SetMainThreadEntryPoint(uptr main_thread_entry_point_) {
        main_thread_entry_point = main_thread_entry_point_;
    }
    void SetMainThreadArg(u32 index, u64 value) {
        ASSERT_DEBUG(index < ARG_COUNT, HorizonKernel, "Invalid arg index {}",
                     index);
        main_thread_args[index] = value;
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
    Result svcCreateThread(vaddr_t entry_point, vaddr_t args_addr,
                           vaddr_t stack_top_addr, i32 priority,
                           i32 processor_id, handle_id_t& out_thread_handle_id);
    Result svcStartThread(handle_id_t thread_handle_id);
    void svcSleepThread(i64 nano);
    Result svcGetThreadPriority(handle_id_t thread_handle_id,
                                i32& out_priority);
    Result svcSetThreadPriority(handle_id_t thread_handle_id, i32 priority);
    Result svcSetThreadCoreMask(handle_id_t thread_handle_id, i32 core_mask0,
                                u64 core_mask1);
    Result svcMapSharedMemory(handle_id_t shared_mem_handle_id, uptr addr,
                              usize size, MemoryPermission perm);
    Result svcUnmapSharedMemory(handle_id_t shared_mem_handle_id, uptr addr,
                                usize size);
    Result svcCreateTransferMemory(uptr addr, u64 size, MemoryPermission perm,
                                   handle_id_t& out_transfer_mem_handle_id);
    Result svcCloseHandle(handle_id_t handle_id);
    Result svcResetSignal(handle_id_t handle_id);
    Result svcWaitSynchronization(handle_id_t* handle_ids, i32 handle_count,
                                  i64 timeout, u64& out_handle_index);
    Result svcArbitrateLock(u32 wait_tag, uptr mutex_addr, u32 self_tag);
    Result svcArbitrateUnlock(uptr mutex_addr);
    Result svcWaitProcessWideKeyAtomic(uptr mutex_addr, uptr var_addr,
                                       u32 self_tag, i64 timeout);
    Result svcSignalProcessWideKey(uptr addr, i32 count);
    void svcGetSystemTick(u64& out_tick);
    Result svcConnectToNamedPort(const std::string& name,
                                 handle_id_t& out_session_handle_id);
    Result svcSendSyncRequest(HW::TegraX1::CPU::MemoryBase* tls_mem,
                              handle_id_t session_handle_id);
    Result svcGetThreadId(handle_id_t thread_handle_id, u64& out_thread_id);
    Result svcBreak(BreakReason reason, uptr buffer_ptr, usize buffer_size);
    Result svcOutputDebugString(const char* str, usize len);
    Result svcGetInfo(InfoType info_type, handle_id_t handle_id,
                      u64 info_sub_type, u64& out_info);

    // Getters
    HW::Bus& GetBus() const { return bus; }

    Filesystem::Filesystem& GetFilesystem() { return filesystem; }

    HW::TegraX1::CPU::MemoryBase* GetTlsMemory() const { return tls_mem; }

    // Helpers
    KernelHandle* GetHandle(handle_id_t handle_id) const {
        return handle_pool.GetObject(handle_id);
    }
    void SetHandle(handle_id_t handle_id, KernelHandle* handle);
    handle_id_t AddHandle(KernelHandle* handle);
    void FreeHandle(handle_id_t handle_id) {
        delete GetHandle(handle_id);
        handle_pool.FreeByIndex(handle_id);
    }

    handle_id_t CreateSharedMemory(usize size);

    const SharedMemory& GetSharedMemory(handle_id_t handle_id) const {
        return *shared_memory_pool.GetObject(handle_id);
    }

    HW::TegraX1::CPU::MemoryBase* CreateTlsMemory(vaddr_t& base);

  private:
    HW::Bus& bus;
    HW::TegraX1::CPU::MMUBase* mmu;

    uptr main_thread_entry_point{0x0};
    u64 main_thread_args[ARG_COUNT] = {0x0};

    Filesystem::Filesystem filesystem;

    // Memory
    HW::TegraX1::CPU::MemoryBase* stack_mem;
    HW::TegraX1::CPU::MemoryBase* tls_mem; // TODO: remove this
    HW::TegraX1::CPU::MemoryBase* heap_mem;
    std::vector<HW::TegraX1::CPU::MemoryBase*> executable_mems;

    vaddr_t executable_mem_base{0x40000000};
    vaddr_t tls_mem_base{TLS_REGION_BASE};

    // Handles
    Allocators::DynamicPool<KernelHandle*> handle_pool;
    Allocators::DynamicPool<SharedMemory*> shared_memory_pool;

    // TODO: use a different container?
    std::map<vaddr_t, Mutex> mutex_map;
    std::map<vaddr_t, std::condition_variable> cond_var_map;

    // Services
    std::map<std::string, Services::ServiceBase*> service_ports;
    u8 service_scratch_buffer[0x200];
    u8 service_scratch_buffer_objects[0x100];
    u8 service_scratch_buffer_move_handles[0x100];
    u8 service_scratch_buffer_copy_handles[0x100];
};

template <typename T> struct KernelHandleWithId {
    static_assert(std::is_convertible_v<T*, KernelHandle*>,
                  "Type does not inherit from KernelHandle");

    T* handle;
    handle_id_t id;

    KernelHandleWithId(T* handle_) : handle{handle_} {
        id = Kernel::GetInstance().AddHandle(handle);
    }

    ~KernelHandleWithId() { Kernel::GetInstance().FreeHandle(id); }
};

} // namespace Hydra::Horizon
