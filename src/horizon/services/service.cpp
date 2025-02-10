#include "horizon/services/service.hpp"

#include "horizon/cmif.hpp"
#include "horizon/const.hpp"
#include "horizon/kernel.hpp"
#include "horizon/services/domain_service.hpp"

namespace Hydra::Horizon::Services {

void ServiceBase::Control(Kernel& kernel, Writer& writer, u8* in_ptr) {
    auto cmif_in = Cmif::read_in_header(in_ptr);

    Result* res = Cmif::write_out_header(writer);

    switch (cmif_in.command_id) {
    case 0: { // convert to domain
        LOG_DEBUG(HorizonServices, "CONVERT TO DOMAIN");

        auto domain_service = new DomainService();
        kernel.SetService(handle, domain_service);
        handle = domain_service->AddObject(this);

        // Out
        writer.Write(handle);

        break;
    }
    default:
        LOG_WARNING(HorizonServices, "Unimplemented control request {}",
                    cmif_in.command_id);
        break;
    }

    *res = RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services
