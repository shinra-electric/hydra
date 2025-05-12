#pragma once

#include "core/horizon/const.hpp"
#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::am {

class IApplicationFunctions : public ServiceBase {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t PopLaunchParameter(add_service_fn_t add_service,
                                LaunchParameterKind kind);
    result_t EnsureSaveData(u128 user_id, u64* out_unknown);
    result_t GetDesiredLanguage(LanguageCode* out_language_code);
    result_t SetTerminateResult(result_t result);
    STUB_REQUEST_COMMAND(NotifyRunning);
};

} // namespace hydra::horizon::services::am
