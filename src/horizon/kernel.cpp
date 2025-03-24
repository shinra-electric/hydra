#include "horizon/kernel.hpp"

#include "common/logging/log.hpp"
#include "horizon/cmif.hpp"
#include "horizon/const.hpp"
#include "horizon/hipc.hpp"
#include "horizon/services/service_base.hpp"
#include "hw/tegra_x1/cpu/cpu_base.hpp"
#include "hw/tegra_x1/cpu/memory.hpp"
#include "hw/tegra_x1/cpu/mmu_base.hpp"
#include "hw/tegra_x1/cpu/thread_base.hpp"

namespace Hydra::Horizon {

constexpr uptr STACK_MEM_BASE = 0x10000000;
constexpr usize STACK_MEM_SIZE = 0x2000000;

constexpr uptr KERNEL_MEM_BASE = 0xF0000000;
constexpr usize KERNEL_MEM_SIZE = 0x10000;

constexpr uptr TLS_MEM_BASE = 0x20000000;
constexpr usize TLS_MEM_SIZE = 0x20000;

constexpr uptr HEAP_MEM_BASE = 0x60000000;
constexpr usize DEFAULT_HEAP_MEM_SIZE = 0x1000000;
constexpr usize HEAP_MEM_ALIGNMENT = 0x200000;

// TODO: what is this?
constexpr uptr ASLR_MEM_BASE = 0x40000000;
constexpr usize ASLR_MEM_SIZE = 0x1000000;

constexpr uptr EXCEPTION_TRAMPOLINE_OFFSET = 0x800;

constexpr uptr CONFIG_ENTRIES_ADDR = ASLR_MEM_BASE; // TODO: where to put this?
constexpr uptr ARGV_ADDR = ASLR_MEM_BASE + 0x1000;  // TODO: where to put this?

const u32 exception_handler[] = {
    0xd41fffe2u, // hvc #0xFFFF
    0xd69f03e0u, // eret
    // 0xD2B00000, // mov x0, #0x80000000
    // 0xD61F0000, // br  x0
    // Shouldn't happen
    0xd4200000u, // brk #0
};

const u32 exception_trampoline[] = {
    0xd508831fu, // msr spsel, xzr

    // 0x910003e0,  // mov x0, sp
    // 0xd5384241,  // TODO
    // 0xd5384202,  // mrs x2, spsel
    // 0xD4200000u, // brk #0

    0xd69f03e0u, // eret
    // Shouldn't happen
    0xd4200000u, // brk #0
};

SINGLETON_DEFINE_GET_INSTANCE(Kernel, HorizonKernel, "Kernel")

Kernel::Kernel(HW::Bus& bus_, HW::TegraX1::CPU::MMUBase* mmu_)
    : bus{bus_}, mmu{mmu_} {
    SINGLETON_SET_INSTANCE(HorizonKernel, "Kernel");

    // Memory

    // Stack memory
    stack_mem =
        new HW::TegraX1::CPU::Memory(STACK_MEM_SIZE, Permission::ReadWrite);
    stack_mem->Clear();
    mmu->Map(STACK_MEM_BASE, stack_mem);

    // Kernel memory
    kernel_mem = new HW::TegraX1::CPU::Memory(KERNEL_MEM_SIZE,
                                              Permission::Execute, true);

    kernel_mem->Clear();
    for (u64 offset = 0; offset < 0x780; offset += 0x80) {
        memcpy(kernel_mem->GetPtrU8() + offset, exception_handler,
               sizeof(exception_handler));
    }
    memcpy(kernel_mem->GetPtrU8() + EXCEPTION_TRAMPOLINE_OFFSET,
           exception_trampoline, sizeof(exception_trampoline));

    mmu->Map(KERNEL_MEM_BASE, kernel_mem);

    // TLS memory
    tls_mem = new HW::TegraX1::CPU::Memory(TLS_MEM_SIZE, Permission::ReadWrite);
    tls_mem->Clear();
    mmu->Map(TLS_MEM_BASE, tls_mem);

    // ASLR memory
    aslr_mem =
        new HW::TegraX1::CPU::Memory(ASLR_MEM_SIZE, Permission::ReadWrite);
    aslr_mem->Clear();
    mmu->Map(ASLR_MEM_BASE, aslr_mem);

    // Heap memory
    heap_mem = new HW::TegraX1::CPU::Memory(DEFAULT_HEAP_MEM_SIZE,
                                            Permission::ReadWrite);
    heap_mem->Clear();
    mmu->Map(HEAP_MEM_BASE, heap_mem);
}

Kernel::~Kernel() {
    delete stack_mem;
    delete kernel_mem;
    delete tls_mem;
    delete aslr_mem;
    for (auto executable_mem : executable_memories)
        delete executable_mem;
    // delete bss_mem;
    delete heap_mem;

    SINGLETON_UNSET_INSTANCE();
}

void Kernel::ConfigureThread(HW::TegraX1::CPU::ThreadBase* thread) {
    thread->Configure([&](HW::TegraX1::CPU::ThreadBase* thread,
                          u64 id) { return SupervisorCall(thread, id); },
                      KERNEL_MEM_BASE, TLS_MEM_BASE,
                      STACK_MEM_BASE + STACK_MEM_SIZE,
                      KERNEL_MEM_BASE + EXCEPTION_TRAMPOLINE_OFFSET);
}

void Kernel::ConfigureMainThread(HW::TegraX1::CPU::ThreadBase* thread) {
    ConfigureThread(thread);

    // Set initial PC
    ASSERT_DEBUG(entry_point != 0x0, HorizonKernel, "Invalid entry point");
    thread->SetRegPC(entry_point);

    // Set arguments
    // TODO: handle this in the loader

    // From https://github.com/switchbrew/libnx

    // NSO
    // TODO: if NSO
    if (false) {
        thread->SetRegX(0, 0x0);
        thread->SetRegX(1,
                        0x0000000f); // TODO: what thread handle should be used?
    }

    // NRO
    // TODO: if NRO
    if (true) {
        thread->SetRegX(0, CONFIG_ENTRIES_ADDR);
        thread->SetRegX(1, UINT64_MAX);

        // Args
        std::string args = fmt::format("\"{}\"", "/rom.nro");
        char* argv = reinterpret_cast<char*>(mmu->UnmapAddr(ARGV_ADDR));
        memcpy(argv, args.c_str(), args.size());
        argv[args.size()] = '\0';

#define ADD_ENTRY(t, f, value0, value1)                                        \
    {                                                                          \
        entry->type = ConfigEntryType::t;                                      \
        entry->flags = ConfigEntryFlag::f;                                     \
        entry->values[0] = value0;                                             \
        entry->values[1] = value1;                                             \
        entry++;                                                               \
    }
#define ADD_ENTRY_MANDATORY(t, value0, value1)                                 \
    ADD_ENTRY(t, None, value0, value1)
#define ADD_ENTRY_NON_MANDATORY(t, value0, value1)                             \
    ADD_ENTRY(t, IsMandatory, value0, value1)

        // Entries
        ConfigEntry* entry =
            reinterpret_cast<ConfigEntry*>(mmu->UnmapAddr(CONFIG_ENTRIES_ADDR));

        ADD_ENTRY_MANDATORY(MainThreadHandle, 0x0000000f,
                            0); // TODO: what thread handle should be used?
        ADD_ENTRY_MANDATORY(Argv, 0, ARGV_ADDR); // TODO: what should value0 be?
        // TODO: supply the actual availability
        ADD_ENTRY_MANDATORY(SyscallAvailableHint, UINT64_MAX, UINT64_MAX);
        ADD_ENTRY_MANDATORY(SyscallAvailableHint2, UINT64_MAX, 0);
        ADD_ENTRY_MANDATORY(EndOfList, 0, 0);

#undef ADD_ENTRY_NON_MANDATORY
#undef ADD_ENTRY_MANDATORY
#undef ADD_ENTRY
    }

    // User-mode exception entry
    // TODO: if user-mode exception
    if (false) {
        // TODO: what is this?
    }
}

HW::TegraX1::CPU::Memory* Kernel::CreateExecutableMemory(usize size,
                                                         uptr& out_base) {
    auto mem = new HW::TegraX1::CPU::Memory(
        size, Permission::ReadExecute |
                  Permission::Write); // TODO: don't give write permissions
    mem->Clear();
    mmu->Map(executable_mem_base, mem);
    out_base = executable_mem_base;
    executable_mem_base += mem->GetSize();

    return mem;
}

bool Kernel::SupervisorCall(HW::TegraX1::CPU::ThreadBase* thread, u64 id) {
    Result res;
    u32 tmpU32;
    u64 tmpU64;
    uptr tmpUPTR;
    HandleId tmpHandleId;
    switch (id) {
    case 0x1:
        res = svcSetHeapSize(&tmpUPTR, thread->GetRegX(1));
        thread->SetRegX(0, res);
        thread->SetRegX(1, tmpUPTR);
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
    case 0x6:
        res = svcQueryMemory(
            reinterpret_cast<MemoryInfo*>(mmu->UnmapAddr(thread->GetRegX(0))),
            &tmpU32, thread->GetRegX(2));
        thread->SetRegX(0, res);
        thread->SetRegX(1, tmpU32);
        break;
    case 0x7:
        svcExitProcess();
        return false;
    case 0xb:
        svcSleepThread(bit_cast<i64>(thread->GetRegX(0)));
        break;
    case 0x13:
        res = svcMapSharedMemory(thread->GetRegX(0), thread->GetRegX(1),
                                 thread->GetRegX(2),
                                 static_cast<Permission>(thread->GetRegX(3)));
        thread->SetRegX(0, res);
        break;
    case 0x15:
        res = svcCreateTransferMemory(
            &tmpHandleId, thread->GetRegX(1), thread->GetRegX(2),
            static_cast<Permission>(thread->GetRegX(3)));
        thread->SetRegX(0, res);
        thread->SetRegX(1, tmpHandleId);
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
            tmpU64, reinterpret_cast<HandleId*>(thread->GetRegX(1)),
            bit_cast<i64>(thread->GetRegX(2)),
            bit_cast<i64>(thread->GetRegX(3)));
        thread->SetRegX(0, res);
        thread->SetRegX(1, tmpU64);
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
    case 0x1f:
        res = svcConnectToNamedPort(
            &tmpHandleId,
            reinterpret_cast<const char*>(mmu->UnmapAddr(thread->GetRegX(1))));
        thread->SetRegX(0, res);
        thread->SetRegX(1, tmpHandleId);
        break;
    case 0x21:
        res = svcSendSyncRequest(thread->GetRegX(0));
        thread->SetRegX(0, res);
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
        res = svcGetInfo(&tmpU64, static_cast<InfoType>(thread->GetRegX(1)),
                         thread->GetRegX(2), thread->GetRegX(3));
        thread->SetRegX(0, res);
        thread->SetRegX(1, tmpU64);
        break;
    default:
        LOG_WARNING(HorizonKernel, "Unimplemented SVC 0x{:08x}", id);
        res = MAKE_KERNEL_RESULT(NotImplemented);
        thread->SetRegX(0, res);
        break;
    }

