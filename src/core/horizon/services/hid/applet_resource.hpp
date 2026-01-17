#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::hid {

namespace internal {
class AppletResource;
}

class IAppletResource : public IService {
  public:
    IAppletResource(kernel::AppletResourceUserId aruid_);
    ~IAppletResource();

  protected:
    result_t RequestImpl([[maybe_unused]] RequestContext& context,
                         u32 id) override;

  private:
    kernel::AppletResourceUserId aruid;
    internal::AppletResource& resource;

    // Commands
    result_t GetSharedMemoryHandle(kernel::Process* process,
                                   OutHandle<HandleAttr::Copy> out_handle);
};

} // namespace hydra::horizon::services::hid
