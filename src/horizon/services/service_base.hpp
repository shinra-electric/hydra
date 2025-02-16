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
                   Hipc::BufferDescriptor* descriptor);

inline Reader create_buffer_reader(const HW::MMU::MMUBase* mmu,
                                   Hipc::BufferDescriptor* descriptor) {
    if (!descriptor)
        return Reader(nullptr);

    return Reader(get_buffer_ptr(mmu, descriptor));
}

inline Writer create_buffer_writer(const HW::MMU::MMUBase* mmu,
                                   Hipc::BufferDescriptor* descriptor) {
    if (!descriptor)
        return Writer(nullptr);

    return Writer(get_buffer_ptr(mmu, descriptor));
}

struct Readers {
    Reader reader;
    Reader send_buffers_reader;
    Reader exch_buffers_reader;
};

struct Writers {
    Writer writer;
    Writer recv_buffers_writer;
    Writer exch_buffers_writer;
    Writer objects_writer;
    Writer move_handles_writer;
    Writer copy_handles_writer;
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
