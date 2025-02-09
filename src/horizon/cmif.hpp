#pragma once

#include "common.hpp"
#include "horizon/const.hpp"
#include "horizon/hipc.hpp"

namespace Hydra::Horizon {

#define CMIF_IN_HEADER_MAGIC 0x49434653  // "SFCI"
#define CMIF_OUT_HEADER_MAGIC 0x4F434653 // "SFCO"

enum class CmifCommandType {
    Invalid = 0,
    LegacyRequest = 1,
    Close = 2,
    LegacyControl = 3,
    Request = 4,
    Control = 5,
    RequestWithContext = 6,
    ControlWithContext = 7,
};

struct CmifDomainInHeader {
    u8 type;
    u8 num_in_objects;
    u16 data_size;
    u32 object_id;
    u32 padding;
    u32 token;
};

typedef struct CmifInHeader {
    u32 magic;
    u32 version;
    u32 command_id;
    u32 token;
} CmifInHeader;

// From https://github.com/switchbrew/libnx
struct CmifDomainOutHeader {
    u32 num_out_objects;
    u32 padding[3];
};

// From https://github.com/switchbrew/libnx
struct CmifOutHeader {
    u32 magic;
    u32 version;
    Result result;
    u32 token;
};

inline CmifInHeader cmif_read_in_header(u8*& in_ptr) {
    auto hdr = reinterpret_cast<CmifInHeader*>(in_ptr);
    in_ptr += sizeof(CmifInHeader);

    return *hdr;
}

inline Result* cmif_write_out_header(u8*& out_ptr, usize& out_size) {
    auto hdr = reinterpret_cast<CmifOutHeader*>(out_ptr);
    *hdr = {
        .magic = CMIF_OUT_HEADER_MAGIC,
        .version = 0,
        .result = MAKE_KERNEL_RESULT(NotImplemented),
        .token = 0,
    };

    out_ptr += sizeof(CmifOutHeader);
    out_size += sizeof(CmifOutHeader);

    return &hdr->result;
}

} // namespace Hydra::Horizon
