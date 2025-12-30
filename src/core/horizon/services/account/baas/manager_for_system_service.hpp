#pragma once

#include "core/horizon/services/account/const.hpp"

namespace hydra::horizon::services::account::baas {

class IManagerForSystemService : public IService {
  protected:
    result_t RequestImpl([[maybe_unused]] RequestContext& context,
                         u32 id) override;

  private:
    // Commands
    STUB_REQUEST_COMMAND(CheckAvailability);
    // TODO: correct?
    result_t GetAccountId(uuid_t* out_id);
};

} // namespace hydra::horizon::services::account::baas
