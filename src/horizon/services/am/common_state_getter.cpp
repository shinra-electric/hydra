#include "horizon/services/am/common_state_getter.hpp"

#include "horizon/services/am/const.hpp"
#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Am {

DEFINE_SERVICE_COMMAND_TABLE(ICommonStateGetter, 1, ReceiveMessage, 9,
                             GetCurrentFocusState)

void ICommonStateGetter::ReceiveMessage(REQUEST_COMMAND_PARAMS) {
    writers.writer.Write(APPLET_NO_MESSAGE); // No message
    LOG_WARNING(HorizonServices, "Not implemented");
}

void ICommonStateGetter::GetCurrentFocusState(REQUEST_COMMAND_PARAMS) {
    writers.writer.Write(AppletFocusState::InFocus);
}

} // namespace Hydra::Horizon::Services::Am
