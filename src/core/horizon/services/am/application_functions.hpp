#pragma once

#include "core/horizon/const.hpp"
#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::am {

class IApplicationFunctions : public ServiceBase {
  public:
    IApplicationFunctions() : gpu_error_detect_event(new kernel::Event()) {}

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    kernel::HandleWithId<kernel::Event> gpu_error_detect_event;

    // Commands
    result_t PopLaunchParameter(add_service_fn_t add_service,
                                LaunchParameterKind kind);
    result_t EnsureSaveData(uuid_t user_id, u64* out_unknown);
    result_t GetDesiredLanguage(LanguageCode* out_language_code);
    result_t SetTerminateResult(result_t result);
    STUB_REQUEST_COMMAND(NotifyRunning);
    STUB_REQUEST_COMMAND(InitializeGamePlayRecording);
    STUB_REQUEST_COMMAND(SetGamePlayRecordingState);
    result_t
    GetGpuErrorDetectedSystemEvent(OutHandle<HandleAttr::Copy> out_handle);
};

} // namespace hydra::horizon::services::am
