#pragma once

#include "core/horizon/const.hpp"
#include "core/horizon/hipc.hpp"
#include "core/horizon/kernel.hpp"

#define REQUEST_PARAMS                                                         \
    Readers &readers, Writers &writers,                                        \
        const std::function<void(ServiceBase*)>&add_service
#define REQUEST_COMMAND_PARAMS REQUEST_PARAMS, Result& result
#define REQUEST_IMPL_PARAMS REQUEST_COMMAND_PARAMS, u32 id

#define PASS_REQUEST_PARAMS readers, writers, add_service
#define PASS_REQUEST_COMMAND_PARAMS PASS_REQUEST_PARAMS, result
#define PASS_REQUEST_IMPL_PARAMS PASS_REQUEST_COMMAND_PARAMS, id

#define SERVICE_COMMAND_CASE(id, func)                                         \
    case id:                                                                   \
        LOG_DEBUG(HorizonServices, #func);                                     \
        func(PASS_REQUEST_COMMAND_PARAMS);                                     \
        break;

#define DEFINE_SERVICE_COMMAND_TABLE(service, ...)                             \
    void service::RequestImpl(REQUEST_IMPL_PARAMS) {                           \
        switch (id) {                                                          \
            FOR_EACH_0_2(SERVICE_COMMAND_CASE, __VA_ARGS__)                    \
        default:                                                               \
            LOG_WARNING(HorizonServices, "Unknown request {}", id);            \
            break;                                                             \
        }                                                                      \
    }

#define STUB_REQUEST_COMMAND(name)                                             \
    void name(REQUEST_COMMAND_PARAMS) { LOG_FUNC_STUBBED(HorizonServices); }

namespace Hydra::HW::TegraX1::CPU {
class MMUBase;
}

namespace Hydra::Horizon {
class Kernel;
}

namespace Hydra::Horizon::Services {

u8* get_buffer_ptr(const HW::TegraX1::CPU::MMUBase* mmu,
                   const Hipc::BufferDescriptor& descriptor);

u8* get_static_ptr(const HW::TegraX1::CPU::MMUBase* mmu,
                   const Hipc::StaticDescriptor& descriptor);

u8* get_list_entry_ptr(const HW::TegraX1::CPU::MMUBase* mmu,
                       const Hipc::RecvListEntry& descriptor);

#define CREATE_READERS_OR_WRITERS(buffer_or_static, reader_or_writer, type)    \
    type##_##buffer_or_static##s_##reader_or_writer##s.reserve(                \
        hipc_in.meta.num_##type##_##buffer_or_static##s);                      \
    for (u32 i = 0; i < hipc_in.meta.num_##type##_##buffer_or_static##s;       \
         i++) {                                                                \
        u8* ptr = get_##buffer_or_static##_ptr(                                \
            mmu, hipc_in.data.type##_##buffer_or_static##s[i]);                \
        if (!ptr)                                                              \
            continue;                                                          \
        type##_##buffer_or_static##s_##reader_or_writer##s.emplace_back(ptr);  \
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
        : reader(align_ptr((u8*)hipc_in.data.data_words, 0x10)) {
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
        : writer(scratch_buffer), objects_writer(scratch_buffer_objects),
          move_handles_writer(scratch_buffer_move_handles),
          copy_handles_writer(scratch_buffer_copy_handles) {
        recv_list_writers.reserve(hipc_in.meta.num_recv_statics);
        for (u32 i = 0; i < hipc_in.meta.num_recv_statics; i++) {
            u8* ptr = get_list_entry_ptr(mmu, hipc_in.data.recv_list[i]);
            if (!ptr)
                continue;
            recv_list_writers.emplace_back(ptr);
        }
        CREATE_BUFFER_READERS_OR_WRITERS(writer, recv);
        CREATE_BUFFER_READERS_OR_WRITERS(writer, exch);
    }
};

#undef CREATE_READERS_OR_WRITERS

#define DEFINE_SERVICE_VIRTUAL_FUNCTIONS(type)                                 \
    virtual type* Clone() const override { return new type(*this); }

class ServiceBase : public KernelHandle {
  public:
    virtual ServiceBase* Clone() const = 0;

    virtual void Request(REQUEST_PARAMS);
    void Control(Readers& readers, Writers& writers);

    // Getters
    // Handle GetHandle() const { return handle; }

    // Setters
    void SetHandleId(HandleId handle_id_) { handle_id = handle_id_; }

  protected:
    virtual void RequestImpl(REQUEST_IMPL_PARAMS) {}

    virtual usize GetPointerBufferSize() { return 0; }

  private:
    HandleId handle_id;
};

} // namespace Hydra::Horizon::Services
