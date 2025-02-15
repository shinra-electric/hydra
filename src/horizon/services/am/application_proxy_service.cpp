#include "horizon/services/am/application_proxy_service.hpp"

#include "horizon/cmif.hpp"
#include "horizon/kernel.hpp"
#include "horizon/services/am/application_proxy.hpp"

namespace Hydra::Horizon::Services::Am {

void IApplicationProxyService::Request(
    Readers& readers, Writers& writers,
    std::function<void(ServiceBase*)> add_service) {
    auto cmif_in = readers.reader.Read<Cmif::InHeader>();

    Result* res = Cmif::write_out_header(writers.writer);

    switch (cmif_in.command_id) {
    case 0: {
        LOG_DEBUG(HorizonServices, "OpenApplicationProxy");

        add_service(new IApplicationProxy());

        break;
    }
    default:
        LOG_WARNING(HorizonServices, "Unknown request {}", cmif_in.command_id);
        break;
    }

    *res = RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::Am
