#pragma once

#include "horizon/const.hpp"

namespace Hydra::Horizon {

#define HIPC_AUTO_RECV_STATIC UINT8_MAX
#define HIPC_RESPONSE_NO_PID UINT32_MAX

// From https://github.com/switchbrew/libnx
struct HipcMetadata {
    u32 type;
    u32 num_send_statics;
    u32 num_send_buffers;
    u32 num_recv_buffers;
    u32 num_exch_buffers;
    u32 num_data_words;
    u32 num_recv_statics; // also accepts HIPC_AUTO_RECV_STATIC
    u32 send_pid;
    u32 num_copy_handles;
    u32 num_move_handles;
};

struct HipcBufferDescriptor {
    u32 size_low;
    u32 address_low;
    u32 mode : 2;
    u32 address_high : 22;
    u32 size_high : 4;
    u32 address_mid : 4;
};

struct HipcRecvListEntry {
    u32 address_low;
    u32 address_high : 16;
    u32 size : 16;
};

// From https://github.com/switchbrew/libnx
struct HipcHeader {
    u32 type : 16;
    u32 num_send_statics : 4;
    u32 num_send_buffers : 4;
    u32 num_recv_buffers : 4;
    u32 num_exch_buffers : 4;
    u32 num_data_words : 10;
    u32 recv_static_mode : 4;
    u32 padding : 6;
    u32 recv_list_offset : 11; // Unused.
    bool has_special_header : 1;
};

// From https://github.com/switchbrew/libnx
struct HipcStaticDescriptor {
    u32 index : 6;
    u32 address_high : 6;
    u32 address_mid : 4;
    u32 size : 16;
    u32 address_low;
};

// From https://github.com/switchbrew/libnx
struct HipcResponse {
    u64 pid;
    u32 num_statics;
    u32 num_data_words;
    u32 num_copy_handles;
    u32 num_move_handles;
    HipcStaticDescriptor* statics;
    u32* data_words;
    Handle* copy_handles;
    Handle* move_handles;
};

// From https://github.com/switchbrew/libnx
struct HipcRequest {
    HipcStaticDescriptor* send_statics;
    HipcBufferDescriptor* send_buffers;
    HipcBufferDescriptor* recv_buffers;
    HipcBufferDescriptor* exch_buffers;
    u32* data_words;
    HipcRecvListEntry* recv_list;
    Handle* copy_handles;
    Handle* move_handles;
};

// From https://github.com/switchbrew/libnx
typedef struct HipcParsedRequest {
    HipcMetadata meta;
    HipcRequest data;
    u64 pid;
} HipcParsedRequest;

// From https://github.com/switchbrew/libnx
struct HipcSpecialHeader {
    u32 send_pid : 1;
    u32 num_copy_handles : 4;
    u32 num_move_handles : 4;
    u32 padding : 23;
};

// From https://github.com/switchbrew/libnx
inline HipcRequest hipcCalcRequestLayout(HipcMetadata meta, void* base) {
    // Copy handles
    Handle* copy_handles = NULL;
    if (meta.num_copy_handles) {
        copy_handles = (Handle*)base;
        base = copy_handles + meta.num_copy_handles;
    }

    // Move handles
    Handle* move_handles = NULL;
    if (meta.num_move_handles) {
        move_handles = (Handle*)base;
        base = move_handles + meta.num_move_handles;
    }

    // Send statics
    HipcStaticDescriptor* send_statics = NULL;
    if (meta.num_send_statics) {
        send_statics = (HipcStaticDescriptor*)base;
        base = send_statics + meta.num_send_statics;
    }

    // Send buffers
    HipcBufferDescriptor* send_buffers = NULL;
    if (meta.num_send_buffers) {
        send_buffers = (HipcBufferDescriptor*)base;
        base = send_buffers + meta.num_send_buffers;
    }

    // Recv buffers
    HipcBufferDescriptor* recv_buffers = NULL;
    if (meta.num_recv_buffers) {
        recv_buffers = (HipcBufferDescriptor*)base;
        base = recv_buffers + meta.num_recv_buffers;
    }

    // Exch buffers
    HipcBufferDescriptor* exch_buffers = NULL;
    if (meta.num_exch_buffers) {
        exch_buffers = (HipcBufferDescriptor*)base;
        base = exch_buffers + meta.num_exch_buffers;
    }

    // Data words
    u32* data_words = NULL;
    if (meta.num_data_words) {
        data_words = (u32*)base;
        base = data_words + meta.num_data_words;
    }

    // Recv list
    HipcRecvListEntry* recv_list = NULL;
    if (meta.num_recv_statics)
        recv_list = (HipcRecvListEntry*)base;

    return (HipcRequest){
        .send_statics = send_statics,
        .send_buffers = send_buffers,
        .recv_buffers = recv_buffers,
        .exch_buffers = exch_buffers,
        .data_words = data_words,
        .recv_list = recv_list,
        .copy_handles = copy_handles,
        .move_handles = move_handles,
    };
}

inline HipcParsedRequest hipcParseRequest(void* base) {
    // Parse message header
    HipcHeader hdr = {};
    memcpy(&hdr, base, sizeof(hdr));
    base = (u8*)base + sizeof(hdr);
    u32 num_recv_statics = 0;
    u64 pid = 0;

    // Parse recv static mode
    if (hdr.recv_static_mode) {
        if (hdr.recv_static_mode == 2u)
            num_recv_statics = HIPC_AUTO_RECV_STATIC;
        else if (hdr.recv_static_mode > 2u)
            num_recv_statics = hdr.recv_static_mode - 2u;
    }

    // Parse special header
    HipcSpecialHeader sphdr = {};
    if (hdr.has_special_header) {
        memcpy(&sphdr, base, sizeof(sphdr));
        base = (u8*)base + sizeof(sphdr);

        // Read PID descriptor
        if (sphdr.send_pid) {
            pid = *(u64*)base;
            base = (u8*)base + sizeof(u64);
        }
    }

    const HipcMetadata meta = {
        .type = hdr.type,
        .num_send_statics = hdr.num_send_statics,
        .num_send_buffers = hdr.num_send_buffers,
        .num_recv_buffers = hdr.num_recv_buffers,
        .num_exch_buffers = hdr.num_exch_buffers,
        .num_data_words = hdr.num_data_words,
        .num_recv_statics = num_recv_statics,
        .send_pid = sphdr.send_pid,
        .num_copy_handles = sphdr.num_copy_handles,
        .num_move_handles = sphdr.num_move_handles,
    };

    return (HipcParsedRequest){
        .meta = meta,
        .data = hipcCalcRequestLayout(meta, base),
        .pid = pid,
    };
}

inline HipcRequest hipcMakeRequest(void* base, HipcMetadata meta) {
    // Write message header
    bool has_special_header =
        meta.send_pid || meta.num_copy_handles || meta.num_move_handles;
    HipcHeader* hdr = (HipcHeader*)base;
    base = hdr + 1;
    *hdr = (HipcHeader){
        .type = meta.type,
        .num_send_statics = meta.num_send_statics,
        .num_send_buffers = meta.num_send_buffers,
        .num_recv_buffers = meta.num_recv_buffers,
        .num_exch_buffers = meta.num_exch_buffers,
        .num_data_words = meta.num_data_words,
        .recv_static_mode =
            meta.num_recv_statics
                ? (meta.num_recv_statics != HIPC_AUTO_RECV_STATIC
                       ? 2u + meta.num_recv_statics
                       : 2u)
                : 0u,
        .padding = 0,
        .recv_list_offset = 0,
        .has_special_header = has_special_header,
    };

    // Write special header
    if (has_special_header) {
        HipcSpecialHeader* sphdr = (HipcSpecialHeader*)base;
        base = sphdr + 1;
        *sphdr = (HipcSpecialHeader){
            .send_pid = meta.send_pid,
            .num_copy_handles = meta.num_copy_handles,
            .num_move_handles = meta.num_move_handles,
        };
        if (meta.send_pid)
            base = (u8*)base + sizeof(u64);
    }

    // Calculate layout
    return hipcCalcRequestLayout(meta, base);
}

} // namespace Hydra::Horizon
