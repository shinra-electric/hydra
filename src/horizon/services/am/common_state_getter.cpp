#include "horizon/services/am/common_state_getter.hpp"

#include "horizon/services/service_base.hpp"
#include "horizon/state_manager.hpp"

namespace Hydra::Horizon::Services::Am {

DEFINE_SERVICE_COMMAND_TABLE(ICommonStateGetter, 1, ReceiveMessage, 9,
                             GetCurrentFocusState)

void ICommonStateGetter::ReceiveMessage(REQUEST_COMMAND_PARAMS) {
    LOG_DEBUG(HorizonServices, "ReceiveMessage");
    writers.writer.Write(StateManager::GetInstance().ReceiveMessage());
}

void ICommonStateGetter::GetCurrentFocusState(REQUEST_COMMAND_PARAMS) {
    LOG_DEBUG(HorizonServices, "GetCurrentFocusState");
    writers.writer.Write(StateManager::GetInstance().GetFocusState());
}

} // namespace Hydra::Horizon::Services::Am
