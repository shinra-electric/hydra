#include "core/horizon/kernel/kernel.hpp"

#include "core/horizon/kernel/cmif.hpp"
#include "core/horizon/kernel/session.hpp"
#include "core/horizon/kernel/thread.hpp"
#include "core/hw/tegra_x1/cpu/cpu_base.hpp"
#include "core/hw/tegra_x1/cpu/mmu_base.hpp"
#include "core/hw/tegra_x1/cpu/thread_base.hpp"

namespace Hydra::Horizon::Kernel {

SINGLETON_DEFINE_GET_INSTANCE(Kernel, HorizonKernel, "Kernel")

Kernel::Kernel(HW::Bus& bus_, HW::TegraX1::CPU::MMUBase* mmu_)
    : bus{bus_}, mmu{mmu_} {
    SINGLETON_SET_INSTANCE(HorizonKernel, "Kernel");

    // Heap memory
    // TODO: is this necessary? The app should call svcSetHeapSize anyway
    heap_mem = mmu->AllocateMemory(0x1000000);
    mmu->Map(HEAP_REGION_BASE, heap_mem,
             {MemoryType::Normal_1_0_0, MemoryAttribute::None,
              MemoryPermission::ReadWriteExecute});

    AddHandle(nullptr);
}

Kernel::~Kernel() {
    // TODO: also unmap
    mmu->FreeMemory(heap_mem);
    for (auto mem : executable_mems)
        mmu->FreeMemory(mem);

    SINGLETON_UNSET_INSTANCE();
}

uptr Kernel::CreateRomMemory(usize size, MemoryType type, MemoryPermission perm,
                             bool add_guard_page, vaddr_t& out_base) {
    size = align(size, HW::TegraX1::CPU::PAGE_SIZE);
    auto mem = mmu->AllocateMemory(size);
    mmu->Map(executable_mem_base, mem, {type, MemoryAttribute::None, perm});
    executable_mems.push_back(mem);

    out_base = executable_mem_base;
    if (add_guard_page)
        size += HW::TegraX1::CPU::PAGE_SIZE; // One guard page
    executable_mem_base += size;

    return mmu->GetMemoryPtr(mem);
}

uptr Kernel::CreateExecutableMemory(usize size, MemoryPermission perm,
                                    bool add_guard_page, vaddr_t& out_base) {
    // TODO: use MemoryType::Static
    return CreateRomMemory(size, static_cast<MemoryType>(3), perm,
                           add_guard_page, out_base);
}

bool Kernel::SupervisorCall(HW::TegraX1::CPU::ThreadBase* thread, u64 id) {
    result_t res;
    i32 tmp_i32;
    u32 tmp_u32;
    u64 tmp_u64;
    uptr tmp_uptr;
    handle_id_t tmp_handle_id;
    switch (id) {
    case 0x1:
        res = svcSetHeapSize(thread->GetRegX(1), tmp_uptr);
        thread->SetRegW(0, res);
        thread->SetRegX(1, tmp_uptr);
        break;
    case 0x2:
        res = svcSetMemoryPermission(
            thread->GetRegX(0), thread->GetRegX(1),
            static_cast<MemoryPermission>(thread->GetRegX(2)));
        thread->SetRegW(0, res);
        break;
    case 0x3:
        res = svcSetMemoryAttribute(thread->GetRegX(0), thread->GetRegX(1),
                                    thread->GetRegX(2), thread->GetRegX(3));
        thread->SetRegW(0, res);
        break;
    case 0x4:
        res = svcMapMemory(thread->GetRegX(0), thread->GetRegX(1),
                           thread->GetRegX(2));
        thread->SetRegW(0, res);
        break;
    case 0x5:
        res = svcUnmapMemory(thread->GetRegX(0), thread->GetRegX(1),
                             thread->GetRegX(2));
        thread->SetRegW(0, res);
        break;
    case 0x6:
        res = svcQueryMemory(
            thread->GetRegX(2),
            *reinterpret_cast<MemoryInfo*>(mmu->UnmapAddr(thread->GetRegX(0))),
            tmp_u32);
        thread->SetRegW(0, res);
        thread->SetRegX(1, tmp_u32);
        break;
    case 0x7:
        svcExitProcess();
        // TODO: exit all threads?
        return false;
    case 0x8:
        res = svcCreateThread(
            thread->GetRegX(1), thread->GetRegX(2), thread->GetRegX(3),
            std::bit_cast<i32>(thread->GetRegW(4)),
            std::bit_cast<i32>(thread->GetRegW(5)), tmp_handle_id);
        thread->SetRegW(0, res);
        thread->SetRegX(1, tmp_handle_id);
        break;
    case 0x9:
        res = svcStartThread(thread->GetRegW(0));
        thread->SetRegW(0, res);
        break;
    case 0xa:
        svcExitThread();
        return false;
    case 0xb:
        svcSleepThread(std::bit_cast<i64>(thread->GetRegX(0)));
        break;
    case 0xc:
        res = svcGetThreadPriority(thread->GetRegX(1), tmp_i32);
        thread->SetRegW(0, res);
        thread->SetRegW(1, std::bit_cast<u32>(tmp_i32));
        break;
    case 0xd:
        res = svcSetThreadPriority(thread->GetRegX(0), thread->GetRegX(1));
        thread->SetRegW(0, res);
        break;
    case 0xf:
        res = svcSetThreadCoreMask(thread->GetRegW(0),
                                   std::bit_cast<i32>(thread->GetRegW(1)),
                                   thread->GetRegX(2));
        thread->SetRegW(0, res);
        break;
    case 0x11:
        res = svcSignalEvent(thread->GetRegW(0));
        thread->SetRegW(0, res);
        break;
    case 0x12:
        res = svcClearEvent(thread->GetRegW(0));
        thread->SetRegW(0, res);
        break;
    case 0x13:
        res = svcMapSharedMemory(
            thread->GetRegX(0), thread->GetRegX(1), thread->GetRegX(2),
            static_cast<MemoryPermission>(thread->GetRegX(3)));
        thread->SetRegW(0, res);
        break;
    case 0x14:
        res = svcUnmapSharedMemory(thread->GetRegX(0), thread->GetRegX(1),
                                   thread->GetRegX(2));
        thread->SetRegW(0, res);
        break;
    case 0x15:
        res = svcCreateTransferMemory(
            thread->GetRegX(1), thread->GetRegX(2),
            static_cast<MemoryPermission>(thread->GetRegX(3)), tmp_handle_id);
        thread->SetRegW(0, res);
        thread->SetRegX(1, tmp_handle_id);
        break;
    case 0x16:
        res = svcCloseHandle(thread->GetRegX(0));
        thread->SetRegW(0, res);
        break;
    case 0x17:
        res = svcResetSignal(thread->GetRegX(0));
        thread->SetRegW(0, res);
        break;
    case 0x18:
        res = svcWaitSynchronization(
            reinterpret_cast<handle_id_t*>(mmu->UnmapAddr(thread->GetRegX(1))),
            std::bit_cast<i64>(thread->GetRegX(2)),
            std::bit_cast<i64>(thread->GetRegX(3)), tmp_u64);
        thread->SetRegW(0, res);
        thread->SetRegX(1, tmp_u64);
        break;
    case 0x1a:
        res = svcArbitrateLock(thread->GetRegX(0), thread->GetRegX(1),
                               thread->GetRegX(2));
        thread->SetRegW(0, res);
        break;
    case 0x1b:
        res = svcArbitrateUnlock(thread->GetRegX(0));
        thread->SetRegW(0, res);
        break;
    case 0x1c:
        res = svcWaitProcessWideKeyAtomic(
            thread->GetRegX(0), thread->GetRegX(1), thread->GetRegX(2),
            std::bit_cast<i64>(thread->GetRegX(3)));
        thread->SetRegW(0, res);
        break;
    case 0x1d:
        res = svcSignalProcessWideKey(thread->GetRegX(0), thread->GetRegX(1));
        thread->SetRegW(0, res);
        break;
    case 0x1e:
        svcGetSystemTick(tmp_u64);
        thread->SetRegX(0, tmp_u64);
        break;
    case 0x1f:
        res = svcConnectToNamedPort(
            reinterpret_cast<const char*>(mmu->UnmapAddr(thread->GetRegX(1))),
            tmp_handle_id);
        thread->SetRegW(0, res);
        thread->SetRegX(1, tmp_handle_id);
        break;
    case 0x21:
        res = svcSendSyncRequest(thread->GetTlsMemory(), thread->GetRegX(0));
        thread->SetRegW(0, res);
        break;
    case 0x25:
        res = svcGetThreadId(static_cast<handle_id_t>(thread->GetRegX(1)),
                             tmp_u64);
        thread->SetRegW(0, res);
        thread->SetRegX(1, tmp_u64);
        break;
    case 0x26:
        // Debug
        thread->LogRegisters();

        res = svcBreak(BreakReason(thread->GetRegX(0)),
                       mmu->UnmapAddr(thread->GetRegX(1)), thread->GetRegX(2));
        thread->SetRegW(0, res);
        break;
    case 0x27:
        res = svcOutputDebugString(
            reinterpret_cast<const char*>(mmu->UnmapAddr(thread->GetRegX(0))),
            thread->GetRegX(1));
        thread->SetRegW(0, res);
        break;
    case 0x29:
        res = svcGetInfo(static_cast<InfoType>(thread->GetRegX(1)),
                         thread->GetRegX(2), thread->GetRegX(3), tmp_u64);
        thread->SetRegW(0, res);
        thread->SetRegX(1, tmp_u64);
        break;
    default:
        LOG_NOT_IMPLEMENTED(HorizonKernel, "SVC 0x{:08x}", id);
        res = MAKE_RESULT(Svc, Error::NotImplemented);
        thread->SetRegW(0, res);
        break;
    }

    return true;
}

result_t Kernel::svcSetHeapSize(usize size, uptr& out_base) {
    LOG_DEBUG(HorizonKernel, "svcSetHeapSize called (size: 0x{:08x})", size);

    if ((size % HEAP_MEM_ALIGNMENT) != 0)
        return MAKE_RESULT(Svc, Error::InvalidSize); // TODO: correct?

    mmu->ResizeHeap(heap_mem, HEAP_REGION_BASE, size);

    out_base = HEAP_REGION_BASE;

    return RESULT_SUCCESS;
}

result_t Kernel::svcSetMemoryPermission(uptr addr, usize size,
                                        MemoryPermission perm) {
    LOG_DEBUG(
        HorizonKernel,
        "svcSetMemoryPermission called (addr: 0x{:08x}, size: 0x{:08x}, perm: "
        "{})",
        addr, size, perm);

    // TODO: implement
    LOG_FUNC_STUBBED(HorizonKernel);

    return RESULT_SUCCESS;
}

result_t Kernel::svcSetMemoryAttribute(uptr addr, usize size, u32 mask,
                                       u32 value) {
    LOG_DEBUG(
        HorizonKernel,
        "svcSetMemoryAttribute called (addr: 0x{:08x}, size: 0x{:08x}, mask: "
        "0x{:08x}, value: 0x{:08x})",
        addr, size, mask, value);

    // TODO: implement
    LOG_FUNC_STUBBED(HorizonKernel);

    return RESULT_SUCCESS;
}

result_t Kernel::svcMapMemory(uptr dst_addr, uptr src_addr, usize size) {
    LOG_DEBUG(
        HorizonKernel,
        "svcMapMemory called (dst_addr: 0x{:08x}, src_addr: 0x{:08x}, size: "
        "0x{:08x})",
        dst_addr, src_addr, size);

    mmu->Map(dst_addr, src_addr, size);

    return RESULT_SUCCESS;
}

result_t Kernel::svcUnmapMemory(uptr dst_addr, uptr src_addr, usize size) {
    LOG_DEBUG(
        HorizonKernel,
        "svcUnmapMemory called (dst_addr: 0x{:08x}, src_addr: 0x{:08x}, size: "
        "0x{:08x})",
        dst_addr, src_addr, size);

    // TODO: check if src_addr is the same as the one used in svcMapMemory

    mmu->Unmap(dst_addr, size);

    return RESULT_SUCCESS;
}

result_t Kernel::svcQueryMemory(uptr addr, MemoryInfo& out_mem_info,
                                u32& out_page_info) {
    LOG_DEBUG(HorizonKernel, "svcQueryMemory called (addr: 0x{:08x})", addr);

    out_mem_info = mmu->QueryMemory(addr);

    // TODO: what is this?
    out_page_info = 0;

    return RESULT_SUCCESS;
}

void Kernel::svcExitProcess() {
    LOG_DEBUG(HorizonKernel, "svcExitProcess called");
}

result_t Kernel::svcCreateThread(vaddr_t entry_point, vaddr_t args_addr,
                                 vaddr_t stack_top_addr, i32 priority,
                                 i32 processor_id,
                                 handle_id_t& out_thread_handle_id) {
    LOG_DEBUG(HorizonKernel,
              "svcCreateThread called (entry_point: 0x{:08x}, args_addr: "
              "0x{:08x}, stack_top_addr: 0x{:08x}, priority: {}, "
              "processor_id: {})",
              entry_point, args_addr, stack_top_addr, priority, processor_id);

    // Thread
    // TODO: processor ID
    auto thread = new Thread(stack_top_addr, priority);
    thread->SetEntryPoint(entry_point);
    thread->SetArg(0, args_addr);
    out_thread_handle_id = AddHandle(thread);

    return RESULT_SUCCESS;
}

result_t Kernel::svcStartThread(handle_id_t thread_handle_id) {
    LOG_DEBUG(HorizonKernel, "svcStartThread called (thread: 0x{:08x})",
              thread_handle_id);

    // Start thread
    auto thread = dynamic_cast<Thread*>(GetHandle(thread_handle_id));
    ASSERT_DEBUG(thread, HorizonKernel, "Handle 0x{:x} is not a Thread",
                 thread_handle_id);
    thread->Run();

    return RESULT_SUCCESS;
}

void Kernel::svcExitThread() {
    LOG_DEBUG(HorizonKernel, "svcExitThread called");
}

void Kernel::svcSleepThread(i64 nano) {
    LOG_DEBUG(HorizonKernel, "svcSleepThread called (nano: {})", nano);

    std::this_thread::sleep_for(std::chrono::nanoseconds(nano));
}

result_t Kernel::svcGetThreadPriority(handle_id_t thread_handle_id,
                                      i32& out_priority) {
    LOG_DEBUG(HorizonKernel, "svcGetThreadPriority called (thread: 0x{:08x})",
              thread_handle_id);

    // TODO: implement
    LOG_FUNC_STUBBED(HorizonKernel);

    // HACK
    out_priority = 0x20; // 0x0 - 0x3f, lower is higher priority

    return RESULT_SUCCESS;
}

result_t Kernel::svcSetThreadPriority(handle_id_t thread_handle_id,
                                      i32 priority) {
    LOG_DEBUG(
        HorizonKernel,
        "svcSetThreadPriority called (thread: 0x{:08x}, priority: 0x{:x})",
        thread_handle_id, priority);

    // TODO: implement
    LOG_FUNC_STUBBED(HorizonKernel);

    return RESULT_SUCCESS;
}

result_t Kernel::svcSetThreadCoreMask(handle_id_t thread_handle_id,
                                      i32 core_mask0, u64 core_mask1) {
    LOG_DEBUG(HorizonKernel,
              "svcSetThreadCoreMask called (thread: 0x{:08x}, core_mask0: "
              "0x{:08x}, core_mask1: 0x{:08x})",
              thread_handle_id, core_mask0, core_mask1);

    // TODO: implement
    LOG_FUNC_STUBBED(HorizonKernel);

    return RESULT_SUCCESS;
}

result_t Kernel::svcSignalEvent(handle_id_t event_handle_id) {
    LOG_DEBUG(HorizonKernel, "svcSignalEvent called (event: 0x{:08x})",
              event_handle_id);

    auto handle = dynamic_cast<Event*>(GetHandle(event_handle_id));
    ASSERT_DEBUG(handle, HorizonKernel, "Handle {} is not an event handle",
                 event_handle_id);

    handle->Signal();

    return RESULT_SUCCESS;
}

result_t Kernel::svcClearEvent(handle_id_t event_handle_id) {
    LOG_DEBUG(HorizonKernel, "svcClearEvent called (event: 0x{:08x})",
              event_handle_id);

    auto event = dynamic_cast<Event*>(GetHandle(event_handle_id));
    ASSERT_DEBUG(event, HorizonKernel, "Handle {} is not an event handle",
                 event_handle_id);

    if (!event->Clear())
        return MAKE_RESULT(Svc, Error::InvalidState); // TODO: correct?

    return RESULT_SUCCESS;
}

result_t Kernel::svcMapSharedMemory(handle_id_t shared_mem_handle_id, uptr addr,
                                    usize size, MemoryPermission perm) {
    LOG_DEBUG(
        HorizonKernel,
        "svcMapSharedMemory called (handle: 0x{:08x}, addr: 0x{:08x}, size: "
        "0x{:08x}, perm: {})",
        shared_mem_handle_id, addr, size, perm);

    // Map
    auto shared_mem = dynamic_cast<SharedMemory*>(
        handle_pool.GetObjectRef(shared_mem_handle_id));
    ASSERT_DEBUG(shared_mem, HorizonKernel,
                 "Handle 0x{:x} is not a shared memory handle",
                 shared_mem_handle_id);
    shared_mem->MapToRange(range(addr, addr + size), perm);

    return RESULT_SUCCESS;
}

result_t Kernel::svcUnmapSharedMemory(handle_id_t shared_mem_handle_id,
                                      uptr addr, usize size) {
    LOG_DEBUG(
        HorizonKernel,
        "svcUnmapSharedMemory called (handle: 0x{:08x}, addr: 0x{:08x}, size: "
        "0x{:08x})",
        shared_mem_handle_id, addr, size);

    // Map
    auto shared_mem = dynamic_cast<SharedMemory*>(
        handle_pool.GetObjectRef(shared_mem_handle_id));
    ASSERT_DEBUG(shared_mem, HorizonKernel,
                 "Handle 0x{:x} is not a shared memory handle",
                 shared_mem_handle_id);
    // TODO: unmap
    LOG_FUNC_STUBBED(HorizonKernel);

    return RESULT_SUCCESS;
}

result_t
Kernel::svcCreateTransferMemory(uptr addr, u64 size, MemoryPermission perm,
                                handle_id_t& out_transfer_mem_handle_id) {
    LOG_DEBUG(
        HorizonKernel,
        "svcCreateTransferMemory called (address: 0x{:08x}, size: 0x{:08x}, "
        "perm: {})",
        addr, size, perm);

    out_transfer_mem_handle_id =
        AddHandle(new TransferMemory(addr, size, perm));

    return RESULT_SUCCESS;
}

result_t Kernel::svcCloseHandle(handle_id_t handle_id) {
    LOG_DEBUG(HorizonKernel, "svcCloseHandle called (handle: 0x{:x})",
              handle_id);

    // TODO: uncomment
    // FreeHandle(handle_id);

    return RESULT_SUCCESS;
}

result_t Kernel::svcResetSignal(handle_id_t handle_id) {
    LOG_DEBUG(HorizonKernel, "svcResetSignal called (handle: 0x{:x})",
              handle_id);

    // TODO: correct?
    auto handle = dynamic_cast<Event*>(GetHandle(handle_id));
    ASSERT_DEBUG(handle, HorizonKernel, "Handle {} is not an event handle",
                 handle_id);

    if (!handle->Clear())
        return MAKE_RESULT(Svc, Error::InvalidState);

    return RESULT_SUCCESS;
}

result_t Kernel::svcWaitSynchronization(handle_id_t* handle_ids,
                                        i32 handle_count, i64 timeout,
                                        u64& out_handle_index) {
    LOG_DEBUG(
        HorizonKernel,
        "svcWaitSynchronization called (handles: 0x{}, count: {}, timeout: "
        "{})",
        (void*)handle_ids, handle_count, timeout);

    // TODO: implement multiple handles
    ASSERT_DEBUG(handle_count <= 1, HorizonKernel,
                 "Multiple handles not "
                 "implemented");

    if (handle_count == 0) {
        // TODO: allow waiting forever
        ASSERT(timeout != INFINITE_TIMEOUT, HorizonKernel,
               "Infinite timeout not implemented");
        std::this_thread::sleep_for(std::chrono::nanoseconds(timeout));
        out_handle_index = 0;

        return MAKE_RESULT(Svc, Error::TimedOut);
    } else {
        handle_id_t handle_id = handle_ids[0];
        auto event = dynamic_cast<Event*>(GetHandle(handle_id));

        // HACK
        if (!event) {
            LOG_WARN(HorizonKernel, "Handle 0x{:x} is not an event handle",
                     handle_id);
            std::this_thread::sleep_for(std::chrono::seconds(5));
            return RESULT_SUCCESS;
        }

        ASSERT_DEBUG(event, HorizonKernel,
                     "Handle 0x{:x} is not an event handle", handle_id);

        LOG_DEBUG(HorizonKernel, "Synchronizing with handle 0x{:x}", handle_id);

        if (!event->Wait(timeout))
            return MAKE_RESULT(Svc, Error::TimedOut);
    }

    return RESULT_SUCCESS;
}

result_t Kernel::svcArbitrateLock(u32 wait_tag, uptr mutex_addr, u32 self_tag) {
    LOG_DEBUG(HorizonKernel,
              "svcArbitrateLock called (wait: 0x{:08x}, mutex: 0x{:08x}, self: "
              "0x{:08x})",
              wait_tag, mutex_addr, self_tag);

    auto& mutex = mutex_map[mutex_addr];
    mutex.Lock(*reinterpret_cast<u32*>(mmu->UnmapAddr(mutex_addr)), self_tag);

    return RESULT_SUCCESS;
}

result_t Kernel::svcArbitrateUnlock(uptr mutex_addr) {
    LOG_DEBUG(HorizonKernel, "svcArbitrateUnlock called (mutex: 0x{:08x})",
              mutex_addr);

    auto& mutex = mutex_map[mutex_addr];
    mutex.Unlock(*reinterpret_cast<u32*>(mmu->UnmapAddr(mutex_addr)));

    // HACK
    /*
    if (mutex_addr == 0x40bae248) {
        static bool hack = false;
        if (!hack) {
            hack = true;
            std::this_thread::sleep_for(std::chrono::seconds(20));
        }
    }
    */

    return RESULT_SUCCESS;
}

result_t Kernel::svcWaitProcessWideKeyAtomic(uptr mutex_addr, uptr var_addr,
                                             u32 self_tag, i64 timeout) {
    LOG_DEBUG(
        HorizonKernel,
        "svcWaitProcessWideKeyAtomic called (mutex: 0x{:08x}, var: 0x{:08x}, "
        "self: 0x{:08x}, timeout: {})",
        mutex_addr, var_addr, self_tag, timeout);

    auto& mutex = mutex_map[mutex_addr];
    auto& cond_var = cond_var_map[var_addr];

    // TODO: correct?
    auto& value = *reinterpret_cast<u32*>(mmu->UnmapAddr(mutex_addr));
    mutex.Unlock(value);

    {
        std::unique_lock lock(mutex.GetNativeHandle());
        if (timeout == INFINITE_TIMEOUT)
            cond_var.wait(lock);
        else
            cond_var.wait_for(lock, std::chrono::nanoseconds(timeout));
    }

    mutex.Lock(value, self_tag);

    return RESULT_SUCCESS;
}

result_t Kernel::svcSignalProcessWideKey(uptr addr, i32 count) {
    LOG_DEBUG(HorizonKernel,
              "svcSignalProcessWideKey called (addr: 0x{:08x}, count: {})",
              addr, count);

    auto& cond_var = cond_var_map[addr];
    if (count == -1) {
        cond_var.notify_all();
    } else {
        ASSERT_DEBUG(count > 0, HorizonKernel, "Invalid signal count {}",
                     count);

        // TODO: correct?
        for (u32 i = 0; i < count; i++)
            cond_var.notify_one();
    }

    return RESULT_SUCCESS;
}

void Kernel::svcGetSystemTick(u64& out_tick) {
    LOG_DEBUG(HorizonKernel, "svcGetSystemTick called");

    out_tick = get_absolute_time();
}

result_t Kernel::svcConnectToNamedPort(const std::string& name,
                                       handle_id_t& out_session_handle_id) {
    LOG_DEBUG(HorizonKernel, "svcConnectToNamedPort called (name: {})", name);

    auto it = service_ports.find(name);
    if (it == service_ports.end()) {
        LOG_ERROR(HorizonKernel, "Unknown service name \"{}\"", name);
        return MAKE_RESULT(Svc, Error::NotFound);
    }

    out_session_handle_id = AddHandle(new Session(it->second));

    return RESULT_SUCCESS;
}

result_t Kernel::svcSendSyncRequest(HW::TegraX1::CPU::MemoryBase* tls_mem,
                                    handle_id_t session_handle_id) {
    LOG_DEBUG(HorizonKernel, "svcSendSyncRequest called (handle: 0x{:x})",
              session_handle_id);

    auto session = dynamic_cast<Session*>(GetHandle(session_handle_id));

    // HACK
    if (!session) {
        LOG_WARN(HorizonKernel, "Handle 0x{:x} is not a session handle",
                 session_handle_id);
        return RESULT_SUCCESS;
    }

    ASSERT_DEBUG(session, HorizonKernel,
                 "Handle 0x{:x} is not a session handle", session_handle_id);
    auto tls_ptr = reinterpret_cast<void*>(mmu->GetMemoryPtr(tls_mem));

    // Request

    // HIPC header
    Hipc::ParsedRequest hipc_in = Hipc::parse_request(tls_ptr);
    u8* in_ptr = align_ptr((u8*)hipc_in.data.data_words, 0x10);

    // Dispatch
    Hipc::Readers readers(mmu, hipc_in);
    Hipc::Writers writers(mmu, hipc_in, service_scratch_buffer,
                          service_scratch_buffer_objects,
                          service_scratch_buffer_move_handles,
                          service_scratch_buffer_copy_handles);
    auto command_type = static_cast<Cmif::CommandType>(hipc_in.meta.type);
    switch (command_type) {
    case Cmif::CommandType::Close:
        LOG_DEBUG(HorizonKernel, "COMMAND: Close");
        session->Close();
        break;
    case Cmif::CommandType::Request: {
        LOG_DEBUG(HorizonKernel, "COMMAND: Request");
        RequestContext context{
            readers,
            writers,
            [&](ServiceBase* service) {
                auto session = new Session(service);
                handle_id_t handle_id = AddHandle(session);
                session->SetHandleId(handle_id);
                writers.move_handles_writer.Write(handle_id);
            },
            [&](handle_id_t handle_id) {
                return static_cast<Session*>(GetHandle(handle_id))
                    ->GetService();
            },
        };
        session->Request(context);
        break;
    }
    case Cmif::CommandType::Control:
        LOG_DEBUG(HorizonKernel, "COMMAND: Control");
        session->Control(readers, writers);
        break;
    default:
        LOG_WARN(HorizonKernel, "Unknown command {}", command_type);
        break;
    }

    // Response

    // HIPC header
#define GET_ARRAY_SIZE(writer)                                                 \
    static_cast<u32>(align(writers.writer.GetWrittenSize(), (usize)4) /        \
                     sizeof(u32))

#define WRITE_ARRAY(writer, ptr)                                               \
    if (ptr) {                                                                 \
        memcpy(ptr, writers.writer.GetBase(),                                  \
               writers.writer.GetWrittenSize());                               \
    }

    Hipc::Metadata meta{.num_data_words = GET_ARRAY_SIZE(writer) +
                                          GET_ARRAY_SIZE(objects_writer),
                        .num_copy_handles = GET_ARRAY_SIZE(copy_handles_writer),
                        .num_move_handles =
                            GET_ARRAY_SIZE(move_handles_writer)};
    auto response = Hipc::make_request(tls_ptr, meta);

    u8* data_start =
        reinterpret_cast<u8*>(align_ptr(response.data_words, 0x10));
    WRITE_ARRAY(writer, data_start);
    if (writers.objects_writer.GetWrittenSize() != 0) {
        memcpy(data_start + GET_ARRAY_SIZE(writer) * sizeof(u32),
               writers.objects_writer.GetBase(),
               writers.objects_writer.GetWrittenSize());
    }
    WRITE_ARRAY(copy_handles_writer, response.copy_handles);
    WRITE_ARRAY(move_handles_writer, response.move_handles);

#undef GET_ARRAY_SIZE
#undef WRITE_ARRAY

    return RESULT_SUCCESS;
}

result_t Kernel::svcGetThreadId(handle_id_t thread_handle_id,
                                u64& out_thread_id) {
    LOG_DEBUG(HorizonKernel, "svcGetThreadId called (thread: 0x{:08x})",
              thread_handle_id);

    // TODO: implement
    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    out_thread_id = 0x0000000f;

    return RESULT_SUCCESS;
}

result_t Kernel::svcBreak(BreakReason reason, uptr buffer_ptr,
                          usize buffer_size) {
    LOG_DEBUG(HorizonKernel,
              "svcBreak called (reason: {}, buffer ptr: 0x{:08x}, buffer "
              "size: 0x{:08x})",
              reason.type, buffer_ptr, buffer_size);

    if (buffer_ptr) {
        if (buffer_size == sizeof(u32)) {
            const u32 result = *reinterpret_cast<u32*>(buffer_ptr);
            const auto module = GET_RESULT_MODULE(result);
            const auto description = GET_RESULT_DESCRIPTION(result);
            LOG_INFO(HorizonKernel, "Module: {}, description: 0x{:x}", module,
                     description);
        } else {
            for (u32 i = 0; i < buffer_size / sizeof(u32); i++) {
                const u32 value = reinterpret_cast<u32*>(buffer_ptr)[i];
                LOG_INFO(HorizonKernel, "0x{:08x}", value);
            }
        }
    }

    if (!reason.notification_only)
        throw;

    return RESULT_SUCCESS;
}

result_t Kernel::svcOutputDebugString(const char* str, usize len) {
    LOG_DEBUG(HorizonKernel, "svcOutputDebugString called");
    if (len != 0) {
        // TODO: handle differently
        LOG_INFO(HorizonKernel, "{}", std::string(str, len));
    }

    return RESULT_SUCCESS;
}

result_t Kernel::svcGetInfo(InfoType info_type, handle_id_t handle_id,
                            u64 info_sub_type, u64& out_info) {
    LOG_DEBUG(HorizonKernel,
              "svcGetInfo called (type: {}, handle: 0x{:08x}, subtype: {})",
              info_type, handle_id, info_sub_type);

    switch (info_type) {
    case InfoType::CoreMask:
        LOG_NOT_IMPLEMENTED(HorizonKernel, "CoreMask");
        // HACK
        out_info = 0;
        return RESULT_SUCCESS;
    case InfoType::AliasRegionAddress:
        out_info = ALIAS_REGION_BASE;
        return RESULT_SUCCESS;
    case InfoType::AliasRegionSize:
        out_info = ALIAS_REGION_SIZE;
        return RESULT_SUCCESS;
    case InfoType::HeapRegionAddress:
        out_info = HEAP_REGION_BASE;
        return RESULT_SUCCESS;
    case InfoType::HeapRegionSize:
        out_info = HEAP_REGION_SIZE;
        return RESULT_SUCCESS;
    case InfoType::AslrRegionAddress:
        out_info = ADDRESS_SPACE_START;
        return RESULT_SUCCESS;
    case InfoType::AslrRegionSize:
        out_info = ADDRESS_SPACE_SIZE;
        return RESULT_SUCCESS;
    case InfoType::StackRegionAddress:
        out_info = STACK_REGION_BASE;
        return RESULT_SUCCESS;
    case InfoType::StackRegionSize:
        out_info = STACK_REGION_SIZE;
        return RESULT_SUCCESS;
    case InfoType::TotalMemorySize:
        // TODO: what should this be?
        out_info = 3u * 1024u * 1024u * 1024u;
        return RESULT_SUCCESS;
    case InfoType::UsedMemorySize: {
        // TODO: correct?
        /*
        usize size = stack_mem->GetSize() + kernel_mem->GetSize() +
                     tls_mem->GetSize() + heap_mem->GetSize();
        for (auto executable_mem : executable_memories)
            size += executable_mem->GetSize();
        out_info = size;
        */
        out_info = 4u * 1024u * 1024u;
        return RESULT_SUCCESS;
    }
    case InfoType::DebuggerAttached:
        // TODO: make this configurable
        out_info = true;
        return RESULT_SUCCESS;
    case InfoType::RandomEntropy:
        // TODO: correct?
        // TODO: subtype 0-3
        out_info = rand();
        return RESULT_SUCCESS;
    case InfoType::AliasRegionExtraSize:
        LOG_NOT_IMPLEMENTED(HorizonKernel, "AliasRegionExtraSize");
        // HACK
        out_info = 0;
        return RESULT_SUCCESS;
    default:
        LOG_WARN(HorizonKernel, "Unimplemented info type {}", info_type);
        return MAKE_RESULT(Svc, 0x78);
    }
}

void Kernel::SetHandle(handle_id_t handle_id, Handle* handle) {
    handle_pool.GetObjectRef(handle_id) = handle;
}

handle_id_t Kernel::AddHandle(Handle* handle) {
    handle_id_t handle_id = handle_pool.AllocateForIndex();
    handle_pool.GetObjectRef(handle_id) = handle;

    return handle_id;
}

HW::TegraX1::CPU::MemoryBase* Kernel::CreateTlsMemory(vaddr_t& base) {
    constexpr usize TLS_MEM_SIZE = 0x20000;

    auto mem = mmu->AllocateMemory(TLS_MEM_SIZE);
    base = tls_mem_base;
    mmu->Map(base, mem,
             {MemoryType::ThreadLocal, MemoryAttribute::None,
              MemoryPermission::ReadWrite});
    tls_mem_base += TLS_MEM_SIZE;

    return mem;
}

} // namespace Hydra::Horizon::Kernel
