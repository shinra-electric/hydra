#include "horizon/services/domain_service.hpp"

#include "horizon/cmif.hpp"

namespace Hydra::Horizon::Services {

void DomainService::Request(Writers& writers, Reader& reader,
                            std::function<void(ServiceBase*)> add_service) {
    LOG_DEBUG(HorizonServices, "Domain service request");

    // Domain in
    auto cmif_in = reader.Read<Cmif::DomainInHeader>();
    LOG_DEBUG(HorizonServices, "Object ID: 0x{:08x}", cmif_in.object_id);
    auto subservice = object_pool[cmif_in.object_id];

    Cmif::write_domain_out_header(writers.writer);

    switch (cmif_in.type) {
    case 1:
        subservice->Request(writers, reader, [&](ServiceBase* service) {
            Handle handle = AddObject(service);
            writers.objects_writer.Write(handle);
        });
        break;
    default:
        LOG_WARNING(HorizonServices, "Unknown domain request type {}",
                    cmif_in.type);
        break;
    }
}

} // namespace Hydra::Horizon::Services
