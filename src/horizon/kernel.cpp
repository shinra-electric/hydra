#include "horizon/kernel.hpp"

#include "common/logging/log.hpp"
#include "horizon/cmif.hpp"
#include "horizon/const.hpp"
#include "horizon/hipc.hpp"
#include "horizon/services/service_base.hpp"
#include "hw/tegra_x1/cpu/cpu_base.hpp"
#include "hw/tegra_x1/cpu/mmu_base.hpp"
#include "hw/tegra_x1/cpu/thread_base.hpp"

namespace Hydra::Horizon {

// TODO: how does this work?
constexpr uptr ADDRESS_SPACE_BASE = 0x10000000;
constexpr usize ADDRESS_SPACE_SIZE = 0x200000000 - ADDRESS_SPACE_BASE;

constexpr uptr STACK_REGION_BASE = 0x10000000;
constexpr usize STACK_REGION_SIZE = 0x10000000;
constexpr usize STACK_MEM_SIZE = 0x2000000;

constexpr uptr TLS_REGION_BASE = 0x20000000;
constexpr usize TLS_REGION_SIZE = 0x10000000;
constexpr usize TLS_MEM_SIZE = 0x20000;

constexpr uptr HEAP_REGION_BASE = 0x100000000;
constexpr usize HEAP_REGION_SIZE = 0x100000000;
constexpr usize DEFAULT_HEAP_MEM_SIZE = 0x1000000;
constexpr usize HEAP_MEM_ALIGNMENT = 0x200000;

SINGLETON_DEFINE_GET_INSTANCE(Kernel, HorizonKernel, "Kernel")

Kernel::Kernel(HW::Bus& bus_, HW::TegraX1::CPU::MMUBase* mmu_)
    : bus{bus_}, mmu{mmu_} {
    SINGLETON_SET_INSTANCE(HorizonKernel, "Kernel");

    // Memory

    // Stack memory
    mmu->AllocateAndMap(STACK_REGION_BASE, STACK_MEM_SIZE);

    // TLS memory
    tls_ptr = mmu->AllocateAndMap(TLS_REGION_BASE, TLS_MEM_SIZE);

    // Heap memory
    mmu->AllocateAndMap(HEAP_REGION_BASE, DEFAULT_HEAP_MEM_SIZE);
}

Kernel::~Kernel() { SINGLETON_UNSET_INSTANCE(); }

void Kernel::ConfigureThread(HW::TegraX1::CPU::ThreadBase* thread) {
    thread->Configure([&](HW::TegraX1::CPU::ThreadBase* thread,
                          u64 id) { return SupervisorCall(thread, id); },
                      TLS_REGION_BASE, STACK_REGION_BASE + STACK_MEM_SIZE);
}

void Kernel::ConfigureMainThread(HW::TegraX1::CPU::ThreadBase* thread) {
    ConfigureThread(thread);

    // Set initial PC
    ASSERT_DEBUG(entry_point != 0x0, HorizonKernel, "Invalid entry point");
    thread->SetRegPC(entry_point);

    // Arguments
    for (u32 i = 0; i < ARG_COUNT; i++)
        thread->SetRegX(i, args[i]);
}

uptr Kernel::CreateExecutableMemory(usize size, vaddr& out_base) {
    size = align(size, HW::TegraX1::CPU::PAGE_SIZE);
    uptr ptr = mmu->AllocateAndMap(executable_mem_base, size);
    out_base = executable_mem_base;
    executable_mem_base += size;

    return ptr;
}

bool Kernel::SupervisorCall(HW::TegraX1::CPU::ThreadBase* thread, u64 id) {
    Result res;
    u32 tmp_u32;
    u64 tmp_u64;
    uptr tmp_uptr;
    HandleId tmp_hanle_id;
    switch (id) {
    case 0x1:
        res = svcSetHeapSize(thread->GetRegX(1), tmp_uptr);
        thread->SetRegX(0, res);
        thread->SetRegX(1, tmp_uptr);
        break;
    case 0x2:
        res =
            svcSetMemoryPermission(thread->GetRegX(0), thread->GetRegX(1),
                                   static_cast<Permission>(thread->GetRegX(2)));
        thread->SetRegX(0, res);
        break;
    case 0x3:
        res = svcSetMemoryAttribute(thread->GetRegX(0), thread->GetRegX(1),
                                    thread->GetRegX(2), thread->GetRegX(3));
        thread->SetRegX(0, res);
        break;
    case 0x4:
        res = svcMapMemory(thread->GetRegX(0), thread->GetRegX(1),
                           thread->GetRegX(2));
        thread->SetRegX(0, res);
        break;
    case 0x5:
        res = svcUnmapMemory(thread->GetRegX(0), thread->GetRegX(1),
                             thread->GetRegX(2));
        thread->SetRegX(0, res);
        break;
    case 0x6:
        res = svcQueryMemory(
            thread->GetRegX(2),
            *reinterpret_cast<MemoryInfo*>(mmu->UnmapAddr(thread->GetRegX(0))),
            tmp_u32);
        thread->SetRegX(0, res);
        thread->SetRegX(1, tmp_u32);
        break;
    case 0x7:
        svcExitProcess();
        return false;
    case 0xb:
        svcSleepThread(bit_cast<i64>(thread->GetRegX(0)));
        break;
    case 0xc:
        res = svcGetThreadPriority(thread->GetRegX(1), tmp_u32);
        thread->SetRegX(0, res);
        thread->SetRegX(1, tmp_u32);
        break;
    case 0x13:
        res = svcMapSharedMemory(thread->GetRegX(0), thread->GetRegX(1),
                                 thread->GetRegX(2),
                                 static_cast<Permission>(thread->GetRegX(3)));
        thread->SetRegX(0, res);
        break;
    case 0x14:
        res = svcUnmapSharedMemory(thread->GetRegX(0), thread->GetRegX(1),
                                   thread->GetRegX(2));
        thread->SetRegX(0, res);
        break;
    case 0x15:
        res = svcCreateTransferMemory(
            thread->GetRegX(1), thread->GetRegX(2),
            static_cast<Permission>(thread->GetRegX(3)), tmp_hanle_id);
        thread->SetRegX(0, res);
        thread->SetRegX(1, tmp_hanle_id);
        break;
    case 0x16:
        res = svcCloseHandle(thread->GetRegX(0));
        thread->SetRegX(0, res);
        break;
    case 0x17:
        res = svcResetSignal(thread->GetRegX(0));
        thread->SetRegX(0, res);
        break;
    case 0x18:
        res = svcWaitSynchronization(
            reinterpret_cast<HandleId*>(thread->GetRegX(1)),
            bit_cast<i64>(thread->GetRegX(2)),
            bit_cast<i64>(thread->GetRegX(3)), tmp_u64);
        thread->SetRegX(0, res);
        thread->SetRegX(1, tmp_u64);
        break;
    case 0x1a:
        res = svcArbitrateLock(thread->GetRegX(0), thread->GetRegX(1),
                               thread->GetRegX(2));
        thread->SetRegX(0, res);
        break;
    case 0x1b:
        res = svcArbitrateUnlock(thread->GetRegX(0));
        thread->SetRegX(0, res);
        break;
    case 0x1c:
        res = svcWaitProcessWideKeyAtomic(
            thread->GetRegX(0), thread->GetRegX(1), thread->GetRegX(2),
            bit_cast<i64>(thread->GetRegX(3)));
        thread->SetRegX(0, res);
        break;
    case 0x1d:
        res = svcSignalProcessWideKey(mmu->UnmapAddr(thread->GetRegX(0)),
                                      thread->GetRegX(1));
        thread->SetRegX(0, res);
        break;
    case 0x1e:
        svcGetSystemTick(tmp_u64);
        thread->SetRegX(0, tmp_u64);
        break;
    case 0x1f:
        res = svcConnectToNamedPort(
            reinterpret_cast<const char*>(mmu->UnmapAddr(thread->GetRegX(1))),
            tmp_hanle_id);
        thread->SetRegX(0, res);
        thread->SetRegX(1, tmp_hanle_id);
        break;
    case 0x21:
        res = svcSendSyncRequest(thread->GetRegX(0));
        thread->SetRegX(0, res);
        break;
    case 0x25:
        res =
            svcGetThreadId(static_cast<HandleId>(thread->GetRegX(1)), tmp_u64);
        thread->SetRegX(0, res);
        thread->SetRegX(1, tmp_u64);
        break;
    case 0x26:
        // Debug
        thread->LogRegisters();

        res = svcBreak(BreakReason(thread->GetRegX(0)),
                       mmu->UnmapAddr(thread->GetRegX(1)), thread->GetRegX(2));
        thread->SetRegX(0, res);
        break;
    case 0x27:
        res = svcOutputDebugString(
            reinterpret_cast<const char*>(mmu->UnmapAddr(thread->GetRegX(0))),
            thread->GetRegX(1));
        thread->SetRegX(0, res);
        break;
    case 0x29:
        res = svcGetInfo(static_cast<InfoType>(thread->GetRegX(1)),
                         thread->GetRegX(2), thread->GetRegX(3), tmp_u64);
        thread->SetRegX(0, res);
        thread->SetRegX(1, tmp_u64);
        break;
    default:
        LOG_WARNING(HorizonKernel, "Unimplemented SVC 0x{:08x}", id);
        res = MAKE_KERNEL_RESULT(NotImplemented);
        thread->SetRegX(0, res);
        break;
    }

    return true;
}

Result Kernel::svcSetHeapSize(usize size, uptr& out_base) {
    LOG_DEBUG(HorizonKernel, "svcSetHeapSize called (size: 0x{:08x})", size);

    if ((size % HEAP_MEM_ALIGNMENT) != 0)
        return MAKE_KERNEL_RESULT(InvalidSize); // TODO: correct?

    // TODO: implement
    /*
    if (size != heap_mem->GetSize()) {
        mmu->UnmapMemory(HEAP_REGION_BASE, heap_mem);
        heap_mem->Resize(size);
        mmu->MapMemory(HEAP_REGION_BASE, heap_mem);
    }
    */

    out_base = HEAP_REGION_BASE;

    return RESULT_SUCCESS;
}

Result Kernel::svcSetMemoryPermission(uptr addr, usize size,
                                      Permission permission) {
    LOG_DEBUG(
        HorizonKernel,
        "svcSetMemoryPermission called (addr: 0x{:08x}, size: 0x{:08x}, perm: "
        "{})",
        addr, size, permission);

    // TODO: implement
    LOG_FUNC_STUBBED(HorizonKernel);

    return RESULT_SUCCESS;
}

Result Kernel::svcSetMemoryAttribute(uptr addr, usize size, u32 mask,
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

Result Kernel::svcMapMemory(uptr dst_addr, uptr src_addr, usize size) {
    LOG_DEBUG(
        HorizonKernel,
        "svcMapMemory called (dst_addr: 0x{:08x}, src_addr: 0x{:08x}, size: "
        "0x{:08x})",
        dst_addr, src_addr, size);

    // TODO: implement
    LOG_FUNC_STUBBED(HorizonKernel);
    // mmu->Map(dst_addr, {mmu->UnmapAddr(src_addr), size});

    return RESULT_SUCCESS;
}

Result Kernel::svcUnmapMemory(uptr dst_addr, uptr src_addr, usize size) {
    LOG_DEBUG(
        HorizonKernel,
        "svcUnmapMemory called (dst_addr: 0x{:08x}, src_addr: 0x{:08x}, size: "
        "0x{:08x})",
        dst_addr, src_addr, size);

    // TODO: implement
    LOG_FUNC_STUBBED(HorizonKernel);
    // mmu->Unmap(dst_addr);

    return RESULT_SUCCESS;
}

Result Kernel::svcQueryMemory(uptr addr, MemoryInfo& out_mem_info,
                              u32& out_page_info) {
    LOG_DEBUG(HorizonKernel, "svcQueryMemory called (addr: 0x{:08x})", addr);

    LOG_FUNC_STUBBED(HorizonKernel);

    // HACK
    /*
    uptr base;
    const auto* mem = mmu->FindAddrImplRef(addr, base);
    if (!mem) {
        // TODO: how should this behave?
        out_mem_info = MemoryInfo{
            .addr = addr,
            .size = (addr < ADDRESS_SPACE_BASE + ADDRESS_SPACE_SIZE
                         ? 0x10000000u
                         : 0x0u), // HACK: awful hack
        };

        // TODO: out_page_info
        out_page_info = 0;

        return RESULT_SUCCESS;
    }
    */

    // HACK
    bool is_rom = (addr >= 0x80000000 && addr < 0xa0000000);
    out_mem_info = MemoryInfo{
        .addr = addr,                         // base, // TODO: check
        .size = 0x1000,                       // mem->size,
        .type = (is_rom ? 0x3u : 0x00402006), // HACK: static
        // TODO: attr
        .perm = Permission::ReadExecute, // HACK
        // TODO: ipc_ref_count
        // TODO: device_ref_count
    };

    // TODO: out_page_info
    out_page_info = 0;

    return RESULT_SUCCESS;
}

void Kernel::svcExitProcess() {
    LOG_DEBUG(HorizonKernel, "svcExitProcess called");
}

void Kernel::svcSleepThread(i64 nano) {
    LOG_DEBUG(HorizonKernel, "svcSleepThread called (nano: 0x{:08x})", nano);

    std::this_thread::sleep_for(std::chrono::nanoseconds(nano));
}

Result Kernel::svcGetThreadPriority(HandleId thread_handle_id,
                                    u32& out_priority) {
    LOG_DEBUG(HorizonKernel, "svcGetThreadPriority called (thread: 0x{:08x})",
              thread_handle_id);

    // TODO: implement
    LOG_FUNC_STUBBED(HorizonKernel);

    // HACK
    out_priority = 0x20; // 0x0 - 0x3f, lower is higher priority

    return RESULT_SUCCESS;
}

Result Kernel::svcMapSharedMemory(HandleId shared_mem_handle_id, uptr addr,
                                  usize size, Permission permission) {
    LOG_DEBUG(
        HorizonKernel,
        "svcMapSharedMemory called (handle: 0x{:08x}, addr: 0x{:08x}, size: "
        "0x{:08x}, perm: {})",
        shared_mem_handle_id, addr, size, permission);

    // Map
    auto shared_mem = shared_memory_pool.GetObjectRef(shared_mem_handle_id);
    shared_mem->MapToRange(mmu, range(addr, size));

    return RESULT_SUCCESS;
}

Result Kernel::svcUnmapSharedMemory(HandleId shared_mem_handle_id, uptr addr,
                                    usize size) {
    LOG_DEBUG(
        HorizonKernel,
        "svcUnmapSharedMemory called (handle: 0x{:08x}, addr: 0x{:08x}, size: "
        "0x{:08x})",
        shared_mem_handle_id, addr, size);

    // Map
    auto shared_mem = shared_memory_pool.GetObjectRef(shared_mem_handle_id);
    // TODO: unmap
    LOG_FUNC_STUBBED(HorizonKernel);

    return RESULT_SUCCESS;
}

Result Kernel::svcCreateTransferMemory(uptr addr, u64 size,
                                       Permission permission,
                                       HandleId& out_transfer_mem_handle_id) {
    LOG_DEBUG(
        HorizonKernel,
        "svcCreateTransferMemory called (address: 0x{:08x}, size: 0x{:08x}, "
        "perm: {})",
        addr, size, permission);

    out_transfer_mem_handle_id =
        AddHandle(new TransferMemory(addr, size, permission));

    return RESULT_SUCCESS;
}

Result Kernel::svcCloseHandle(HandleId handle_id) {
    LOG_DEBUG(HorizonKernel, "svcCloseHandle called (handle: 0x{:08x})",
              handle_id);

    delete GetHandle(handle_id);
    handle_pool.FreeByIndex(handle_id);

    return RESULT_SUCCESS;
}

Result Kernel::svcResetSignal(HandleId handle_id) {
    LOG_DEBUG(HorizonKernel, "svcResetSignal called (handle: 0x{:08x})",
              handle_id);

    // TODO: implement
    LOG_FUNC_STUBBED(HorizonKernel);

    return RESULT_SUCCESS;
}

Result Kernel::svcWaitSynchronization(HandleId* handle_ids, i32 handles_count,
                                      i64 timeout, u64& out_handle_index) {
    LOG_DEBUG(
        HorizonKernel,
        "svcWaitSynchronization called (handles: 0x{}, count: {}, timeout: "
        "{})",
        (void*)handle_ids, handles_count, timeout);

    // TODO: implement
    LOG_FUNC_STUBBED(HorizonKernel);

    // HACK
    out_handle_index = 0;

    return RESULT_SUCCESS;
}

Result Kernel::svcArbitrateLock(u32 wait_tag, uptr mutex_addr, u32 self_tag) {
    LOG_DEBUG(HorizonKernel,
              "svcArbitrateLock called (wait: 0x{:08x}, mutex: 0x{:08x}, self: "
              "0x{:08x})",
              wait_tag, mutex_addr, self_tag);

    // TODO: implement
    LOG_FUNC_STUBBED(HorizonKernel);

    return RESULT_SUCCESS;
}

Result Kernel::svcArbitrateUnlock(uptr mutex_addr) {
    LOG_DEBUG(HorizonKernel, "svcArbitrateUnlock called (mutex: 0x{:08x})",
              mutex_addr);

    // TODO: implement
    LOG_FUNC_STUBBED(HorizonKernel);

    return RESULT_SUCCESS;
}

Result Kernel::svcWaitProcessWideKeyAtomic(uptr mutex_addr, uptr var_addr,
                                           u32 self_tag, i64 timeout) {
    LOG_DEBUG(
        HorizonKernel,
        "svcWaitProcessWideKeyAtomic called (mutex: 0x{:08x}, var: 0x{:08x}, "
        "self: 0x{:08x}, timeout: {})",
        mutex_addr, var_addr, self_tag, timeout);

    // TODO: implement
    LOG_FUNC_STUBBED(HorizonKernel);

    return RESULT_SUCCESS;
}

Result Kernel::svcSignalProcessWideKey(uptr addr, i32 v) {
    LOG_DEBUG(HorizonKernel,
              "svcSignalProcessWideKey called (addr: 0x{:08x}, value: {})",
              addr, v);

    // TODO: implement
    LOG_FUNC_STUBBED(HorizonKernel);

    return RESULT_SUCCESS;
}

void Kernel::svcGetSystemTick(u64& out_tick) {
    LOG_DEBUG(HorizonKernel, "svcGetSystemTick called");

    // TODO: implement
    LOG_FUNC_STUBBED(HorizonKernel);

    // HACK
    out_tick = 0x0000000000000000;
}

Result Kernel::svcConnectToNamedPort(const std::string& name,
                                     HandleId& out_session_handle_id) {
    LOG_DEBUG(HorizonKernel, "svcConnectToNamedPort called (name: {})", name);

    auto it = service_ports.find(name);
    if (it == service_ports.end()) {
        LOG_ERROR(HorizonKernel, "Unknown service name \"{}\"", name);
        return MAKE_KERNEL_RESULT(NotFound);
    }

    out_session_handle_id = AddHandle(it->second);

    return RESULT_SUCCESS;
}

Result Kernel::svcSendSyncRequest(HandleId session_handle_id) {
    LOG_DEBUG(HorizonKernel, "svcSendSyncRequest called (handle: 0x{:08x})",
              session_handle_id);

    auto service =
        static_cast<Services::ServiceBase*>(GetHandle(session_handle_id));

    // Request

    // HIPC header
    Hipc::ParsedRequest hipc_in =
        Hipc::parse_request(reinterpret_cast<void*>(tls_ptr));
    u8* in_ptr = align_ptr((u8*)hipc_in.data.data_words, 0x10);

    // Dispatch
    Services::Readers readers(mmu, hipc_in);
    Services::Writers writers(mmu, hipc_in, service_scratch_buffer,
                              service_scratch_buffer_objects,
                              service_scratch_buffer_move_handles,
                              service_scratch_buffer_copy_handles);
    auto command_type = static_cast<Cmif::CommandType>(hipc_in.meta.type);
    switch (command_type) {
    case Cmif::CommandType::Request:
        LOG_DEBUG(HorizonKernel, "COMMAND: Request");
        service->Request(readers, writers, [&](Services::ServiceBase* service) {
            HandleId handle_id = AddHandle(service);
            service->SetHandleId(handle_id);
            writers.move_handles_writer.Write(handle_id);
        });
        break;
    case Cmif::CommandType::Control:
        LOG_DEBUG(HorizonKernel, "COMMAND: Control");
        service->Control(readers, writers);
        break;
    default:
        LOG_WARNING(HorizonKernel, "Unknown command {}", command_type);
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
    auto response = Hipc::make_request(reinterpret_cast<void*>(tls_ptr), meta);

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

    // AppletMessage_FocusStateChanged for _appletReceiveMessage
    // AppletMessage_InFocus for _appletGetCurrentFocusState
    // TODO: no longer needed?
    /*
    static int num_executed = 0;
    num_executed++;
    Logging::log(Logging::Level::Debug, "NUM EXECUTED: %i", num_executed);
    if (num_executed == 25)
        *((u32*)(out_addr + sizeof(CmifDomainOutHeader) +
                 sizeof(CmifOutHeader))) = 15;
    if (num_executed == 26)
        *((u32*)(out_addr + sizeof(CmifDomainOutHeader) +
                 sizeof(CmifOutHeader))) = 1;
    */

    return RESULT_SUCCESS;
}

Result Kernel::svcGetThreadId(HandleId thread_handle_id, u64& out_thread_id) {
    LOG_DEBUG(HorizonKernel, "svcGetThreadId called (thread: 0x{:08x})",
              thread_handle_id);

    // TODO: implement
    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    out_thread_id = 0x0000000f;

    return RESULT_SUCCESS;
}

Result Kernel::svcBreak(BreakReason reason, uptr buffer_ptr,
                        usize buffer_size) {
    LOG_DEBUG(HorizonKernel,
              "svcBreak called (reason: {}, buffer ptr: 0x{:08x}, buffer "
              "size: 0x{:08x})",
              reason.type, buffer_ptr, buffer_size);

    if (buffer_ptr && buffer_size == 0x4) {
        LOG_DEBUG(HorizonKernel, "diagAbortWithResult (description: {})",
                  ((*(u32*)buffer_ptr) >> 9) & 0x1FFF);
    }

    if (!reason.notification_only)
        throw;

    return RESULT_SUCCESS;
}

Result Kernel::svcOutputDebugString(const char* str, usize len) {
    LOG_DEBUG(HorizonKernel, "svcOutputDebugString called");
    if (len != 0) {
        // TODO: handle differently
        LOG_INFO(HorizonKernel, "{}", std::string(str, len));
    }

    return RESULT_SUCCESS;
}

Result Kernel::svcGetInfo(InfoType info_type, HandleId handle_id,
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
        LOG_NOT_IMPLEMENTED(HorizonKernel, "AliasRegionAddress");
        // HACK
        out_info = STACK_REGION_BASE;
        return RESULT_SUCCESS;
    case InfoType::AliasRegionSize:
        LOG_NOT_IMPLEMENTED(HorizonKernel, "AliasRegionSize");
        // HACK
        out_info = STACK_REGION_SIZE;
        return RESULT_SUCCESS;
    case InfoType::HeapRegionAddress:
        out_info = HEAP_REGION_BASE;
        return RESULT_SUCCESS;
    case InfoType::HeapRegionSize:
        out_info = HEAP_REGION_SIZE;
        return RESULT_SUCCESS;
    case InfoType::AslrRegionAddress:
        out_info = ADDRESS_SPACE_BASE;
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
        out_info = 4u * 1024u * 1024u * 1024u;
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
        out_info = 0;
        return RESULT_SUCCESS;
    }
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
        LOG_WARNING(HorizonKernel, "Unimplemented info type {}", info_type);
        return MAKE_KERNEL_RESULT(InvalidEnumValue);
    }
}

void Kernel::SetHandle(HandleId handle_id, KernelHandle* handle) {
    handle_pool.GetObjectRef(handle_id) = handle;
}

HandleId Kernel::AddHandle(KernelHandle* handle) {
    HandleId handle_id = handle_pool.AllocateForIndex();
    handle_pool.GetObjectRef(handle_id) = handle;

    return handle_id;
}

HandleId Kernel::CreateSharedMemory(usize size) {
    auto handle_id = shared_memory_pool.AllocateForIndex();
    shared_memory_pool.GetObjectRef(handle_id) = new SharedMemory(size);

    return handle_id;
}

} // namespace Hydra::Horizon
