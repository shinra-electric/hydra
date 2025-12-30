#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::fatalsrv {

class IService : public ::hydra::horizon::services::IService {
  protected:
    result_t RequestImpl([[maybe_unused]] RequestContext& context,
                         u32 id) override;

  private:
    // Commands
    STUB_REQUEST_COMMAND(ThrowFatalWithPolicy);
    // TODO: PID descriptor
    result_t ThrowFatalWithCpuContext(u64 code, u64 _unknown_x8,
                                      InBuffer<BufferAttr::MapAlias> in_buffer);
};

} // namespace hydra::horizon::services::fatalsrv
