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

#define STACK_MEM_BASE 0x10000000
#define STACK_MEM_SIZE 0x2000000

#define KERNEL_MEM_BASE 0xF0000000
#define KERNEL_MEM_SIZE 0x10000

#define TLS_MEM_BASE 0x20000000
#define TLS_MEM_SIZE 0x20000

#define ROM_MEM_BASE 0x80000000

#define HEAP_MEM_BASE 0x60000000
#define DEFAULT_HEAP_MEM_SIZE 0x1000000
#define HEAP_MEM_ALIGNMENT 0x00200000

// TODO: what is this?
#define ASLR_MEM_BASE 0x40000000
#define ASLR_MEM_SIZE 0x1000000

#define EXCEPTION_TRAMPOLINE_OFFSET 0x800

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

static Kernel* s_instance = nullptr;

Kernel& Kernel::GetInstance() { return *s_instance; }

Kernel::Kernel(HW::Bus& bus_, HW::TegraX1::CPU::MMUBase* mmu_)
    : bus{bus_}, mmu{mmu_} {
    ASSERT(s_instance == nullptr, HorizonKernel,
           "Horizon kernel already exists");
    s_instance = this;

    // Memory

    // Stack memory
    stack_mem =
        new HW::TegraX1::CPU::Memory(STACK_MEM_SIZE, Permission::ReadWrite);
    stack_mem->Clear();
    mmu->Map(stack_mem, STACK_MEM_BASE);

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

    mmu->Map(kernel_mem, KERNEL_MEM_BASE);

    // TLS memory
    tls_mem = new HW::TegraX1::CPU::Memory(TLS_MEM_SIZE, Permission::ReadWrite);
    tls_mem->Clear();
    mmu->Map(tls_mem, TLS_MEM_BASE);

    // ASLR memory
    aslr_mem =
        new HW::TegraX1::CPU::Memory(ASLR_MEM_SIZE, Permission::ReadWrite);
    aslr_mem->Clear();
    mmu->Map(aslr_mem, ASLR_MEM_BASE);

    // Heap memory
    heap_mem = new HW::TegraX1::CPU::Memory(DEFAULT_HEAP_MEM_SIZE,
                                            Permission::ReadWrite);
    heap_mem->Clear();
    mmu->Map(heap_mem, HEAP_MEM_BASE);
}

Kernel::~Kernel() {
    delete stack_mem;
    delete kernel_mem;
    delete tls_mem;
    delete aslr_mem;
    if (rom_mem)
        delete rom_mem;
    // delete bss_mem;
    delete heap_mem;

    s_instance = nullptr;
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
    thread->SetRegPC(ROM_MEM_BASE + rom_text_offset);

    // Set arguments

    // From https://github.com/switchbrew/libnx

    // NSO
    // TODO

    // NRO
    // TODO: should be ptr to env context
    thread->SetRegX(0, 0);
    thread->SetRegX(1, 0x0000000F);

    // User-mode exception entry
    // TODO: what is this?
}

void Kernel::LoadROM(Rom* rom) {
    if (rom_mem) {
        mmu->Unmap(ROM_MEM_BASE);
        delete rom_mem;
    }

    // ROM memory
    rom_mem = new HW::TegraX1::CPU::Memory(
        rom->GetRom().size(),
        Permission::ReadExecute |
            Permission::Write); // TODO: should write be possible?
    rom_mem->Clear();
    memcpy(rom_mem->GetPtrU8(), rom->GetRom().data(), rom->GetRom().size());
    mmu->Map(rom_mem, ROM_MEM_BASE);

    rom_text_offset = rom->GetTextOffset();
}

