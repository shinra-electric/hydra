#pragma once

#include "horizon/const.hpp"

namespace Hydra::Horizon::Services::NvDrv {

enum class NvResult : u32 {
    Success,
    NotImplemented,
    NotSupported,
    NotInitialized,
    BadParameter,
    Timeout,
    InsufficientMemory,
    ReadOnlyAttribute,
    InvalidState,
    InvalidAddress,
    InvalidSize,
    BadValue,
    AlreadyAllocated,
    Busy,
    ResourceError,
    CountMismatch,
    SharedMemoryTooSmall = 0x1000,
    FileOperationFailed = 0x30003,
    IoctlFailed = 0x3000F,
};

}
