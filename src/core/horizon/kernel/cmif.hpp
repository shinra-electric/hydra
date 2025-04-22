#pragma once

#include "common/common.hpp"
#include "common/macros.hpp"
#include "core/horizon/kernel/hipc.hpp"

namespace Hydra::Horizon::Kernel::Cmif {

#define CMIF_IN_HEADER_MAGIC 0x49434653  // "SFCI"
#define CMIF_OUT_HEADER_MAGIC 0x4F434653 // "SFCO"

enum class DomainCommandType : u8 {
    Invalid,
    SendMessage,
    Close,
};

enum class CommandType {
    Invalid,
    LegacyRequest,
    Close,
    LegacyControl,
    Request,
    Control,
    RequestWithContext,
    ControlWithContext,
};

enum class ControlCommandType {
    ConvertCurrentObjectToDomain,
    CopyFromCurrentDomain,
    CloneCurrentObject,
    QueryPointerBufferSize,
    CloneCurrentObjectEx,
};

struct DomainInHeader {
    DomainCommandType type;
    u8 num_in_objects;
    u16 data_size;
    u32 object_id;
    u32 padding;
    u32 token;
};

struct InHeader {
    u32 magic;
    u32 version;
    u32 command_id;
    u32 token;
};

// From https://github.com/switchbrew/libnx
struct DomainOutHeader {
    u32 num_out_objects;
    u32 padding[3];
};

// From https://github.com/switchbrew/libnx
struct OutHeader {
    u32 magic;
    u32 version;
    Result result;
    u32 token;
};

inline Result* write_out_header(Writer& writer) {
    auto hdr = writer.Write(OutHeader{
        .magic = CMIF_OUT_HEADER_MAGIC,
        .version = 0,
        .result = MAKE_RESULT(
            SfHipc, Error::NotImplemented), // TODO: is module correct?
        .token = 0,
    });

    return &hdr->result;
}

inline void write_domain_out_header(Writer& writer) {
    writer.Write(DomainOutHeader{
        .num_out_objects = 0,
    });
}

} // namespace Hydra::Horizon::Kernel::Cmif

ENABLE_ENUM_FORMATTING(Hydra::Horizon::Kernel::Cmif::DomainCommandType, Invalid,
                       "invalid", SendMessage, "send message", Close, "close")

ENABLE_ENUM_FORMATTING(Hydra::Horizon::Kernel::Cmif::CommandType, Invalid,
                       "invalid", LegacyRequest, "legacy request", Close,
                       "close", LegacyControl, "legacy control", Request,
                       "request", Control, "control", RequestWithContext,
                       "request with context", ControlWithContext,
                       "control with context")

ENABLE_ENUM_FORMATTING(Hydra::Horizon::Kernel::Cmif::ControlCommandType,
                       ConvertCurrentObjectToDomain,
                       "convert current object to domain",
                       CopyFromCurrentDomain, "copy from current domain",
                       CloneCurrentObject, "clone current object",
                       QueryPointerBufferSize, "query pointer buffer size",
                       CloneCurrentObjectEx, "clone current object ex")
