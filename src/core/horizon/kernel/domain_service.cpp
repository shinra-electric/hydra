#include "core/horizon/kernel/domain_service.hpp"

#include "core/horizon/kernel/cmif.hpp"
#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Kernel {

void DomainService::Request(REQUEST_PARAMS) {
    LOG_DEBUG(HorizonServices, "Domain service request");

    // Domain in
    auto cmif_in = readers.reader.Read<Cmif::DomainInHeader>();
    LOG_DEBUG(HorizonServices, "Object ID: 0x{:08x}", cmif_in.object_id);
    auto subservice = object_pool.GetObject(cmif_in.object_id);

    if (cmif_in.num_in_objects != 0) {
        auto objects = readers.reader.GetPtr() + cmif_in.data_size;
        readers.objects_reader =
            new Reader(objects, cmif_in.num_in_objects * sizeof(handle_id_t));
    }

    Cmif::write_domain_out_header(writers.writer);

    switch (cmif_in.type) {
    case Cmif::DomainCommandType::SendMessage:
        subservice->Request(
            readers, writers,
            [&](ServiceBase* service) {
                handle_id_t handle_id = AddObject(service);
                writers.objects_writer.Write(handle_id);
            },
            [&](handle_id_t handle_id) {
                return object_pool.GetObject(handle_id);
            });
        break;
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