    return true;
}

Result Kernel::svcSetHeapSize(uptr* out, usize size) {
    LOG_DEBUG(HorizonKernel, "svcSetHeapSize called (size: 0x{:08x})", size);

    if ((size % HEAP_MEM_ALIGNMENT) != 0)
        return MAKE_KERNEL_RESULT(InvalidSize); // TODO: correct?

    if (size != heap_mem->GetSize()) {
        heap_mem->Resize(size);
        mmu->Remap(HEAP_MEM_BASE);
    }

    *out = HEAP_MEM_BASE;

    return RESULT_SUCCESS;
}

Result Kernel::svcSetMemoryPermission(uptr addr, usize size,
                                      Permission permission) {
    LOG_DEBUG(
        HorizonKernel,
        "svcSetMemoryPermission called (addr: 0x{:08x}, size: 0x{:08x}, perm: "
        "{})",
        addr, size, permission);

    uptr base;
    HW::TegraX1::CPU::Memory* mem = mmu->FindAddrImpl(addr, base);
    if (!mem) {
        // TODO: check
        return MAKE_KERNEL_RESULT(InvalidAddress);
    }

    mem->SetPermission(permission);
    // TODO: uncomment
    // cpu->ReprotectMemory(mem);

    // TODO: implement
    LOG_WARNING(HorizonKernel, "Not implemented");

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
    LOG_WARNING(HorizonKernel, "Not implemented");

    return RESULT_SUCCESS;
}

