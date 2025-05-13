#include "core/horizon/services/nvdrv/nvdrv_services.hpp"

#include "core/horizon/const.hpp"
#include "core/horizon/services/nvdrv/const.hpp"
#include "core/horizon/services/nvdrv/ioctl/nvhost_as_gpu.hpp"
#include "core/horizon/services/nvdrv/ioctl/nvhost_ctrl.hpp"
#include "core/horizon/services/nvdrv/ioctl/nvhost_ctrl_gpu.hpp"
#include "core/horizon/services/nvdrv/ioctl/nvhost_gpu.hpp"
#include "core/horizon/services/nvdrv/ioctl/nvmap.hpp"

namespace hydra::horizon::services::nvdrv {

allocators::StaticPool<ioctl::FdBase*, MAX_FD_COUNT> INvDrvServices::fd_pool;

DEFINE_SERVICE_COMMAND_TABLE(INvDrvServices, 0, Open, 1, Ioctl, 2, Close, 3,
                             Initialize, 4, QueryEvent, 8, SetAruid, 13,
                             SetGraphicsFirmwareMemoryMarginEnabled)

result_t INvDrvServices::Open(InBuffer<BufferAttr::MapAlias> path_buffer,
                              u32* out_fd_id, u32* out_error) {
    auto path = path_buffer.reader->ReadString();
    handle_id_t fd_id = fd_pool.AllocateForIndex();
    if (path == "/dev/nvhost-ctrl") {
        fd_pool.GetRef(fd_id) = new ioctl::NvHostCtrl();
    } else if (path == "/dev/nvmap") {
        fd_pool.GetRef(fd_id) = new ioctl::NvMap();
    } else if (path == "/dev/nvhost-as-gpu") {
        fd_pool.GetRef(fd_id) = new ioctl::NvHostAsGpu();
    } else if (path == "/dev/nvhost-ctrl-gpu") {
        fd_pool.GetRef(fd_id) = new ioctl::NvHostCtrlGpu();
    } else if (path == "/dev/nvhost-gpu") {
        fd_pool.GetRef(fd_id) = new ioctl::NvHostGpu();
    } else {
        LOG_WARN(Services, "Unknown path \"{}\"", path);
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
    auto fd = fd_pool.Get(fd_id);

    // Dispatch
    u32 type = (code >> 8) & 0xff;
    u32 nr = code & 0xff;

    ioctl::IoctlContext context{
        .reader = in_buffer.reader,
        .writer = out_buffer.writer,
    };
    NvResult result = fd->Ioctl(context, type, nr);

    // Write result
    *out_result = result;

    if (result != NvResult::Success)
        return MAKE_RESULT(
            Svc,
            kernel::Error::NotFound); // TODO: what should this be?
    else
        return RESULT_SUCCESS;
}

result_t INvDrvServices::Close(u32 fd_id, u32* out_err) {
    // TODO: check if exists
    auto fd = fd_pool.Get(fd_id);
    delete fd;
    fd_pool.Free(fd_id);

    *out_err = 0;
    return RESULT_SUCCESS;
}

result_t INvDrvServices::Initialize(u32 transfer_mem_size,
                                    NvResult* out_result) {
    LOG_FUNC_STUBBED(Services);

    // TODO: read process and transfer mem handle IDs

    *out_result = NvResult::Success;
    return RESULT_SUCCESS;
}

result_t INvDrvServices::QueryEvent(handle_id_t fd_id, u32 event_id,
                                    NvResult* out_result,
                                    OutHandle<HandleAttr::Copy> out_handle) {
    auto fd = fd_pool.Get(fd_id);

    // Dispatch
    handle_id_t handle_id = INVALID_HANDLE_ID;
    NvResult result = fd->QueryEvent(event_id, handle_id);

    // Write result
    *out_result = result;
    out_handle = handle_id;

    if (result != NvResult::Success)
        return MAKE_RESULT(
            Svc,
            kernel::Error::NotFound); // TODO: what should this be?
    else
        return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::nvdrv
