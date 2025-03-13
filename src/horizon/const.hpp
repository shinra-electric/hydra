#pragma once

#include "common/common.hpp"
#include "common/macros.hpp"

namespace Hydra::Horizon {

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

enum class ConfigEntryType : u32 {
    EndOfList = 0,        ///< Entry list terminator.
    MainThreadHandle = 1, ///< Provides the handle to the main thread.
    NextLoadPath = 2,    ///< Provides a buffer containing information about the
                         ///< next homebrew application to load.
    OverrideHeap = 3,    ///< Provides heap override information.
    OverrideService = 4, ///< Provides service override information.
    Argv = 5,            ///< Provides argv.
    SyscallAvailableHint =
        6,          ///< Provides syscall availability hints (SVCs 0x00..0x7F).
    AppletType = 7, ///< Provides APT applet type.
    AppletWorkaround =
        8, ///< Indicates that APT is broken and should not be used.
    Reserved9 =
        9, ///< Unused/reserved entry type, formerly used by StdioSockets.
    ProcessHandle = 10,  ///< Provides the process handle.
    LastLoadResult = 11, ///< Provides the last load result.
    RandomSeed = 14, ///< Provides random data used to seed the pseudo-random
                     ///< number generator.
    UserIdStorage =
        15, ///< Provides persistent storage for the preselected user id.
    HosVersion = 16, ///< Provides the currently running Horizon OS version.
    SyscallAvailableHint2 =
        17, ///< Provides syscall availability hints (SVCs 0x80..0xBF).
};

enum class ConfigEntryFlag : u32 {
    None = 0,
    IsMandatory = BIT(0), ///< Specifies that the entry **must** be processed by
                          ///< the homebrew application.
};

struct ConfigEntry {
    ConfigEntryType type;  ///< Type of entry
    ConfigEntryFlag flags; ///< Entry flags
    u64 values[2];         ///< Entry arguments (type-specific)
};

// TODO: idle tick count -1, {current coreid} (probably the same logic as thread
// tick count)
// TODO: random entropy 0 - 3
constexpr u32 INFO_SUB_TYPE_INITIAL_PROCESS_ID_RANGE_LOWER_BOUND = 0;
constexpr u32 INFO_SUB_TYPE_INITIAL_PROCESS_ID_RANGE_UPPER_BOUND = 1;
constexpr u32 INFO_SUB_TYPE_THREAD_TICK_COUNT_CORE0 = 0;
constexpr u32 INFO_SUB_TYPE_THREAD_TICK_COUNT_CORE1 = 1;
constexpr u32 INFO_SUB_TYPE_THREAD_TICK_COUNT_CORE2 = 2;
constexpr u32 INFO_SUB_TYPE_THREAD_TICK_COUNT_CORE3 = 3;
constexpr u32 INFO_SUB_TYPE_THREAD_TICK_COUNT_ALL = 0xFFFFFFFF;

} // namespace Hydra::Horizon

ENABLE_ENUM_FLAGS_FORMATTING(Hydra::Horizon::Permission, Read, "read", Write,
                             "write", Execute, "execute", DontCare,
                             "don't care")

ENABLE_ENUM_FORMATTING(Hydra::Horizon::BreakReasonType, Panic, "panic", Assert,
                       "assert", User, "user", PreLoadDll, "pre load dll",
                       PostLoadDll, "post load dll", PreUnloadDll,
                       "pre unload dll", PostUnloadDll, "post unload dll",
                       CppException, "cpp exception")

ENABLE_ENUM_FORMATTING(
    Hydra::Horizon::InfoType, CoreMask, "core mask", PriorityMask,
    "priority mask", AliasRegionAddress, "alias region address",
    AliasRegionSize, "alias region size", HeapRegionAddress,
    "heap region address", HeapRegionSize, "heap region size", TotalMemorySize,
    "total memory size", UsedMemorySize, "used memory size", DebuggerAttached,
    "debugger attached", ResourceLimit, "resource limit", IdleTickCount,
    "idle tick count", RandomEntropy, "random entropy", AslrRegionAddress,
    "aslr region address", AslrRegionSize, "aslr region size",
    StackRegionAddress, "stack region address", StackRegionSize,
    "stack region size", SystemResourceSizeTotal, "system resource size total",
    SystemResourceSizeUsed, "system resource size used", ProgramId,
    "program id", InitialProcessIdRange, "initial process id range",
    UserExceptionContextAddress, "user exception context address",
    TotalNonSystemMemorySize, "total non-system memory size",
    UsedNonSystemMemorySize, "used non-system memory size", IsApplication,
    "is application", FreeThreadCount, "free thread count", ThreadTickCount,
    "thread tick count", IsSvcPermitted, "is svc permitted", IoRegionHint,
    "io region hint", AliasRegionExtraSize, "alias region extra size",
    TransferMemoryHint, "transfer memory hint", ThreadTickCountDeprecated,
    "thread tick count deprecated")

ENABLE_ENUM_FORMATTING(Hydra::Horizon::SystemInfoType, TotalPhysicalMemorySize,
                       "total physical memory size", UsedPhysicalMemorySize,
                       "used physical memory size", InitialProcessIdRange,
                       "initial process id range")