Result Kernel::svcQueryMemory(MemoryInfo* out_mem_info, u32* out_page_info,
                              uptr addr) {
    LOG_DEBUG(HorizonKernel, "svcQueryMemory called (addr: 0x{:08x})", addr);

    // TODO: implement
    LOG_WARNING(HorizonKernel, "Not implemented");

    // HACK
    if (addr == 0x0) {
        LOG_WARNING(HorizonKernel, "Address is 0x0");

        *out_mem_info = MemoryInfo{
            .addr = 0x0, .size = 0x10000000,
            //.type = 0x3,
            // TODO: attr
            //.perm = Permission::ReadExecute,
            // TODO: ipc_ref_count
            // TODO: device_ref_count
        };

        *out_page_info = 0;

        return RESULT_SUCCESS;
    }

    uptr base;
    HW::TegraX1::CPU::Memory* mem = mmu->FindAddrImpl(addr, base);
    if (!mem) {
        // TODO: check
        return MAKE_KERNEL_RESULT(InvalidAddress);
    }

    *out_mem_info = MemoryInfo{
        .addr = base, // TODO: check
        .size = mem->GetSize(),
        // TODO: type
        // TODO: attr
        .perm = mem->GetPermission(),
        // TODO: ipc_ref_count
        // TODO: device_ref_count
    };

    // TODO: out_page_info
    *out_page_info = 0;

    return RESULT_SUCCESS;
}

