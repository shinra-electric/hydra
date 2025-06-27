#include "core/horizon/services/fatalsrv/service.hpp"

#include "core/debugger/debugger.hpp"

namespace hydra::horizon::services::fatalsrv {

DEFINE_SERVICE_COMMAND_TABLE(IService, 2, ThrowFatalWithCpuContext)

result_t
IService::ThrowFatalWithCpuContext(u64 code, u64 _unknown_x8,
                                   InBuffer<BufferAttr::MapAlias> in_buffer) {
    // TODO: print properly
    LOG_INFO(Services, "Error: 0x{:x}", code);

    // TODO: stack trace and other stuff in the buffer

    DEBUGGER_INSTANCE.BreakOnThisThread(
        "fatalsrv::IService::ThrowFatalWithCpuContext");

    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::fatalsrv
