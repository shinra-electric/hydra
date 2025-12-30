#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::pctl {

class IParentalControlService : public IService {
    friend class IParentalControlServiceFactory;

  protected:
    result_t RequestImpl([[maybe_unused]] RequestContext& context,
                         u32 id) override;

  private:
    // Commands
    result_t Initialize();
    result_t CheckFreeCommunicationPermission();
};

} // namespace hydra::horizon::services::pctl