bool Kernel::SupervisorCall(HW::TegraX1::CPU::ThreadBase* thread, u64 id) {
    Result res;
    u32 tmpU32;
    u64 tmpU64;
    uptr tmpUPTR;
    Handle tmpHandle;
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
            &tmpHandle, thread->GetRegX(1), thread->GetRegX(2),
            static_cast<Permission>(thread->GetRegX(3)));
        thread->SetRegX(0, res);
        thread->SetRegX(1, tmpHandle);
        break;
    case 0x16:
        res = svcCloseHandle(thread->GetRegX(0));
        thread->SetRegX(0, res);
        break;
    case 0x18:
        res = svcWaitSynchronization(
            tmpU64, reinterpret_cast<Handle*>(thread->GetRegX(1)),
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
    case 0x1f:
        res = svcConnectToNamedPort(
            &tmpHandle,
            reinterpret_cast<const char*>(mmu->UnmapAddr(thread->GetRegX(1))));
        thread->SetRegX(0, res);
        thread->SetRegX(1, tmpHandle);
        break;
    case 0x21:
        res = svcSendSyncRequest(thread->GetRegX(0));
        thread->SetRegX(0, res);
        break;
    case 0x26:
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
    HW::TegraX1::CPU::Memory* mem = mmu->FindMemoryForAddr(addr, base);
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

    uptr base;
    HW::TegraX1::CPU::Memory* mem = mmu->FindMemoryForAddr(addr, base);
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

Result Kernel::svcMapSharedMemory(Handle handle, uptr addr, usize size,
                                  Permission permission) {
    LOG_DEBUG(
        HorizonKernel,
        "svcMapSharedMemory called (handle: 0x{:08x}, addr: 0x{:08x}, size: "
        "0x{:08x}, perm: {})",
        handle, addr, size, (u32)permission);

    // Map
    auto shared_mem = shared_memories.GetObjectRef(handle);
    shared_mem.MapToRange(range(addr, size));

    return RESULT_SUCCESS;
}

Result Kernel::svcCreateTransferMemory(Handle* out, uptr address, u64 size,
                                       Permission permission) {
    LOG_DEBUG(
        HorizonKernel,
        "svcCreateTransferMemory called (address: 0x{:08x}, size: 0x{:08x}, "
        "perm: {})",
        address, size, permission);

    // TODO: implement
    LOG_WARNING(HorizonKernel, "Not implemented");

    return RESULT_SUCCESS;
}

Result Kernel::svcCloseHandle(Handle handle) {
    LOG_DEBUG(HorizonKernel, "svcCloseHandle called (handle: 0x{:08x})",
              handle);

    // TODO: uncomment
    // delete GetService(handle);

    return RESULT_SUCCESS;
}

Result Kernel::svcWaitSynchronization(u64& handle_index, Handle* handles_ptr,
                                      i32 handles_count, i64 timeout) {
    LOG_DEBUG(
        HorizonKernel,
        "svcWaitSynchronization called (handles: 0x{}, count: {}, timeout: "
        "{})",
        (void*)handles_ptr, handles_count, timeout);

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

Result Kernel::svcConnectToNamedPort(Handle* out, const std::string& name) {
    LOG_DEBUG(HorizonKernel, "svcConnectToNamedPort called (name: {})", name);

    auto it = service_ports.find(name);
    if (it == service_ports.end()) {
        LOG_ERROR(HorizonKernel, "Unknown service name \"{}\"", name);
        return MAKE_KERNEL_RESULT(NotFound);
    }

    *out = AddService(it->second);

    return RESULT_SUCCESS;
}

Result Kernel::svcSendSyncRequest(Handle session_handle) {
    LOG_DEBUG(HorizonKernel, "svcSendSyncRequest called (session: 0x{:08x})",
              session_handle);

    Services::ServiceBase* service = GetService(session_handle);
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
    switch (static_cast<Cmif::CommandType>(hipc_in.meta.type)) {
    case Cmif::CommandType::Request:
        LOG_DEBUG(HorizonKernel, "COMMAND: Request");
        service->Request(readers, writers, [&](Services::ServiceBase* service) {
            Handle handle = AddService(service);
            writers.move_handles_writer.Write(handle);
        });
        break;
    case Cmif::CommandType::Control:
        LOG_DEBUG(HorizonKernel, "COMMAND: Control");
        service->Control(readers.reader, writers.writer);
        break;
    default:
        LOG_WARNING(HorizonKernel, "Unknown command {}", hipc_in.meta.type);
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

Result Kernel::svcGetInfo(u64* out, InfoType info_type, Handle handle,
                          u64 info_sub_type) {
    LOG_DEBUG(HorizonKernel,
              "svcGetInfo called (type: {}, handle: 0x{:08x}, subtype: {})",
              info_type, handle, info_sub_type);

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
    case InfoType::UsedMemorySize:
        // TODO: correct?
        *out = stack_mem->GetSize() + kernel_mem->GetSize() +
               tls_mem->GetSize() + aslr_mem->GetSize() + rom_mem->GetSize() +
               heap_mem->GetSize();
        return RESULT_SUCCESS;
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

void Kernel::SetService(Handle handle, Services::ServiceBase* service) {
    service_pool.GetObjectRef(handle) = service;
    service->SetHandle(handle);
}

Handle Kernel::AddService(Services::ServiceBase* service) {
    Handle handle = service_pool.AllocateForIndex();
    SetService(handle, service);

    return handle;
}

Handle Kernel::CreateSharedMemory() {
    return shared_memories.AllocateForIndex();
}

} // namespace Hydra::Horizon
