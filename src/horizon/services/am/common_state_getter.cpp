#include "horizon/services/am/common_state_getter.hpp"

#include "horizon/services/service_base.hpp"
#include "horizon/state_manager.hpp"

namespace Hydra::Horizon::Services::Am {

DEFINE_SERVICE_COMMAND_TABLE(ICommonStateGetter, 0, GetEventHandle, 1,
                             ReceiveMessage, 9, GetCurrentFocusState)

void ICommonStateGetter::GetEventHandle(REQUEST_COMMAND_PARAMS) {
    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    writers.writer.Write(invalid<HandleId>());
}

void ICommonStateGetter::ReceiveMessage(REQUEST_COMMAND_PARAMS) {
    writers.writer.Write(StateManager::GetInstance().ReceiveMessage());
}

void ICommonStateGetter::GetCurrentFocusState(REQUEST_COMMAND_PARAMS) {
    writers.writer.Write(StateManager::GetInstance().GetFocusState());
}

} // namespace Hydra::Horizon::Services::Am
