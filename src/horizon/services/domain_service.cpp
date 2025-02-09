#include "horizon/services/domain_service.hpp"

#include "horizon/cmif.hpp"

namespace Hydra::Horizon::Services {

void DomainService::Request(Kernel& kernel, Writer& writer, u8* in_ptr) {
    printf("Domain service request\n");

    // Domain in
    auto cmif_in = cmif_read_domain_in_header(in_ptr);
    printf("Object ID: 0x%08x\n", cmif_in.object_id);
    auto subservice = object_pool[cmif_in.object_id];

    cmif_write_domain_out_header(writer);

    switch (cmif_in.type) {
    case 1:
        subservice->Request(kernel, writer, in_ptr);
        break;
    default:
        printf("Unknown domain request type: %u\n", cmif_in.type);
        break;
    }
}

} // namespace Hydra::Horizon::Services
