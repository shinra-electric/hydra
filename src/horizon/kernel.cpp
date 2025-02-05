#include "horizon/kernel.hpp"

#include "hw/cpu/cpu.hpp"
#include "hw/mmu/memory.hpp"
#include "hw/mmu/mmu.hpp"

namespace Hydra::Horizon {

// TODO: don't hardcode these
#define ROM_MEM_SIZE 0x01000000
#define ROM_MEM_BASE 0x80000000

// HACK: no idea what I am doing, but _impure_ptr seems to point there
#define BSS_MEM_BASE 0x00010000

#define STACK_SIZE 0x200000
// For EL0 and EL1
#define STACK_MEM_SIZE (STACK_SIZE * 2)
#define STACK_MEM_BASE 0x01000000

#define KERNEL_MEM_SIZE 0x10000
#define KERNEL_MEM_BASE 0xF0000000

#define TLS_MEM_SIZE 0x20000
#define TLS_MEM_BASE 0x02000000

const u32 exception_handler[] = {
    0xD41FFFE2u, // hvc #0xFFFF
    0xD69F03E0u, // eret
    // 0xD2B00000, // mov x0, #0x80000000
    // 0xD61F0000, // br  x0
    // Shouldn't happen
    0xD4200000u, // brk #0
};

const u32 exception_trampoline[] = {
    0xD508831Fu, // msr spsel, xzr

    // 0x910003e0,  // mov x0, sp
    // 0xd5384241,  // TODO
    // 0xd5384202,  // mrs x2, spsel
    // 0xD4200000u, // brk #0

    0xD69F03E0u, // eret
    // Shouldn't happen
    0xD4200000u, // brk #0
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

    // HACK: cmif expects this header magic?
    //*((u32*)tls_mem->GetPtr() + 0x10) = 0x4F434653; // "SFCO"
}

Kernel::~Kernel() {
    delete rom_mem;
    delete bss_mem;
    delete stack_mem;
    delete kernel_mem;
    delete tls_mem;
}

void Kernel::LoadROM(Rom* rom) {
    rom_mem = new HW::MMU::Memory(
        ROM_MEM_BASE, ROM_MEM_SIZE,
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
    bss_mem = new HW::MMU::Memory(BSS_MEM_BASE, rom->GetBssSize(),
                                  Permission::ReadWrite);
    bss_mem->Clear();
}

bool Kernel::SupervisorCall(HW::CPU::CPUBase* cpu, u64 id) {
    Result res;
    u32 tmpU32;
    u64 tmpU64;
    uptr tmpUPTR;
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
    case 0x13:
        res = svcMapSharedMemory(cpu->GetRegX(0), cpu->GetRegX(1),
                                 cpu->GetRegX(2),
                                 static_cast<Permission>(cpu->GetRegX(3)));
        cpu->SetRegX(0, res);
        break;
    case 0x16:
        res = svcCloseHandle(cpu->GetRegX(0));
        cpu->SetRegX(0, res);
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
            reinterpret_cast<Handle*>(&tmpUPTR),
            reinterpret_cast<const char*>(mmu->UnmapPtr(cpu->GetRegX(1))));
        cpu->SetRegX(0, res);
        cpu->SetRegX(1, tmpUPTR);
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

    // TODO: implement
    printf("Not implemented\n");

    // HACK
    *out = 0x1FFFFFFF;

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

Result Kernel::svcMapSharedMemory(Handle handle, uptr addr, usize size,
                                  Permission permission) {
    printf("svcMapSharedMemory called (handle: 0x%08x, addr: 0x%08lx, size: "
           "0x%08zx, perm: %u)\n",
           handle, addr, size, (u32)permission);

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

Result Kernel::svcConnectToNamedPort(Handle* out, const char* name) {
    printf("svcConnectToNamedPort called (name: %s)\n", name);

    // TODO: implement
    printf("Not implemented\n");

    // HACK
    *out = 0x1FFFFFFF;

    return RESULT_SUCCESS;
}

Result Kernel::svcSendSyncRequest(Handle session_handle) {
    printf("svcSendSyncRequest called (session: 0x%08x)\n", session_handle);

    uptr out_addr = tls_mem->GetPtr();

    // HIPC header
    HipcHeader hipc_header = {
        // TODO: type
        .num_send_statics = 0, // HACK
        .num_send_buffers = 0, // HACK
        .num_recv_buffers = 0, // HACK
        .num_exch_buffers = 0, // HACK
        .num_data_words = 0,   // HACK
        // TODO: recv_static_mode
        // TODO: recv_list_offset
        .has_special_header = false, // HACK
    };

    *((HipcHeader*)out_addr) = hipc_header;
    out_addr += align(sizeof(HipcHeader), (usize)0x10);

    // CMIF header
    // TODO: what is CMIF domain header?
    CmifOutHeader cmif_header = {
        .magic = CMIF_OUT_HEADER_MAGIC,
        .version = 0, // HACK
        .result = RESULT_SUCCESS,
        .token = 0, // HACK
    };

    *((CmifOutHeader*)out_addr) = cmif_header;
    out_addr += align(sizeof(CmifOutHeader), (usize)0x10);

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
            printf("Not implemented\n");
            // HACK
            *out = 0;
            return RESULT_SUCCESS;
        case InfoType::HeapRegionSize:
            printf("Not implemented\n");
            // HACK
            *out = 0;
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
            printf("Not implemented\n");
            // HACK
            *out = 0;
            return RESULT_SUCCESS;
        case InfoType::UsedMemorySize:
            printf("Not implemented\n");
            // HACK
            *out = 0;
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
