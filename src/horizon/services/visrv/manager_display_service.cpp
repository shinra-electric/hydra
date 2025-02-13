#include "horizon/services/visrv/manager_display_service.hpp"

#include "horizon/cmif.hpp"

namespace Hydra::Horizon::Services::ViSrv {

void IManagerDisplayService::Request(
    Writers& writers, Reader& reader,
    std::function<void(ServiceBase*)> add_service) {
    auto cmif_in = reader.Read<Cmif::InHeader>();

    Result* res = Cmif::write_out_header(writers.writer);

    switch (cmif_in.command_id) {
    default:
        LOG_WARNING(HorizonServices, "Unknown request {}", cmif_in.command_id);
        break;
    }

    *res = RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::ViSrv
