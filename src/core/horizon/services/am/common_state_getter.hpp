#pragma once

#include "core/horizon/services/const.hpp"
#include "core/horizon/services/oe/const.hpp"
#include "core/horizon/state_manager.hpp"

namespace hydra::horizon::services::am {

enum class OperationMode {
    Handheld,
    Console,
};

class ICommonStateGetter : public ServiceBase {
  public:
    ICommonStateGetter()
        : default_display_resolution_change_event(
              new kernel::Event(kernel::EventFlags::AutoClear,
                                "Default display resolution change event")) {}

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    kernel::HandleWithId<kernel::Event> default_display_resolution_change_event;

    // Commands
    result_t GetEventHandle(OutHandle<HandleAttr::Copy> out_handle);
    result_t ReceiveMessage(AppletMessage* out_message);
    STUB_REQUEST_COMMAND(DisallowToEnterSleep);
    result_t GetOperationMode(OperationMode* out_mode);
    STUB_REQUEST_COMMAND(GetPerformanceMode);
    result_t GetDefaultDisplayResolution(i32* out_width, i32* out_height);
    result_t GetDefaultDisplayResolutionChangeEvent(
        OutHandle<HandleAttr::Copy> out_handle);
    result_t GetCurrentFocusState(AppletFocusState* out_state);
    result_t SetCpuBoostMode(oe::CpuBoostMode mode);
    STUB_REQUEST_COMMAND(
        SetRequestExitToLibraryAppletAtExecuteNextProgramEnabled);
};

} // namespace hydra::horizon::services::am