void Kernel::svcExitProcess() {
    LOG_DEBUG(HorizonKernel, "svcExitProcess called");
}

void Kernel::svcSleepThread(i64 nano) {
    LOG_DEBUG(HorizonKernel, "svcSleepThread called (nano: 0x{:08x})", nano);

    std::this_thread::sleep_for(std::chrono::nanoseconds(nano));
}

Result Kernel::svcMapSharedMemory(HandleId handle_id, uptr addr, usize size,
                                  Permission permission) {
    LOG_DEBUG(
        HorizonKernel,
        "svcMapSharedMemory called (handle: 0x{:08x}, addr: 0x{:08x}, size: "
        "0x{:08x}, perm: {})",
        handle_id, addr, size, permission);

    // Map
    auto shared_mem = shared_memory_pool.GetObjectRef(handle_id);
    shared_mem.MapToRange(range(addr, size));

    return RESULT_SUCCESS;
}

Result Kernel::svcCreateTransferMemory(HandleId* out_handle_id, uptr addr,
                                       u64 size, Permission permission) {
    LOG_DEBUG(
        HorizonKernel,
        "svcCreateTransferMemory called (address: 0x{:08x}, size: 0x{:08x}, "
        "perm: {})",
        addr, size, permission);

    *out_handle_id = AddHandle(new TransferMemory(addr, size, permission));

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
    LOG_WARNING(HorizonKernel, "Not implemented");

    return RESULT_SUCCESS;
}

Result Kernel::svcWaitSynchronization(u64& handle_index, HandleId* handle_ids,
                                      i32 handles_count, i64 timeout) {
    LOG_DEBUG(
        HorizonKernel,
        "svcWaitSynchronization called (handles: 0x{}, count: {}, timeout: "
        "{})",
        (void*)handle_ids, handles_count, timeout);

    // TODO: implement
    LOG_WARNING(HorizonKernel, "Not implemented");

    // HACK
    handle_index = 0;

    return RESULT_SUCCESS;
}

Result Kernel::svcArbitrateLock(u32 wait_tag, uptr mutex_addr, u32 self_tag) {
    LOG_DEBUG(HorizonKernel,
              "svcArbitrateLock called (wait: 0x{:08x}, mutex: 0x{:08x}, self: "
              "0x{:08x})",
              wait_tag, mutex_addr, self_tag);

    // TODO: implement
    LOG_WARNING(HorizonKernel, "Not implemented");

    return RESULT_SUCCESS;
}

Result Kernel::svcArbitrateUnlock(uptr mutex_addr) {
    LOG_DEBUG(HorizonKernel, "svcArbitrateUnlock called (mutex: 0x{:08x})",
              mutex_addr);

    // TODO: implement
    LOG_WARNING(HorizonKernel, "Not implemented");

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
    LOG_WARNING(HorizonKernel, "Not implemented");

    return RESULT_SUCCESS;
}

