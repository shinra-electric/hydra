#include "core/horizon/services/nifm/request.hpp"

namespace Hydra::Horizon::Services::Nifm {

namespace {

enum class RequestState {
    Invalid,
    Free,
    OnHold,
    Accepted,
    Blocking,
};

}

DEFINE_SERVICE_COMMAND_TABLE(IRequest, 0, GetRequestState, 1, GetResult, 2,
                             GetSystemEventReadableHandles)

IRequest::IRequest() : events{{new Kernel::Event()}, {new Kernel::Event()}} {}

void IRequest::GetRequestState(REQUEST_COMMAND_PARAMS) {
    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    writers.writer.Write(RequestState::Accepted);
}

void IRequest::GetSystemEventReadableHandles(REQUEST_COMMAND_PARAMS) {
    writers.copy_handles_writer.Write(events[0].id);
    writers.copy_handles_writer.Write(events[1].id);
}

} // namespace Hydra::Horizon::Services::Nifm
