#include "horizon/services/nvdrv/nvdrv_services.hpp"

#include "horizon/const.hpp"
#include "horizon/services/nvdrv/const.hpp"
#include "horizon/services/nvdrv/ioctl/nvmap.hpp"

namespace Hydra::Horizon::Services::NvDrv {

DEFINE_SERVICE_COMMAND_TABLE(INvDrvServices, 0, Open, 1, Ioctl)

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

struct IoctlIn {
    Handle handle;
    u32 code;
};

void INvDrvServices::Ioctl(REQUEST_COMMAND_PARAMS) {
    auto in = readers.reader.Read<IoctlIn>();
    auto ioctl = ioctl_pool.GetObject(in.handle);

    u32 nr = in.code & 0xFF;
    NvResult r = NvResult::Success;
    ioctl->Ioctl(readers.send_buffers_reader, writers.recv_buffers_writer, nr,
                 r);

    writers.writer.Write(r);

    if (r != NvResult::Success)
        result = MAKE_KERNEL_RESULT(NotFound); // TODO: what should this be?
}

} // namespace Hydra::Horizon::Services::NvDrv
