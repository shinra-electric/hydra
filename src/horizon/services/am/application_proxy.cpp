#include "horizon/services/am/application_proxy.hpp"

#include "horizon/cmif.hpp"
#include "horizon/services/am/common_state_getter.hpp"

namespace Hydra::Horizon::Services::Am {

void IApplicationProxy::Request(Writers& writers, Reader& reader,
                                std::function<void(ServiceBase*)> add_service) {
    auto cmif_in = reader.Read<Cmif::InHeader>();

    Result* res = Cmif::write_out_header(writers.writer);

    switch (cmif_in.command_id) {
    case 0: // GetCommonStateGetter
        add_service(new ICommonStateGetter());
        break;
    default:
        LOG_WARNING(HorizonServices, "Unknown request {}", cmif_in.command_id);
        break;
    }

    *res = RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::Am
