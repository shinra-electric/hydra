#include "horizon/services/domain_service.hpp"

#include "horizon/cmif.hpp"

namespace Hydra::Horizon::Services {

void DomainService::Request(Writers& writers, u8* in_ptr,
                            std::function<void(ServiceBase*)> add_service) {
    Logging::log(Logging::Level::Debug, "Domain service request");

    // Domain in
    auto cmif_in = Cmif::read_domain_in_header(in_ptr);
    Logging::log(Logging::Level::Debug, "Object ID: 0x{:08x}",
                 cmif_in.object_id);
    auto subservice = object_pool[cmif_in.object_id];

    Cmif::write_domain_out_header(writers.writer);

    switch (cmif_in.type) {
    case 1:
        subservice->Request(writers, in_ptr, [&](ServiceBase* service) {
            Handle handle = AddObject(service);
            writers.objects_writer.Write(handle);
        });
        break;
    default:
        Logging::log(Logging::Level::Warning, "Unknown domain request type {}",
                     cmif_in.type);
        break;
    }
}

} // namespace Hydra::Horizon::Services
