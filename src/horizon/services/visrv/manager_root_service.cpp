#include "horizon/services/visrv/manager_root_service.hpp"

#include "horizon/cmif.hpp"
#include "horizon/services/visrv/application_display_service.hpp"

namespace Hydra::Horizon::Services::ViSrv {

void IManagerRootService::Request(
    Readers& readers, Writers& writers,
    std::function<void(ServiceBase*)> add_service) {
    auto cmif_in = readers.reader.Read<Cmif::InHeader>();

    Result* res = Cmif::write_out_header(writers.writer);

    switch (cmif_in.command_id) {
    case 0: // GetDisplayService
        add_service(new IApplicationDisplayService());
        break;
    case 2: // GetDisplayServiceWithProxyNameExchange
        // TODO: should take input u64 and u32
        add_service(new IApplicationDisplayService());
        break;
    default:
        LOG_WARNING(HorizonServices, "Unknown request {}", cmif_in.command_id);
        break;
    }

    *res = RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::ViSrv
