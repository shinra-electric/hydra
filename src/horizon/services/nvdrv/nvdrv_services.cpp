#include "horizon/services/nvdrv/nvdrv_services.hpp"

#include "horizon/const.hpp"
#include "horizon/services/nvdrv/const.hpp"
#include "horizon/services/nvdrv/ioctl/nvhost_as_gpu.hpp"
#include "horizon/services/nvdrv/ioctl/nvhost_ctrl.hpp"
#include "horizon/services/nvdrv/ioctl/nvhost_ctrl_gpu.hpp"
#include "horizon/services/nvdrv/ioctl/nvhost_gpu.hpp"
#include "horizon/services/nvdrv/ioctl/nvmap.hpp"

namespace Hydra::Horizon::Services::NvDrv {

namespace {

struct IoctlIn {
    HandleId fd_id;
    u32 code;
};

struct QueryEventIn {
    HandleId fd_id;
    u32 event_id;
};

} // namespace

Allocators::StaticPool<Ioctl::FdBase*, MAX_FD_COUNT> INvDrvServices::fd_pool;

DEFINE_SERVICE_COMMAND_TABLE(INvDrvServices, 0, Open, 1, Ioctl, 3, Initialize,
                             4, QueryEvent, 8, SetAruid)

void INvDrvServices::Open(REQUEST_COMMAND_PARAMS) {
    auto path = readers.send_buffers_readers[0].ReadString();
    HandleId handle_id = fd_pool.AllocateForIndex();
    if (path == "/dev/nvhost-ctrl") {
        fd_pool.GetObjectRef(handle_id) = new Ioctl::NvHostCtrl();
    } else if (path == "/dev/nvmap") {
        fd_pool.GetObjectRef(handle_id) = new Ioctl::NvMap();
    } else if (path == "/dev/nvhost-as-gpu") {
        fd_pool.GetObjectRef(handle_id) = new Ioctl::NvHostAsGpu();
    } else if (path == "/dev/nvhost-ctrl-gpu") {
        fd_pool.GetObjectRef(handle_id) = new Ioctl::NvHostCtrlGpu();
    } else if (path == "/dev/nvhost-gpu") {
        fd_pool.GetObjectRef(handle_id) = new Ioctl::NvHostGpu();
    } else {
        LOG_WARNING(HorizonServices, "Unknown path \"{}\"", path);
        // TODO: don't throw
        throw;
    }

    writers.writer.Write(handle_id);
}

void INvDrvServices::Ioctl(REQUEST_COMMAND_PARAMS) {
    auto in = readers.reader.Read<IoctlIn>();
    auto fd = fd_pool.GetObject(in.fd_id);

    // Reader
    Reader* reader = nullptr;
    if (readers.send_buffers_readers.size() != 0)
        reader = &readers.send_buffers_readers[0];

    // Writer
    Writer* writer = nullptr;
    if (writers.recv_buffers_writers.size() != 0)
        writer = &writers.recv_buffers_writers[0];

    // Dispatch
    u32 type = (in.code >> 8) & 0xff;
    u32 nr = in.code & 0xff;
    NvResult r = NvResult::Success;
    fd->Ioctl(reader, writer, type, nr, r);

    // Write result
    writers.writer.Write(r);

    if (r != NvResult::Success)
        result = MAKE_KERNEL_RESULT(NotFound); // TODO: what should this be?
}

void INvDrvServices::Initialize(REQUEST_COMMAND_PARAMS) {
    LOG_FUNC_STUBBED(HorizonServices);

    u32 transfer_mem_size = readers.reader.Read<u32>();

    // TODO: read process and transfer mem handle IDs

    writers.writer.Write(NvResult::Success);
}

void INvDrvServices::QueryEvent(REQUEST_COMMAND_PARAMS) {
    auto in = readers.reader.Read<QueryEventIn>();
    auto fd = fd_pool.GetObject(in.fd_id);

    // Dispatch
    NvResult r = NvResult::Success;
    HandleId handle_id = 0x0;
    fd->QueryEvent(in.event_id, handle_id, r);

    // Write result
    writers.writer.Write(r);
    writers.copy_handles_writer.Write(handle_id);

    if (r != NvResult::Success)
        result = MAKE_KERNEL_RESULT(NotFound); // TODO: what should this be?
}

} // namespace Hydra::Horizon::Services::NvDrv
