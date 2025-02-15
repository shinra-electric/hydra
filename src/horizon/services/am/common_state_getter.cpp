#include "horizon/services/am/common_state_getter.hpp"

#include "horizon/services/am/const.hpp"

namespace Hydra::Horizon::Services::Am {

void ICommonStateGetter::RequestImpl(
    Readers& readers, Writers& writers,
    std::function<void(ServiceBase*)> add_service, Result& result, u32 id) {
    switch (id) {
    case 1: // ReceiveMessage
        ReceiveMessage(writers);
        break;
    case 9: // GetCurrentFocusState
        GetCurrentFocusState(writers);
        break;
    default:
        LOG_WARNING(HorizonServices, "Unknown request {}", id);
        break;
    }
}

void ICommonStateGetter::ReceiveMessage(Writers& writers) {
    writers.writer.Write(APPLET_NO_MESSAGE); // No message
    LOG_WARNING(HorizonServices, "Not implemented");
}

void ICommonStateGetter::GetCurrentFocusState(Writers& writers) {
    writers.writer.Write(AppletFocusState::InFocus);
}

} // namespace Hydra::Horizon::Services::Am
