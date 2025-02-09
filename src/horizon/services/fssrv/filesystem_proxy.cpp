#include "horizon/services/fssrv/filesystem_proxy.hpp"

#include "horizon/cmif.hpp"

namespace Hydra::Horizon::Services::Fssrv {

void FileSystemProxy::Request(Kernel& kernel, Writer& writer,
                              Writer& move_handles_writer, u8* in_ptr) {
    auto cmif_in = cmif_read_in_header(in_ptr);

    Result* res = cmif_write_out_header(writer);

    switch (cmif_in.command_id) {
    default:
        printf("Unknown fssrv::filesystem_proxy request %u\n",
               cmif_in.command_id);
        break;
    }

    *res = RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::Fssrv
