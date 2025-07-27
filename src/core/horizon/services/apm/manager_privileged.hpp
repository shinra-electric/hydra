#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::apm {

class IManagerPrivileged : public IService {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t OpenSession(RequestContext* ctx);
};

} // namespace hydra::horizon::services::apm
