#pragma once

#include "horizon/const.hpp"
#include "horizon/hipc.hpp"

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

namespace Hydra::HW::MMU {
class MMUBase;
}

namespace Hydra::Horizon {
class Kernel;
}

namespace Hydra::Horizon::Services {

u8* get_buffer_ptr(const HW::MMU::MMUBase* mmu,
                   const Hipc::BufferDescriptor& descriptor);

struct Readers {
    Reader reader;
    std::vector<Reader> send_buffers_readers;
    std::vector<Reader> exch_buffers_readers;

    Readers(const HW::MMU::MMUBase* mmu, Hipc::ParsedRequest hipc_in)
        : reader(align_ptr((u8*)hipc_in.data.data_words, 0x10)) {
        send_buffers_readers.reserve(hipc_in.meta.num_send_buffers);
        for (u32 i = 0; i < hipc_in.meta.num_send_buffers; i++)
            send_buffers_readers.emplace_back(
                get_buffer_ptr(mmu, hipc_in.data.send_buffers[i]));

        exch_buffers_readers.reserve(hipc_in.meta.num_exch_buffers);
        for (u32 i = 0; i < hipc_in.meta.num_exch_buffers; i++)
            exch_buffers_readers.emplace_back(
                get_buffer_ptr(mmu, hipc_in.data.exch_buffers[i]));
    }
};

struct Writers {
    Writer writer;
    std::vector<Writer> recv_buffers_writers;
    std::vector<Writer> exch_buffers_writers;
    Writer objects_writer;
    Writer move_handles_writer;
    Writer copy_handles_writer;

    Writers(const HW::MMU::MMUBase* mmu, Hipc::ParsedRequest hipc_in,
            u8* scratch_buffer, u8* scratch_buffer_objects,
            u8* scratch_buffer_move_handles, u8* scratch_buffer_copy_handles)
        : writer(scratch_buffer), objects_writer(scratch_buffer_objects),
          move_handles_writer(scratch_buffer_move_handles),
          copy_handles_writer(scratch_buffer_copy_handles) {
        recv_buffers_writers.reserve(hipc_in.meta.num_recv_buffers);
        for (u32 i = 0; i < hipc_in.meta.num_recv_buffers; i++)
            recv_buffers_writers.emplace_back(
                get_buffer_ptr(mmu, hipc_in.data.recv_buffers[i]));

        exch_buffers_writers.reserve(hipc_in.meta.num_exch_buffers);
        for (u32 i = 0; i < hipc_in.meta.num_exch_buffers; i++)
            exch_buffers_writers.emplace_back(
                get_buffer_ptr(mmu, hipc_in.data.exch_buffers[i]));
    }
};

class ServiceBase {
  public:
    virtual ServiceBase* Clone() const = 0;

    virtual void Request(REQUEST_PARAMS);
    void Control(Reader& reader, Writer& writer);

    // Getters
    // Handle GetHandle() const { return handle; }

    // Setters
    void SetHandle(Handle handle_) { handle = handle_; }

  protected:
    virtual void RequestImpl(REQUEST_IMPL_PARAMS) {}

  private:
    Handle handle;
};

} // namespace Hydra::Horizon::Services
