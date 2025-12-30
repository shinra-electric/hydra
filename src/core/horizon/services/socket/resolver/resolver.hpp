#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::socket::Resolver {

class IResolver : public IService {
  protected:
    result_t RequestImpl([[maybe_unused]] RequestContext& context,
                         u32 id) override;

  private:
    // Commands
    result_t GetAddrInfoRequest();
};

} // namespace hydra::horizon::services::socket::Resolver