Result Kernel::svcSignalProcessWideKey(uptr addr, i32 v) {
    LOG_DEBUG(HorizonKernel,
              "svcSignalProcessWideKey called (addr: 0x{:08x}, value: {})",
              addr, v);

    // TODO: implement
    LOG_WARNING(HorizonKernel, "Not implemented");

    return RESULT_SUCCESS;
}

Result Kernel::svcConnectToNamedPort(HandleId* out_handle_id,
                                     const std::string& name) {
    LOG_DEBUG(HorizonKernel, "svcConnectToNamedPort called (name: {})", name);

    auto it = service_ports.find(name);
    if (it == service_ports.end()) {
        LOG_ERROR(HorizonKernel, "Unknown service name \"{}\"", name);
        return MAKE_KERNEL_RESULT(NotFound);
    }

    *out_handle_id = AddHandle(it->second);

    return RESULT_SUCCESS;
}

Result Kernel::svcSendSyncRequest(HandleId handle_id) {
    LOG_DEBUG(HorizonKernel, "svcSendSyncRequest called (handle: 0x{:08x})",
              handle_id);

    auto service = static_cast<Services::ServiceBase*>(GetHandle(handle_id));
    u8* tls_ptr = tls_mem->GetPtrU8();

    // Request

    // HIPC header
    Hipc::ParsedRequest hipc_in = Hipc::parse_request(tls_ptr);
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

Result Kernel::svcGetInfo(u64* out, InfoType info_type, HandleId handle_id,
                          u64 info_sub_type) {
    LOG_DEBUG(HorizonKernel,
              "svcGetInfo called (type: {}, handle: 0x{:08x}, subtype: {})",
              info_type, handle_id, info_sub_type);

    switch (info_type) {
    case InfoType::AliasRegionAddress:
        LOG_WARNING(HorizonKernel, "Not implemented");
        // HACK
        *out = 0;
        return RESULT_SUCCESS;
    case InfoType::AliasRegionSize:
        LOG_WARNING(HorizonKernel, "Not implemented");
        // HACK
        *out = 0;
        return RESULT_SUCCESS;
    case InfoType::HeapRegionAddress:
        *out = HEAP_MEM_BASE;
        return RESULT_SUCCESS;
    case InfoType::HeapRegionSize:
        *out = heap_mem->GetSize();
        return RESULT_SUCCESS;
    case InfoType::AslrRegionAddress:
        *out = ASLR_MEM_BASE;
        return RESULT_SUCCESS;
    case InfoType::AslrRegionSize:
        *out = aslr_mem->GetSize();
        return RESULT_SUCCESS;
    case InfoType::StackRegionAddress:
        *out = STACK_MEM_BASE;
        return RESULT_SUCCESS;
    case InfoType::StackRegionSize:
        *out = stack_mem->GetSize();
        return RESULT_SUCCESS;
    case InfoType::TotalMemorySize:
        // TODO: what should this be?
        *out = 4u * 1024u * 1024u * 1024u;
        return RESULT_SUCCESS;
    case InfoType::UsedMemorySize: {
        // TODO: correct?
        usize size = stack_mem->GetSize() + kernel_mem->GetSize() +
                     tls_mem->GetSize() + aslr_mem->GetSize() +
                     heap_mem->GetSize();
        for (auto executable_mem : executable_memories)
            size += executable_mem->GetSize();
        *out = size;
        return RESULT_SUCCESS;
    }
    case InfoType::RandomEntropy:
        // TODO: correct?
        // TODO: subtype 0-3
        *out = rand();
        return RESULT_SUCCESS;
    case InfoType::AliasRegionExtraSize:
        LOG_WARNING(HorizonKernel, "Not implemented");
        // HACK
        *out = 0;
        return RESULT_SUCCESS;
    default:
        LOG_WARNING(HorizonKernel, "Unimplemented info type {}", info_type);
        return MAKE_KERNEL_RESULT(NotImplemented);
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

HandleId Kernel::CreateSharedMemory() {
    return shared_memory_pool.AllocateForIndex();
}

} // namespace Hydra::Horizon
