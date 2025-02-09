#include "horizon/services/service.hpp"

#include "horizon/cmif.hpp"
#include "horizon/const.hpp"
#include "horizon/kernel.hpp"
#include "horizon/services/domain_service.hpp"

namespace Hydra::Horizon::Services {

void ServiceBase::Control(Kernel& kernel, u8* out_ptr, usize& out_size,
                          u8* in_ptr) {
    auto cmif_in = cmif_read_in_header(in_ptr);

    Result* res = cmif_write_out_header(out_ptr, out_size);

    switch (cmif_in.command_id) {
    case 0: { // convert to domain
        printf("CONVERT TO DOMAIN\n");
        auto domain_service = new DomainService(handle);
        kernel.SetService(handle, domain_service);
        handle = domain_service->AddObject(this);

        // Out
        *((Handle*)out_ptr) = handle;
        out_size += sizeof(Handle);

        break;
    }
    default:
        printf("Unimplemented control request %u\n", cmif_in.command_id);
        break;
    }

    *res = RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services
