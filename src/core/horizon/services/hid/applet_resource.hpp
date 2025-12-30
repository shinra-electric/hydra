#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::hid {

class IAppletResource : public IService {
  protected:
    result_t RequestImpl([[maybe_unused]] RequestContext& context,
                         u32 id) override;

  private:
    // Commands
    result_t GetSharedMemoryHandle(kernel::Process* process,
                                   OutHandle<HandleAttr::Copy> out_handle);
};

} // namespace hydra::horizon::services::hid
