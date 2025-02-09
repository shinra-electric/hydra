#include "horizon/services/domain_service.hpp"

#include "horizon/cmif.hpp"

namespace Hydra::Horizon::Services {

void DomainService::Request(Kernel& kernel, u8* out_ptr, usize& out_size,
                            u8* in_ptr) {
    printf("Domain service request\n");

    // Domain in
    auto domain_in = *reinterpret_cast<CmifDomainInHeader*>(in_ptr);
    in_ptr += sizeof(CmifDomainInHeader);
    printf("Object ID: 0x%08x\n", domain_in.object_id);
    auto subservice = object_pool[domain_in.object_id];

    auto domain_out = reinterpret_cast<CmifDomainOutHeader*>(out_ptr);
    out_ptr += sizeof(CmifDomainOutHeader);

    switch (domain_in.type) {
    case 1:
        subservice->Request(kernel, out_ptr, out_size, in_ptr);
        break;
    default:
        printf("Unknown domain request type: %u\n", domain_in.type);
        break;
    }

    // Domain out
    *domain_out = {
        .num_out_objects = 0,
    };
}

} // namespace Hydra::Horizon::Services
