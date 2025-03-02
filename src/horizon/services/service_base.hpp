#pragma once

#include "horizon/const.hpp"
#include "horizon/hipc.hpp"
#include "horizon/kernel.hpp"

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

namespace Hydra::HW::TegraX1::CPU {
class MMUBase;
}

namespace Hydra::Horizon {
class Kernel;
}

namespace Hydra::Horizon::Services {

u8* get_buffer_ptr(const HW::TegraX1::CPU::MMUBase* mmu,
                   const Hipc::BufferDescriptor& descriptor);

#define CREATE_READERS_OR_WRITERS(reader_or_buffer, type)                      \
    type##_buffers_##reader_or_buffer##s.reserve(                              \
        hipc_in.meta.num_##type##_buffers);                                    \
    for (u32 i = 0; i < hipc_in.meta.num_##type##_buffers; i++) {              \
        u8* ptr = get_buffer_ptr(mmu, hipc_in.data.type##_buffers[i]);         \
        if (!ptr)                                                              \
            continue;                                                          \
        type##_buffers_##reader_or_buffer##s.emplace_back(ptr);                \
    }

struct Readers {
    Reader reader;
    std::vector<Reader> send_buffers_readers;
    std::vector<Reader> exch_buffers_readers;

    Readers(const HW::TegraX1::CPU::MMUBase* mmu, Hipc::ParsedRequest hipc_in)
        : reader(align_ptr((u8*)hipc_in.data.data_words, 0x10)) {
        send_buffers_readers.reserve(hipc_in.meta.num_send_buffers);
        CREATE_READERS_OR_WRITERS(reader, send);
        CREATE_READERS_OR_WRITERS(reader, exch);
    }
};

struct Writers {
    Writer writer;
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
        CREATE_READERS_OR_WRITERS(writer, recv);
        CREATE_READERS_OR_WRITERS(writer, exch);
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

  private:
    HandleId handle_id;
};

} // namespace Hydra::Horizon::Services
