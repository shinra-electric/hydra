#include "horizon/services/nvdrv/nvdrv_services.hpp"

#include "horizon/services/nvdrv/ioctl/nvmap.hpp"

namespace Hydra::Horizon::Services::NvDrv {

DEFINE_SERVICE_COMMAND_TABLE(INvDrvServices, 0, Open)

void INvDrvServices::Open(REQUEST_COMMAND_PARAMS) {
    auto path = readers.send_buffers_reader.ReadString();
    Handle handle = 0;
    if (path == "/dev/nvmap") {
        handle = ioctl_pool.AllocateForIndex();
        ioctl_pool.GetObjectRef(handle) = new Ioctl::NvMap();
    } else {
        LOG_WARNING(HorizonServices, "Unknown path \"{}\"", path);
    }

    writers.writer.Write(handle);
}

} // namespace Hydra::Horizon::Services::NvDrv
