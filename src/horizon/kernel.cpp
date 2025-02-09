#include "horizon/kernel.hpp"

#include "horizon/cmif.hpp"
#include "horizon/hipc.hpp"
#include "horizon/services/sm.hpp"
#include "hw/cpu/cpu.hpp"
#include "hw/mmu/memory.hpp"
#include "hw/mmu/mmu.hpp"

namespace Hydra::Horizon {

// HACK: no idea what I am doing, but _impure_ptr seems to point there
// #define BSS_MEM_BASE 0x00010000

#define STACK_MEM_BASE 0x01000000
#define STACK_SIZE 0x200000
// For EL0 and EL1
#define STACK_MEM_SIZE (STACK_SIZE * 2)

#define KERNEL_MEM_BASE 0xF0000000
#define KERNEL_MEM_SIZE 0x10000

#define TLS_MEM_BASE 0x02000000
#define TLS_MEM_SIZE 0x20000

#define ROM_MEM_BASE 0x80000000

#define HEAP_MEM_BASE 0x20000000
#define DEFAULT_HEAP_MEM_SIZE 0x1000000
#define HEAP_MEM_ALIGNMENT 0x00200000

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

Kernel::Kernel() {
    // Memory

    // Stack memory
    stack_mem = new HW::MMU::Memory(STACK_MEM_BASE, STACK_MEM_SIZE,
                                    Permission::ReadWrite);
    stack_mem->Clear();

    // Kernel memory
    kernel_mem = new HW::MMU::Memory(KERNEL_MEM_BASE, KERNEL_MEM_SIZE,
                                     Permission::Execute);
    kernel_mem->Clear();
    for (u64 offset = 0; offset < 0x780; offset += 0x80) {
        memcpy(kernel_mem->GetPtrU8() + offset, exception_handler,
               sizeof(exception_handler));
    }
    memcpy(kernel_mem->GetPtrU8() + 0x800, exception_trampoline,
           sizeof(exception_trampoline));

    // TLS memory
    tls_mem =
        new HW::MMU::Memory(TLS_MEM_BASE, TLS_MEM_SIZE, Permission::ReadWrite);
    tls_mem->Clear();

    // Heap memory
    heap_mem = new HW::MMU::Memory(HEAP_MEM_BASE, DEFAULT_HEAP_MEM_SIZE,
                                   Permission::ReadWrite);
    heap_mem->Clear();
}

Kernel::~Kernel() {
    delete stack_mem;
    delete kernel_mem;
    delete tls_mem;
    if (rom_mem)
        delete rom_mem;
    // delete bss_mem;
    delete heap_mem;
}

void Kernel::SetMMU(HW::MMU::MMUBase* mmu_) {
    mmu = mmu_;

    mmu->MapMemory(stack_mem);
    mmu->MapMemory(kernel_mem);
    mmu->MapMemory(tls_mem);
    mmu->MapMemory(heap_mem);
}

void Kernel::LoadROM(Rom* rom) {
    if (rom_mem) {
        mmu->UnmapMemory(rom_mem);
        delete rom_mem;
    }

    rom_mem = new HW::MMU::Memory(
        ROM_MEM_BASE, rom->GetRom().size(),
        Permission::ReadExecute |
            Permission::Write); // TODO: should write be possible?
    rom_mem->Clear();

    memcpy(rom_mem->GetPtrU8(), rom->GetRom().data(), rom->GetRom().size());

    // Text
    // memcpy(rom_mem->GetPtr(), rom->GetText().data(), rom->GetText().size());

    // RO data
    // memcpy(rom_mem->GetPtr() + RO_DATA_OFFSET, rom->GetRoData().data(),
    //       rom->GetRoData().size());

    // BSS memory
    // bss_mem = new HW::MMU::Memory(BSS_MEM_BASE, rom->GetBssSize(),
    //                              Permission::ReadWrite);
    // bss_mem->Clear();

    mmu->MapMemory(rom_mem);
    // mmu->MapMemory(horizon.GetKernel().GetBssMemory());
}

bool Kernel::SupervisorCall(HW::CPU::CPUBase* cpu, u64 id) {
    Result res;
    u32 tmpU32;
    u64 tmpU64;
    uptr tmpUPTR;
    Handle tmpHandle;
    switch (id) {
    case 0x1:
        res = svcSetHeapSize(&tmpUPTR, cpu->GetRegX(1));
        cpu->SetRegX(0, res);
        cpu->SetRegX(1, tmpUPTR);
        break;
    case 0x2:
        res = svcSetMemoryPermission(cpu->GetRegX(0), cpu->GetRegX(1),
                                     static_cast<Permission>(cpu->GetRegX(2)));
        cpu->SetRegX(0, res);
        break;
    case 0x3:
        res = svcSetMemoryAttribute(cpu->GetRegX(0), cpu->GetRegX(1),
                                    cpu->GetRegX(2), cpu->GetRegX(3));
        cpu->SetRegX(0, res);
        break;
    case 0x6:
        res = svcQueryMemory(
            reinterpret_cast<MemoryInfo*>(mmu->UnmapPtr(cpu->GetRegX(0))),
            &tmpU32, cpu->GetRegX(2));
        cpu->SetRegX(0, res);
        cpu->SetRegX(1, tmpU32);
        break;
    case 0x7:
        svcExitProcess();
        return false;
    case 0xb:
        svcSleepThread(bit_cast<i64>(cpu->GetRegX(0)));
        break;
    case 0x13:
        res = svcMapSharedMemory(cpu->GetRegX(0), cpu->GetRegX(1),
                                 cpu->GetRegX(2),
                                 static_cast<Permission>(cpu->GetRegX(3)));
        cpu->SetRegX(0, res);
        break;
    case 0x15:
        res = svcCreateTransferMemory(&tmpHandle, cpu->GetRegX(1),
                                      cpu->GetRegX(2),
                                      static_cast<Permission>(cpu->GetRegX(3)));
        cpu->SetRegX(0, res);
        cpu->SetRegX(1, tmpHandle);
        break;
    case 0x16:
        res = svcCloseHandle(cpu->GetRegX(0));
        cpu->SetRegX(0, res);
        break;
    case 0x18:
        res = svcWaitSynchronization(
            tmpU64, reinterpret_cast<Handle*>(cpu->GetRegX(1)),
            bit_cast<i64>(cpu->GetRegX(2)), bit_cast<i64>(cpu->GetRegX(3)));
        cpu->SetRegX(0, res);
        cpu->SetRegX(1, tmpU64);
        break;
    case 0x1a:
        res =
            svcArbitrateLock(cpu->GetRegX(0), cpu->GetRegX(1), cpu->GetRegX(2));
        cpu->SetRegX(0, res);
        break;
    case 0x1b:
        res = svcArbitrateUnlock(cpu->GetRegX(0));
        cpu->SetRegX(0, res);
        break;
    case 0x1c:
        res = svcWaitProcessWideKeyAtomic(cpu->GetRegX(0), cpu->GetRegX(1),
                                          cpu->GetRegX(2),
                                          bit_cast<i64>(cpu->GetRegX(3)));
        cpu->SetRegX(0, res);
        break;
    case 0x1f:
        res = svcConnectToNamedPort(
            &tmpHandle,
            reinterpret_cast<const char*>(mmu->UnmapPtr(cpu->GetRegX(1))));
        cpu->SetRegX(0, res);
        cpu->SetRegX(1, tmpHandle);
        break;
    case 0x21:
        res = svcSendSyncRequest(cpu->GetRegX(0));
        cpu->SetRegX(0, res);
        break;
    case 0x26:
        res = svcBreak(BreakReason(cpu->GetRegX(0)),
                       mmu->UnmapPtr(cpu->GetRegX(1)), cpu->GetRegX(2));
        cpu->SetRegX(0, res);
        break;
    case 0x27:
        res = svcOutputDebugString(
            reinterpret_cast<const char*>(mmu->UnmapPtr(cpu->GetRegX(0))),
            cpu->GetRegX(1));
        cpu->SetRegX(0, res);
        break;
    case 0x29:
        res = svcGetInfo(
            &tmpU64, Info(cpu->GetRegX(1), cpu->GetRegX(2), cpu->GetRegX(3)));
        cpu->SetRegX(0, res);
        cpu->SetRegX(1, tmpU64);
        break;
    default:
        printf("Unimplemented SVC 0x%08llx\n", id);
        res = MAKE_KERNEL_RESULT(NotImplemented);
        cpu->SetRegX(0, res);
        break;
    }

    return true;
}

Result Kernel::svcSetHeapSize(uptr* out, usize size) {
    printf("svcSetHeapSize called (size: 0x%08zx)\n", size);

    if ((size % HEAP_MEM_ALIGNMENT) != 0)
        return MAKE_KERNEL_RESULT(InvalidSize); // TODO: correct?

    if (size != heap_mem->GetSize()) {
        heap_mem->Resize(size);
        mmu->RemapMemory(heap_mem);
    }

    *out = heap_mem->GetBase();

    return RESULT_SUCCESS;
}

Result Kernel::svcSetMemoryPermission(uptr addr, usize size,
                                      Permission permission) {
    printf("svcSetMemoryPermission called (addr: 0x%08lx, size: 0x%08zx, perm: "
           "%u)\n",
           addr, size, (u32)permission);

    HW::MMU::Memory* mem = mmu->UnmapPtrToMemory(addr);
    if (!mem) {
        // TODO: check
        return MAKE_KERNEL_RESULT(InvalidAddress);
    }

    mem->SetPermission(permission);
    // TODO: uncomment
    // cpu->ReprotectMemory(mem);

    return RESULT_SUCCESS;
}

Result Kernel::svcSetMemoryAttribute(uptr addr, usize size, u32 mask,
                                     u32 value) {
    printf("svcSetMemoryAttribute called (addr: 0x%08lx, size: 0x%08zx, mask: "
           "0x%08x, value: 0x%08x)\n",
           addr, size, mask, value);

    // TODO: implement
    printf("Not implemented\n");

    return RESULT_SUCCESS;
}

Result Kernel::svcQueryMemory(MemoryInfo* out_mem_info, u32* out_page_info,
                              uptr addr) {
    printf("svcQueryMemory called (addr: 0x%08lx)\n", addr);

    HW::MMU::Memory* mem = mmu->UnmapPtrToMemory(addr);
    if (!mem) {
        // TODO: check
        return MAKE_KERNEL_RESULT(InvalidAddress);
    }

    *out_mem_info = MemoryInfo{
        .addr = mem->GetBase(), // TODO: check
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

void Kernel::svcExitProcess() { printf("svcExitProcess called\n"); }

void Kernel::svcSleepThread(i64 nano) {
    printf("svcSleepThread called (nano: %lld)\n", nano);

    std::this_thread::sleep_for(std::chrono::nanoseconds(nano));
}

Result Kernel::svcMapSharedMemory(Handle handle, uptr addr, usize size,
                                  Permission permission) {
    printf("svcMapSharedMemory called (handle: 0x%08x, addr: 0x%08lx, size: "
           "0x%08zx, perm: %u)\n",
           handle, addr, size, (u32)permission);

    // TODO: implement
    printf("Not implemented\n");

    return RESULT_SUCCESS;
}

Result Kernel::svcCreateTransferMemory(Handle* out, uptr address, u64 size,
                                       Permission permission) {
    printf("svcCreateTransferMemory called (address: 0x%08lx, size: %llu, "
           "perm: %u)\n",
           address, size, (u32)permission);

    // TODO: implement
    printf("Not implemented\n");

    return RESULT_SUCCESS;
}

Result Kernel::svcCloseHandle(Handle handle) {
    printf("svcCloseHandle called (handle: 0x%08x)\n", handle);

    // TODO: implement
    printf("Not implemented\n");

    return RESULT_SUCCESS;
}

Result Kernel::svcWaitSynchronization(u64& handle_index, Handle* handles_ptr,
                                      i32 handles_count, i64 timeout) {
    printf("svcWaitSynchronization called (handles: %p, count: %d, timeout: "
           "%lld)\n",
           handles_ptr, handles_count, timeout);

    // TODO: implement
    printf("Not implemented\n");

    // HACK
    handle_index = 0;

    return RESULT_SUCCESS;
}

Result Kernel::svcArbitrateLock(u32 wait_tag, uptr mutex_addr, u32 self_tag) {
    printf("svcArbitrateLock called (wait: 0x%08x, mutex: 0x%08lx, self: "
           "0x%08x)\n",
           wait_tag, mutex_addr, self_tag);

    // TODO: implement
    printf("Not implemented\n");

    return RESULT_SUCCESS;
}

Result Kernel::svcArbitrateUnlock(uptr mutex_addr) {
    printf("svcArbitrateUnlock called (mutex: 0x%08lx)\n", mutex_addr);

    // TODO: implement
    printf("Not implemented\n");

    return RESULT_SUCCESS;
}

Result Kernel::svcWaitProcessWideKeyAtomic(uptr mutex_addr, uptr var_addr,
                                           u32 self_tag, i64 timeout) {
    printf("svcWaitProcessWideKeyAtomic called (mutex: 0x%08lx, var: 0x%08lx, "
           "self: 0x%08x, timeout: %llu)\n",
           mutex_addr, var_addr, self_tag, timeout);

    // TODO: implement
    printf("Not implemented\n");

    return RESULT_SUCCESS;
}

Result Kernel::svcConnectToNamedPort(Handle* out, const std::string& name) {
    printf("svcConnectToNamedPort called (name: %s)\n", name.c_str());

    if (name == "sm:") {
        *out = AddService<Services::ServiceManager>();
    } else {
        printf("Unknown service name: %s\n", name.c_str());
        return MAKE_KERNEL_RESULT(NotFound);
    }

    return RESULT_SUCCESS;
}

Result Kernel::svcSendSyncRequest(Handle session_handle) {
    printf("svcSendSyncRequest called (session: 0x%08x)\n", session_handle);

    Services::ServiceBase* service = GetService(session_handle);
    u8* tls_ptr = tls_mem->GetPtrU8();

    // Request

    // HIPC header
    Hipc::ParsedRequest hipc_in = Hipc::parse_request(tls_ptr);
    u8* in_ptr = align_ptr((u8*)hipc_in.data.data_words, 0x10);

    // Dispatch
    Writer writer(service_scratch_buffer);
    Writer move_handles_writer(service_scratch_buffer_move_handles);
    switch (static_cast<Cmif::CommandType>(hipc_in.meta.type)) {
    case Cmif::CommandType::Request:
        printf("COMMAND: Request\n");
        service->Request(*this, writer, move_handles_writer, in_ptr);
        break;
    case Cmif::CommandType::Control:
        printf("COMMAND: Control\n");
        service->Control(*this, writer, in_ptr);
        break;
    default:
        printf("Unknown command %u\n", hipc_in.meta.type);
        break;
    }

    // Response

    // HIPC header
#define GET_ARRAY_SIZE(writer)                                                 \
    static_cast<u32>(align(writer.GetWrittenSize(), (usize)4) / sizeof(u32))

    auto response = Hipc::make_request(
        tls_ptr, {.num_data_words = GET_ARRAY_SIZE(writer),
                  .num_move_handles = GET_ARRAY_SIZE(move_handles_writer)});

#undef GET_ARRAY_SIZE

#define WRITE_ARRAY(writer, ptr, align)                                        \
    if (response.ptr) {                                                        \
        u8* aligned_ptr = (u8*)response.ptr;                                   \
        if (align)                                                             \
            aligned_ptr = align_ptr(aligned_ptr, 0x10);                        \
        memcpy(aligned_ptr, writer.GetBase(), writer.GetWrittenSize());        \
    }

    WRITE_ARRAY(writer, data_words, true);
    WRITE_ARRAY(move_handles_writer, move_handles, false);

#undef WRITE_ARRAY

    // AppletMessage_FocusStateChanged for _appletReceiveMessage
    // AppletMessage_InFocus for _appletGetCurrentFocusState
    // TODO: no longer needed?
    /*
    static int num_executed = 0;
    num_executed++;
    printf("NUM EXECUTED: %i\n", num_executed);
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
    printf("svcBreak called (reason: ");
    switch (reason.type) {
    case BreakReasonType::Panic:
        printf("panic");
        break;
    case BreakReasonType::Assert:
        printf("assert");
        break;
    case BreakReasonType::User:
        printf("user");
        break;
    case BreakReasonType::PreLoadDll:
        printf("pre load dll");
        break;
    case BreakReasonType::PostLoadDll:
        printf("post load dll");
        break;
    case BreakReasonType::PreUnloadDll:
        printf("pre unload dll");
        break;
    case BreakReasonType::PostUnloadDll:
        printf("post unload dll");
        break;
    case BreakReasonType::CppException:
        printf("cpp exception");
        break;
    }

    printf(", buffer ptr: 0x%08lx, buffer size: 0x%08zx)\n", buffer_ptr,
           buffer_size);

    if (buffer_ptr && buffer_size == 0x4) {
        printf("diagAbortWithResult (description: %u)\n",
               ((*(u32*)buffer_ptr) >> 9) & 0x1FFF);
    }

    if (!reason.notification_only)
        throw;

    return RESULT_SUCCESS;
}

Result Kernel::svcOutputDebugString(const char* str, usize len) {
    printf("svcOutputDebugString called\n");
    if (len != 0) {
        printf("%.*s", (int)len, str);
    }

    return RESULT_SUCCESS;
}

Result Kernel::svcGetInfo(u64* out, Info info) {
    printf("svcGetInfo called\n");

    if (info.is_system_info) {
        switch (info.system_info_type) {
        default:
            printf("Unimplemented system info type %u\n",
                   (u32)info.system_info_type);
            return MAKE_KERNEL_RESULT(NotImplemented);
        }
    } else {
        switch (info.info_type) {
        case InfoType::AliasRegionAddress:
            printf("Not implemented\n");
            // HACK
            *out = 0;
            return RESULT_SUCCESS;
        case InfoType::AliasRegionSize:
            printf("Not implemented\n");
            // HACK
            *out = 0;
            return RESULT_SUCCESS;
        case InfoType::HeapRegionAddress:
            *out = heap_mem->GetBase();
            return RESULT_SUCCESS;
        case InfoType::HeapRegionSize:
            *out = heap_mem->GetSize();
            return RESULT_SUCCESS;
        case InfoType::AslrRegionAddress:
            printf("Not implemented\n");
            // HACK
            *out = 0;
            return RESULT_SUCCESS;
        case InfoType::AslrRegionSize:
            printf("Not implemented\n");
            // HACK
            *out = 0;
            return RESULT_SUCCESS;
        case InfoType::StackRegionAddress:
            *out = stack_mem->GetBase();
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
                   tls_mem->GetSize() + rom_mem->GetSize() +
                   heap_mem->GetSize();
            return RESULT_SUCCESS;
        case InfoType::AliasRegionExtraSize:
            printf("Not implemented\n");
            // HACK
            *out = 0;
            return RESULT_SUCCESS;
        default:
            printf("Unimplemented info type %u\n", (u32)info.info_type);
            return MAKE_KERNEL_RESULT(NotImplemented);
        }
    }
}

} // namespace Hydra::Horizon
