#include "horizon/services/sm.hpp"

#include "horizon/cmif.hpp"
#include "horizon/kernel.hpp"
#include "horizon/services/fsp_srv.hpp"
#include "horizon/services/hid.hpp"

namespace Hydra::Horizon::Services {

struct GetServiceHandleIn {
    char name[8];
};

void ServiceManager::Request(Kernel& kernel, Writer& writer,
                             Writer& move_handles_writer, u8* in_ptr) {
    auto cmif_in = cmif_read_in_header(in_ptr);

    Result* res = cmif_write_out_header(writer);

    switch (cmif_in.command_id) {
    case 1: {
        printf("Get service handle\n");
        auto in = *reinterpret_cast<GetServiceHandleIn*>(in_ptr);
        std::string name(in.name);
        Handle handle{UINT32_MAX};
        if (name == "hid") {
            handle = kernel.AddService<Services::HidServer>();
        } else if (name == "fsp-srv") {
            handle = kernel.AddService<Services::HidServer>();
        } else {
            printf("Unknown service name \"%s\"\n", name.c_str());
        }

        // Out
        move_handles_writer.Write(handle);

        break;
    }
    default:
        printf("Unknown sm command %u\n", cmif_in.command_id);
        break;
    }

    *res = RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services
