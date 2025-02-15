#include "horizon/services/am/common_state_getter.hpp"

#include "horizon/services/am/const.hpp"
#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Am {

void ICommonStateGetter::RequestImpl(REQUEST_IMPL_PARAMS) {
    switch (id) {
    case 1: // ReceiveMessage
        CmdReceiveMessage(PASS_REQUEST_PARAMS_WITH_RESULT);
        break;
    case 9: // GetCurrentFocusState
        CmdGetCurrentFocusState(PASS_REQUEST_PARAMS_WITH_RESULT);
        break;
    default:
        LOG_WARNING(HorizonServices, "Unknown request {}", id);
        break;
    }
}

void ICommonStateGetter::CmdReceiveMessage(REQUEST_PARAMS_WITH_RESULT) {
    writers.writer.Write(APPLET_NO_MESSAGE); // No message
    LOG_WARNING(HorizonServices, "Not implemented");
}

void ICommonStateGetter::CmdGetCurrentFocusState(REQUEST_PARAMS_WITH_RESULT) {
    writers.writer.Write(AppletFocusState::InFocus);
}

} // namespace Hydra::Horizon::Services::Am
