#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::sm {

class IUserInterface : public IService {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    STUB_REQUEST_COMMAND(RegisterClient);
    result_t GetServiceHandle(RequestContext* ctx, u64 name);
};

} // namespace hydra::horizon::services::sm
