#pragma once

#include "common.hpp"

namespace Hydra::Horizon {

typedef u32 Handle;
#define INVALID_HANDLE 0

// From https://github.com/switchbrew/libnx
enum class Error {
    OutOfSessions = 7,
    InvalidCapabilityDescriptor = 14,
    NotImplemented = 33,
    ThreadTerminating = 59,
    OutOfDebugEvents = 70,
    InvalidSize = 101,
    InvalidAddress = 102,
    ResourceExhausted = 103,
    OutOfMemory = 104,
    OutOfHandles = 105,
    InvalidMemoryState = 106,
    InvalidMemoryPermissions = 108,
    InvalidMemoryRange = 110,
    InvalidPriority = 112,
    InvalidCoreId = 113,
    InvalidHandle = 114,
    InvalidUserBuffer = 115,
    InvalidCombination = 116,
    TimedOut = 117,
    Cancelled = 118,
    OutOfRange = 119,
    InvalidEnumValue = 120,
    NotFound = 121,
    AlreadyExists = 122,
    ConnectionClosed = 123,
    UnhandledUserInterrupt = 124,
    InvalidState = 125,
    ReservedValue = 126,
    InvalidHwBreakpoint = 127,
    FatalUserException = 128,
    OwnedByAnotherProcess = 129,
    ConnectionRefused = 131,
    OutOfResource = 132,
    IpcMapFailed = 259,
    IpcCmdbufTooSmall = 260,
    NotDebugged = 520,
};

typedef u32 Result;

#define MODULE_KERNEL 1

#define MAKE_RESULT(module, description)                                       \
    ((((module) & 0x1FF)) | (static_cast<u32>(description) & 0x1FFF) << 9)

#define RESULT_SUCCESS 0

#define MAKE_KERNEL_RESULT(description)                                        \
    MAKE_RESULT(MODULE_KERNEL, Error::description)

enum class Permission : u32 {
    None = 0x0,
    Read = BIT(0),
    Write = BIT(1),
    Execute = BIT(2),
    ReadWrite = Read | Write,
    ReadExecute = Read | Execute,
    DontCare = BIT(28),
};
ENABLE_ENUM_BITMASK_OPERATORS(Permission)

struct MemoryInfo {
    u64 addr;
    u64 size;
    u32 type; // TODO: memory state
    u32 attr; // TODO: memory attributes
    Permission perm;
    u32 ipc_ref_count;    // TODO: what
    u32 device_ref_count; // TODO: what
    u32 padding = 0;
};

enum class BreakReasonType {
    Panic,
    Assert,
    User,
    PreLoadDll,
    PostLoadDll,
    PreUnloadDll,
    PostUnloadDll,
    CppException,
};

struct BreakReason {
    BreakReasonType type;
    bool notification_only;

    BreakReason(u64 reg) {
        notification_only = reg & 0x80000000;
        type = static_cast<BreakReasonType>(reg & 0x7FFFFFFF);
    }
};

// From https://github.com/switchbrew/libnx
enum class InfoType : u32 {
    CoreMask = 0,            ///< Bitmask of allowed Core IDs.
    PriorityMask = 1,        ///< Bitmask of allowed Thread Priorities.
    AliasRegionAddress = 2,  ///< Base of the Alias memory region.
    AliasRegionSize = 3,     ///< Size of the Alias memory region.
    HeapRegionAddress = 4,   ///< Base of the Heap memory region.
    HeapRegionSize = 5,      ///< Size of the Heap memory region.
    TotalMemorySize = 6,     ///< Total amount of memory available for process.
    UsedMemorySize = 7,      ///< Amount of memory currently used by process.
    DebuggerAttached = 8,    ///< Whether current process is being debugged.
    ResourceLimit = 9,       ///< Current process's resource limit handle.
    IdleTickCount = 10,      ///< Number of idle ticks on CPU.
    RandomEntropy = 11,      ///< [2.0.0+] Random entropy for current process.
    AslrRegionAddress = 12,  ///< [2.0.0+] Base of the process's address space.
    AslrRegionSize = 13,     ///< [2.0.0+] Size of the process's address space.
    StackRegionAddress = 14, ///< [2.0.0+] Base of the Stack memory region.
    StackRegionSize = 15,    ///< [2.0.0+] Size of the Stack memory region.
    SystemResourceSizeTotal =
        16, ///< [3.0.0+] Total memory allocated for process memory management.
    SystemResourceSizeUsed = 17, ///< [3.0.0+] Amount of memory currently used
                                 ///< by process memory management.
    ProgramId = 18,              ///< [3.0.0+] Program ID for the process.
    InitialProcessIdRange = 19,  ///< [4.0.0-4.1.0] Min/max initial process IDs.
    UserExceptionContextAddress = 20, ///< [5.0.0+] Address of the process's
                                      ///< exception context (for break).
    TotalNonSystemMemorySize =
        21, ///< [6.0.0+] Total amount of memory available for process,
            ///< excluding that for process memory management.
    UsedNonSystemMemorySize =
        22, ///< [6.0.0+] Amount of memory used by process, excluding that for
            ///< process memory management.
    IsApplication =
        23, ///< [9.0.0+] Whether the specified process is an Application.
    FreeThreadCount = 24, ///< [11.0.0+] The number of free threads available to
                          ///< the process's resource limit.
    ThreadTickCount = 25, ///< [13.0.0+] Number of ticks spent on thread.
    IsSvcPermitted =
        26, ///< [14.0.0+] Does process have access to SVC (only usable with
            ///< \ref svcSynchronizePreemptionState at present).
    IoRegionHint =
        27, ///< [16.0.0+] Low bits of the physical address for a KIoRegion.
    AliasRegionExtraSize =
        28, ///< [18.0.0+] Extra size added to the reserved region.

    TransferMemoryHint = 34, ///< [19.0.0+] Low bits of the process
                             ///< address for a KTransferMemory.

    ThreadTickCountDeprecated =
        0xF0000002, ///< [1.0.0-12.1.0] Number of ticks spent on thread.
};

// From https://github.com/switchbrew/libnx
enum class SystemInfoType {
    TotalPhysicalMemorySize = 0, ///< Total amount of DRAM available to system.
    UsedPhysicalMemorySize = 1,  ///< Current amount of DRAM used by system.
    InitialProcessIdRange = 2,   ///< Min/max initial process IDs.
};

// TODO: idle tick count -1, {current coreid} (probably the same logic as thread
// tick count)
// TODO: random entropy 0 - 3
#define INFO_SUB_TYPE_INITIAL_PROCESS_ID_RANGE_LOWER_BOUND 0
#define INFO_SUB_TYPE_INITIAL_PROCESS_ID_RANGE_UPPER_BOUND 1
#define INFO_SUB_TYPE_THREAD_TICK_COUNT_CORE0 0
#define INFO_SUB_TYPE_THREAD_TICK_COUNT_CORE1 1
#define INFO_SUB_TYPE_THREAD_TICK_COUNT_CORE2 2
#define INFO_SUB_TYPE_THREAD_TICK_COUNT_CORE3 3
#define INFO_SUB_TYPE_THREAD_TICK_COUNT_ALL 0xFFFFFFFF

struct Info {
    union {
        u32 value;
        InfoType info_type;
        SystemInfoType system_info_type;
    };

    Handle handle;
    bool is_system_info;

    u64 info_sub_type;

    Info(u32 id0, Handle handle_, u64 id1)
        : value{id0}, handle{handle_}, is_system_info{handle == INVALID_HANDLE},
          info_sub_type{id1} {}
};

} // namespace Hydra::Horizon
