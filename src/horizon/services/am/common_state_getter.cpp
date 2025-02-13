#include "horizon/services/am/common_state_getter.hpp"

#include "horizon/cmif.hpp"
#include "horizon/services/am/const.hpp"

namespace Hydra::Horizon::Services::Am {

void ICommonStateGetter::Request(
    Writers& writers, Reader& reader,
    std::function<void(ServiceBase*)> add_service) {
    auto cmif_in = reader.Read<Cmif::InHeader>();

    Result* res = Cmif::write_out_header(writers.writer);

    switch (cmif_in.command_id) {
    case 1: // ReceiveMessage
        ReceiveMessage(writers);
        break;
    case 9: // GetCurrentFocusState
        GetCurrentFocusState(writers);
        break;
    default:
        LOG_WARNING(HorizonServices, "Unknown request {}", cmif_in.command_id);
        break;
    }

    *res = RESULT_SUCCESS;
}

void ICommonStateGetter::ReceiveMessage(Writers& writers) {
    writers.writer.Write(APPLET_NO_MESSAGE); // No message
    LOG_WARNING(HorizonServices, "Not implemented");
}

void ICommonStateGetter::GetCurrentFocusState(Writers& writers) {
    writers.writer.Write(AppletFocusState::InFocus);
}

} // namespace Hydra::Horizon::Services::Am
