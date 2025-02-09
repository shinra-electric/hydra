#include "horizon/services/nvdrv/nvdrv_services.hpp"

#include "horizon/cmif.hpp"

namespace Hydra::Horizon::Services::Nvdrv {

void NvDrvServices::Request(Kernel& kernel, Writer& writer,
                            Writer& move_handles_writer, u8* in_ptr) {
    auto cmif_in = Cmif::read_in_header(in_ptr);

    Result* res = Cmif::write_out_header(writer);

    switch (cmif_in.command_id) {
    default:
        printf("Unknown nvdrv::nvdrv_services request %u\n",
               cmif_in.command_id);
        break;
    }

    *res = RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::Nvdrv
