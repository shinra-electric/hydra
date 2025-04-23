#pragma once

#include "core/horizon/kernel/hipc.hpp"

#define REQUEST_PARAMS                                                         \
    ::Hydra::Horizon::Kernel::Hipc::Readers &readers,                          \
        ::Hydra::Horizon::Kernel::Hipc::Writers &writers,                      \
        const std::function<void(ServiceBase*)>&add_service
#define REQUEST_COMMAND_PARAMS                                                 \
    REQUEST_PARAMS, ::Hydra::Horizon::Kernel::Result& result
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
            LOG_WARN(HorizonServices, "Unknown request {}", id);               \
            break;                                                             \
        }                                                                      \
    }

#define STUB_REQUEST_COMMAND(name)                                             \
    void name(REQUEST_COMMAND_PARAMS) { LOG_FUNC_STUBBED(HorizonServices); }

namespace Hydra::Horizon::Kernel {

// TODO: remove this
#define DEFINE_SERVICE_VIRTUAL_FUNCTIONS(type)

class ServiceBase {
  public:
    virtual ~ServiceBase() = default;

    virtual void Request(REQUEST_PARAMS);
    virtual usize GetPointerBufferSize() { return 0; }

  protected:
    virtual void RequestImpl(REQUEST_IMPL_PARAMS) = 0;
};

} // namespace Hydra::Horizon::Kernel
