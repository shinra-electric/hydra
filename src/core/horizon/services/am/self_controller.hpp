#pragma once

#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/services/const.hpp"

namespace Hydra::Horizon::Services::Am {

class ISelfController : public ServiceBase {
  public:
    ISelfController();

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    Kernel::HandleWithId<Kernel::Event> library_applet_launchable_event;

    // Commands
    result_t LockExit();
    result_t UnlockExit();
    result_t
    GetLibraryAppletLaunchableEvent(OutHandle<HandleAttr::Copy> out_handle);
    STUB_REQUEST_COMMAND(SetScreenShotPermission);
    STUB_REQUEST_COMMAND(SetOperationModeChangedNotification);
    STUB_REQUEST_COMMAND(SetPerformanceModeChangedNotification);
    STUB_REQUEST_COMMAND(SetFocusHandlingMode);
    STUB_REQUEST_COMMAND(SetOutOfFocusSuspendingEnabled);
    result_t CreateManagedDisplayLayer(u64* out_layer_id);
};

} // namespace Hydra::Horizon::Services::Am
