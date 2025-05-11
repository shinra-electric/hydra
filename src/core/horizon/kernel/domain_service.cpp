#include "core/horizon/kernel/domain_service.hpp"

#include "core/horizon/kernel/cmif.hpp"
#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Kernel {

void DomainService::Request(RequestContext& context) {
    LOG_DEBUG(HorizonServices, "Domain service request");

    // Domain in
    auto cmif_in = context.readers.reader.Read<Cmif::DomainInHeader>();
    LOG_DEBUG(HorizonServices, "Object ID: 0x{:08x}", cmif_in.object_id);
    auto subservice = object_pool.GetObject(cmif_in.object_id);

    if (cmif_in.num_in_objects != 0) {
        auto objects = context.readers.reader.GetPtr() + cmif_in.data_size;
        context.readers.objects_reader =
            new Reader(objects, cmif_in.num_in_objects * sizeof(handle_id_t));
    }

    Cmif::write_domain_out_header(context.writers.writer);

    switch (cmif_in.type) {
    case Cmif::DomainCommandType::SendMessage: {
        RequestContext subcontext{
            context.readers,
            context.writers,
            [&](ServiceBase* service) {
                handle_id_t handle_id = AddObject(service);
                context.writers.objects_writer.Write(handle_id);
            },
            [&](handle_id_t handle_id) {
                return object_pool.GetObject(handle_id);
            },
        };
        subservice->Request(subcontext);
        break;
    }
    case Cmif::DomainCommandType::Close:
        delete subservice;
        object_pool.FreeByIndex(cmif_in.object_id);
        LOG_DEBUG(HorizonKernel, "Closed subservice");
        break;
    default:
        LOG_WARN(HorizonKernel, "Unknown domain request type {}", cmif_in.type);
        break;
    }
}

} // namespace Hydra::Horizon::Kernel
