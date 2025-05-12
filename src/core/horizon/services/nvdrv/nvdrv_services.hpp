#pragma once

#include "common/allocators/static_pool.hpp"
#include "core/horizon/services/const.hpp"
#include "core/horizon/services/nvdrv/const.hpp"

namespace Hydra::Horizon::Services::NvDrv {

namespace Ioctl {
class FdBase;
}

constexpr usize MAX_FD_COUNT = 256;

class INvDrvServices : public ServiceBase {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t Open(InBuffer<BufferAttr::MapAlias> path_buffer, u32* out_fd,
                  u32* out_error);
    result_t Ioctl(handle_id_t fd_id, u32 code,
                   InBuffer<BufferAttr::AutoSelect> in_buffer,
                   NvResult* out_result,
                   OutBuffer<BufferAttr::AutoSelect> out_buffer);
    result_t Initialize(u32 transfer_mem_size, NvResult* out_result);
    result_t QueryEvent(handle_id_t fd_id, u32 event_id, NvResult* out_result,
                        OutHandle<HandleAttr::Copy> out_handle);
    STUB_REQUEST_COMMAND(SetAruid);
    STUB_REQUEST_COMMAND(SetGraphicsFirmwareMemoryMarginEnabled);

    // TODO: what should be the max number of fds?
    static Allocators::StaticPool<Ioctl::FdBase*, MAX_FD_COUNT> fd_pool;
};

} // namespace Hydra::Horizon::Services::NvDrv
