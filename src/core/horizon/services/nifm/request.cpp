#include "core/horizon/services/nifm/request.hpp"

namespace hydra::horizon::services::nifm {

DEFINE_SERVICE_COMMAND_TABLE(IRequest, 0, GetRequestState, 1, GetResult, 2,
                             GetSystemEventReadableHandles, 3, Cancel, 4,
                             Submit)

// TODO: how come is the autoclear of the second one user-specified?
IRequest::IRequest()
    : events{{new kernel::Event(kernel::EventFlags::AutoClear,
                                "IRequest system event")},
             {new kernel::Event(kernel::EventFlags::AutoClear,
                                "IRequest system event")}} {}

result_t IRequest::GetRequestState(RequestState* out_state) {
    LOG_FUNC_STUBBED(Services);

    // HACK
    // TODO: why does Minecraft not work with RequestState::Accepted?
    *out_state = RequestState::Invalid;
    return RESULT_SUCCESS;
}

result_t IRequest::GetSystemEventReadableHandles(
    OutHandle<HandleAttr::Copy> out_handle0,
    OutHandle<HandleAttr::Copy> out_handle1) {
    out_handle0 = events[0].id;
    out_handle1 = events[1].id;
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::nifm
