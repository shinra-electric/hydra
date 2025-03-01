#pragma once

#include "horizon/const.hpp"

namespace Hydra::Horizon::Hipc {

#define HIPC_AUTO_RECV_STATIC UINT8_MAX
#define HIPC_RESPONSE_NO_PID UINT32_MAX

// From https://github.com/switchbrew/libnx
struct Metadata {
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

struct BufferDescriptor {
    u32 size_low;
    u32 address_low;
    u32 mode : 2;
    u32 address_high : 22;
    u32 size_high : 4;
    u32 address_mid : 4;
};

struct RecvListEntry {
    u32 address_low;
    u32 address_high : 16;
    u32 size : 16;
};

// From https://github.com/switchbrew/libnx
struct Header {
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
struct StaticDescriptor {
    u32 index : 6;
    u32 address_high : 6;
    u32 address_mid : 4;
    u32 size : 16;
    u32 address_low;
};

// From https://github.com/switchbrew/libnx
struct Response {
    u64 pid;
    u32 num_statics;
    u32 num_data_words;
    u32 num_copy_handles;
    u32 num_move_handles;
    StaticDescriptor* statics;
    u32* data_words;
    HandleId* copy_handles;
    HandleId* move_handles;
};

// From https://github.com/switchbrew/libnx
struct Request {
    StaticDescriptor* send_statics;
    BufferDescriptor* send_buffers;
    BufferDescriptor* recv_buffers;
    BufferDescriptor* exch_buffers;
    u32* data_words;
    RecvListEntry* recv_list;
    HandleId* copy_handles;
    HandleId* move_handles;
};

// From https://github.com/switchbrew/libnx
struct ParsedRequest {
    Metadata meta;
    Request data;
    u64 pid;
};

// From https://github.com/switchbrew/libnx
struct SpecialHeader {
    u32 send_pid : 1;
    u32 num_copy_handles : 4;
    u32 num_move_handles : 4;
    u32 padding : 23;
};

// From https://github.com/switchbrew/libnx
inline Request calc_request_layout(Metadata meta, void* base) {
    // Copy handles
    HandleId* copy_handles = NULL;
    if (meta.num_copy_handles) {
        copy_handles = (HandleId*)base;
        base = copy_handles + meta.num_copy_handles;
    }

    // Move handles
    HandleId* move_handles = NULL;
    if (meta.num_move_handles) {
        move_handles = (HandleId*)base;
        base = move_handles + meta.num_move_handles;
    }

    // Send statics
    StaticDescriptor* send_statics = NULL;
    if (meta.num_send_statics) {
        send_statics = (StaticDescriptor*)base;
        base = send_statics + meta.num_send_statics;
    }

    // Send buffers
    BufferDescriptor* send_buffers = NULL;
    if (meta.num_send_buffers) {
        send_buffers = (BufferDescriptor*)base;
        base = send_buffers + meta.num_send_buffers;
    }

    // Recv buffers
    BufferDescriptor* recv_buffers = NULL;
    if (meta.num_recv_buffers) {
        recv_buffers = (BufferDescriptor*)base;
        base = recv_buffers + meta.num_recv_buffers;
    }

    // Exch buffers
    BufferDescriptor* exch_buffers = NULL;
    if (meta.num_exch_buffers) {
        exch_buffers = (BufferDescriptor*)base;
        base = exch_buffers + meta.num_exch_buffers;
    }

    // Data words
    u32* data_words = NULL;
    if (meta.num_data_words) {
        data_words = (u32*)base;
        base = data_words + meta.num_data_words;
    }

    // Recv list
    RecvListEntry* recv_list = NULL;
    if (meta.num_recv_statics)
        recv_list = (RecvListEntry*)base;

    return (Request){
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

inline ParsedRequest parse_request(void* base) {
    // Parse message header
    Header hdr = {};
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
    SpecialHeader sphdr = {};
    if (hdr.has_special_header) {
        memcpy(&sphdr, base, sizeof(sphdr));
        base = (u8*)base + sizeof(sphdr);

        // Read PID descriptor
        if (sphdr.send_pid) {
            pid = *(u64*)base;
            base = (u8*)base + sizeof(u64);
        }
    }

    const Metadata meta = {
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

    return {
        .meta = meta,
        .data = calc_request_layout(meta, base),
        .pid = pid,
    };
}

inline Request make_request(void* base, Metadata meta) {
    // Write message header
    bool has_special_header =
        meta.send_pid || meta.num_copy_handles || meta.num_move_handles;
    Header* hdr = (Header*)base;
    base = hdr + 1;
    *hdr = (Header){
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
        SpecialHeader* sphdr = (SpecialHeader*)base;
        base = sphdr + 1;
        *sphdr = (SpecialHeader){
            .send_pid = meta.send_pid,
            .num_copy_handles = meta.num_copy_handles,
            .num_move_handles = meta.num_move_handles,
        };
        if (meta.send_pid)
            base = (u8*)base + sizeof(u64);
    }

    // Calculate layout
    return calc_request_layout(meta, base);
}

} // namespace Hydra::Horizon::Hipc
