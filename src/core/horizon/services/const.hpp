#pragma once

#include "core/horizon/const.hpp"
#include "core/horizon/hipc.hpp"

namespace Hydra::HW::TegraX1::CPU {
class MMUBase;
}

namespace Hydra::Horizon::Services {

u8* get_buffer_ptr(const HW::TegraX1::CPU::MMUBase* mmu,
                   const Hipc::BufferDescriptor& descriptor, usize size);

u8* get_static_ptr(const HW::TegraX1::CPU::MMUBase* mmu,
                   const Hipc::StaticDescriptor& descriptor, usize size);

u8* get_list_entry_ptr(const HW::TegraX1::CPU::MMUBase* mmu,
                       const Hipc::RecvListEntry& descriptor, usize size);

#define CREATE_READERS_OR_WRITERS(buffer_or_static, reader_or_writer, type)    \
    type##_##buffer_or_static##s_##reader_or_writer##s.reserve(                \
        hipc_in.meta.num_##type##_##buffer_or_static##s);                      \
    for (u32 i = 0; i < hipc_in.meta.num_##type##_##buffer_or_static##s;       \
         i++) {                                                                \
        usize size;                                                            \
        u8* ptr = get_##buffer_or_static##_ptr(                                \
            mmu, hipc_in.data.type##_##buffer_or_static##s[i], size);          \
        if (!ptr)                                                              \
            continue;                                                          \
        type##_##buffer_or_static##s_##reader_or_writer##s.emplace_back(ptr,   \
                                                                        size); \
    }

#define CREATE_STATIC_READERS_OR_WRITERS(reader_or_writer, type)               \
    CREATE_READERS_OR_WRITERS(static, reader_or_writer, type)
#define CREATE_BUFFER_READERS_OR_WRITERS(reader_or_writer, type)               \
    CREATE_READERS_OR_WRITERS(buffer, reader_or_writer, type)

struct Readers {
    Reader reader;
    std::vector<Reader> send_statics_readers;
    std::vector<Reader> send_buffers_readers;
    std::vector<Reader> exch_buffers_readers;

    Readers(const HW::TegraX1::CPU::MMUBase* mmu, Hipc::ParsedRequest hipc_in)
        : reader(align_ptr((u8*)hipc_in.data.data_words, 0x10),
                 hipc_in.meta.num_data_words * sizeof(u32)) {
        CREATE_STATIC_READERS_OR_WRITERS(reader, send);
        CREATE_BUFFER_READERS_OR_WRITERS(reader, send);
        CREATE_BUFFER_READERS_OR_WRITERS(reader, exch);
    }
};

struct Writers {
    Writer writer;
    std::vector<Writer> recv_list_writers;
    std::vector<Writer> recv_buffers_writers;
    std::vector<Writer> exch_buffers_writers;
    Writer objects_writer;
    Writer move_handles_writer;
    Writer copy_handles_writer;

    Writers(const HW::TegraX1::CPU::MMUBase* mmu, Hipc::ParsedRequest hipc_in,
            u8* scratch_buffer, u8* scratch_buffer_objects,
            u8* scratch_buffer_move_handles, u8* scratch_buffer_copy_handles)
        : writer(scratch_buffer, 0x1000),
          objects_writer(scratch_buffer_objects, 0x1000),
          move_handles_writer(scratch_buffer_move_handles, 0x1000),
          copy_handles_writer(scratch_buffer_copy_handles, 0x1000) {
        recv_list_writers.reserve(hipc_in.meta.num_recv_statics);
        for (u32 i = 0; i < hipc_in.meta.num_recv_statics; i++) {
            usize size;
            u8* ptr = get_list_entry_ptr(mmu, hipc_in.data.recv_list[i], size);
            if (!ptr)
                continue;
            recv_list_writers.emplace_back(ptr, size);
        }
        CREATE_BUFFER_READERS_OR_WRITERS(writer, recv);
        CREATE_BUFFER_READERS_OR_WRITERS(writer, exch);
    }
};

#undef CREATE_READERS_OR_WRITERS

} // namespace Hydra::Horizon::Services
