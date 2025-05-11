#include "core/horizon/services/nvdrv/nvdrv_services.hpp"

#include "core/horizon/const.hpp"
#include "core/horizon/services/nvdrv/const.hpp"
#include "core/horizon/services/nvdrv/ioctl/nvhost_as_gpu.hpp"
#include "core/horizon/services/nvdrv/ioctl/nvhost_ctrl.hpp"
#include "core/horizon/services/nvdrv/ioctl/nvhost_ctrl_gpu.hpp"
#include "core/horizon/services/nvdrv/ioctl/nvhost_gpu.hpp"
#include "core/horizon/services/nvdrv/ioctl/nvmap.hpp"

namespace Hydra::Horizon::Services::NvDrv {

Allocators::StaticPool<Ioctl::FdBase*, MAX_FD_COUNT> INvDrvServices::fd_pool;

DEFINE_SERVICE_COMMAND_TABLE(INvDrvServices, 0, Open, 1, Ioctl, 3, Initialize,
                             4, QueryEvent, 8, SetAruid, 13,
                             SetGraphicsFirmwareMemoryMarginEnabled)

result_t INvDrvServices::Open(InBuffer<BufferAttr::MapAlias> path_buffer,
                              u32* out_fd_id, u32* out_error) {
    auto path = path_buffer.reader->ReadString();
    handle_id_t fd_id = fd_pool.AllocateForIndex();
    if (path == "/dev/nvhost-ctrl") {
        fd_pool.GetObjectRef(fd_id) = new Ioctl::NvHostCtrl();
    } else if (path == "/dev/nvmap") {
        fd_pool.GetObjectRef(fd_id) = new Ioctl::NvMap();
    } else if (path == "/dev/nvhost-as-gpu") {
        fd_pool.GetObjectRef(fd_id) = new Ioctl::NvHostAsGpu();
    } else if (path == "/dev/nvhost-ctrl-gpu") {
        fd_pool.GetObjectRef(fd_id) = new Ioctl::NvHostCtrlGpu();
    } else if (path == "/dev/nvhost-gpu") {
        fd_pool.GetObjectRef(fd_id) = new Ioctl::NvHostGpu();
    } else {
        LOG_WARN(HorizonServices, "Unknown path \"{}\"", path);
        *out_error = MAKE_RESULT(Svc, 0); // TODO
        return MAKE_RESULT(Svc, 0);       // TODO
    }

    *out_fd_id = fd_id;
    *out_error = 0;
    return RESULT_SUCCESS;
}

result_t INvDrvServices::Ioctl(handle_id_t fd_id, u32 code,
                               InBuffer<BufferAttr::AutoSelect> in_buffer,
                               NvResult* out_result,
                               OutBuffer<BufferAttr::AutoSelect> out_buffer) {
    auto fd = fd_pool.GetObject(fd_id);

    // Dispatch
    u32 type = (code >> 8) & 0xff;
    u32 nr = code & 0xff;
    NvResult r = NvResult::Success;
    fd->Ioctl(in_buffer.reader, out_buffer.writer, type, nr, r);

    // Write result
    *out_result = r;

    if (r != NvResult::Success)
        return MAKE_RESULT(
            Svc,
            Kernel::Error::NotFound); // TODO: what should this be?
    else
        return RESULT_SUCCESS;
}

result_t INvDrvServices::Initialize(u32 transfer_mem_size,
                                    NvResult* out_result) {
    LOG_FUNC_STUBBED(HorizonServices);

    // TODO: read process and transfer mem handle IDs

    *out_result = NvResult::Success;
    return RESULT_SUCCESS;
}

result_t INvDrvServices::QueryEvent(handle_id_t fd_id, u32 event_id,
                                    NvResult* out_result,
                                    OutHandle<HandleAttr::Copy> out_handle) {
    auto fd = fd_pool.GetObject(fd_id);

    // Dispatch
    NvResult r = NvResult::Success;
    handle_id_t handle_id = 0x0;
    fd->QueryEvent(event_id, handle_id, r);

    // Write result
    *out_result = r;
    out_handle = handle_id;

    if (r != NvResult::Success)
        return MAKE_RESULT(
            Svc,
            Kernel::Error::NotFound); // TODO: what should this be?
    else
        return RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::NvDrv
