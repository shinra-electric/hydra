#include "horizon/services/fsp_srv.hpp"

#include "horizon/cmif.hpp"

namespace Hydra::Horizon::Services {

void FileSystemProxy::Request(Kernel& kernel, u8* out_ptr, usize& out_size,
                              u8* in_ptr) {
    auto cmif_in = cmif_read_in_header(in_ptr);

    Result* res = cmif_write_out_header(out_ptr, out_size);

    switch (cmif_in.command_id) {
    default:
        printf("Unknown fps-srv request %u\n", cmif_in.command_id);
        break;
    }

    *res = RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services
