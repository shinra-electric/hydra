#include "horizon/services/nvdrv/nvdrv_services.hpp"

#include "horizon/const.hpp"
#include "horizon/services/nvdrv/const.hpp"
#include "horizon/services/nvdrv/ioctl/nvhost_as_gpu.hpp"
#include "horizon/services/nvdrv/ioctl/nvhost_ctrl.hpp"
#include "horizon/services/nvdrv/ioctl/nvhost_ctrl_gpu.hpp"
#include "horizon/services/nvdrv/ioctl/nvmap.hpp"

namespace Hydra::Horizon::Services::NvDrv {

DEFINE_SERVICE_COMMAND_TABLE(INvDrvServices, 0, Open, 1, Ioctl)

void INvDrvServices::Open(REQUEST_COMMAND_PARAMS) {
    auto path = readers.send_buffers_readers[0].ReadString();
    Handle handle = ioctl_pool.AllocateForIndex();
    if (path == "/dev/nvhost-ctrl") {
        ioctl_pool.GetObjectRef(handle) = new Ioctl::NvHostCtrl();
    } else if (path == "/dev/nvmap") {
        ioctl_pool.GetObjectRef(handle) = new Ioctl::NvMap();
    } else if (path == "/dev/nvhost-as-gpu") {
        ioctl_pool.GetObjectRef(handle) = new Ioctl::NvHostAsGpu();
    } else if (path == "/dev/nvhost-ctrl-gpu") {
        ioctl_pool.GetObjectRef(handle) = new Ioctl::NvHostCtrlGpu();
    } else {
        LOG_WARNING(HorizonServices, "Unknown path \"{}\"", path);
        // TODO: don't throw
        throw;
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

    // Reader
    Reader* reader = nullptr;
    if (readers.send_buffers_readers.size() != 0)
        reader = &readers.send_buffers_readers[0];

    // Writer
    Writer* writer = nullptr;
    if (writers.recv_buffers_writers.size() != 0)
        writer = &writers.recv_buffers_writers[0];

    // Dispatch
    u32 nr = in.code & 0xFF;
    NvResult r = NvResult::Success;
    ioctl->Ioctl(reader, writer, nr, r);

    // Write result
    writers.writer.Write(r);

    if (r != NvResult::Success)
        result = MAKE_KERNEL_RESULT(NotFound); // TODO: what should this be?
}

} // namespace Hydra::Horizon::Services::NvDrv
