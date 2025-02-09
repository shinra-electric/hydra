#include "horizon/services/sm.hpp"

#include "horizon/cmif.hpp"
#include "horizon/kernel.hpp"
#include "horizon/services/fssrv/filesystem_proxy.hpp"
#include "horizon/services/hid/hid_server.hpp"
#include "horizon/services/nvdrv/nvdrv_services.hpp"
#include "horizon/services/time/static_service.hpp"

namespace Hydra::Horizon::Services {

struct GetServiceHandleIn {
    char name[8];
};

void ServiceManager::Request(Kernel& kernel, Writer& writer,
                             Writer& move_handles_writer, u8* in_ptr) {
    auto cmif_in = Cmif::read_in_header(in_ptr);

    Result* res = Cmif::write_out_header(writer);

    switch (cmif_in.command_id) {
    case 1: {
        printf("Get service handle\n");
        auto in = *reinterpret_cast<GetServiceHandleIn*>(in_ptr);
        std::string name(in.name);
        Handle handle;
        if (name == "hid") {
            handle = kernel.AddService<Hid::HidServer>();
        } else if (name == "fsp-srv") {
            handle = kernel.AddService<Fssrv::FileSystemProxy>();
        } else if (name == "time:u" || name == "time:a" || name == "time:r") {
            // TODO: are all these the same?
            handle = kernel.AddService<Time::StaticService>();
        } else if (name == "nvdrv") {
            handle = kernel.AddService<Nvdrv::NvDrvServices>();
        } else {
            printf("Unknown service name \"%s\"\n", name.c_str());
            handle = UINT32_MAX;
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
