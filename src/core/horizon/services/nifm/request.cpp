#include "core/horizon/services/nifm/request.hpp"

namespace Hydra::Horizon::Services::Nifm {

DEFINE_SERVICE_COMMAND_TABLE(IRequest, 0, GetRequestState, 1, GetResult, 2,
                             GetSystemEventReadableHandles)

IRequest::IRequest() : events{{new Kernel::Event()}, {new Kernel::Event()}} {}

result_t IRequest::GetRequestState(RequestState* out_state) {
    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    *out_state = RequestState::Accepted;
    return RESULT_SUCCESS;
}

result_t IRequest::GetSystemEventReadableHandles(
    OutHandle<HandleAttr::Copy> out_handle0,
    OutHandle<HandleAttr::Copy> out_handle1) {
    out_handle0 = events[0].id;
    out_handle1 = events[1].id;
    return RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::Nifm
