#include "core/horizon/services/am/self_controller.hpp"

#include "core/horizon/os.hpp"
#include "core/hw/bus.hpp"
#include "core/hw/display/display.hpp"

namespace hydra::horizon::services::am {

DEFINE_SERVICE_COMMAND_TABLE(
    ISelfController, 0, Exit, 1, LockExit, 2, UnlockExit, 9,
    GetLibraryAppletLaunchableEvent, 10, SetScreenShotPermission, 11,
    SetOperationModeChangedNotification, 12,
    SetPerformanceModeChangedNotification, 13, SetFocusHandlingMode, 14,
    SetRestartMessageEnabled, 16, SetOutOfFocusSuspendingEnabled, 40,
    CreateManagedDisplayLayer, 44, CreateManagedDisplaySeparableLayer, 50,
    SetHandlesRequestToDisplay, 62, SetIdleTimeDetectionExtension, 80,
    SetWirelessPriorityMode, 91, GetAccumulatedSuspendedTickChangedEvent)

ISelfController::ISelfController()
    : library_applet_launchable_event(new kernel::Event(
          kernel::EventFlags::Signalled, "Library applet launchable event")),
      accumulated_suspended_tick_changed_event(
          new kernel::Event(kernel::EventFlags::AutoClear,
                            "Accumulated suspended tick changed event")) {}

result_t ISelfController::LockExit() {
    StateManager::GetInstance().LockExit();
    return RESULT_SUCCESS;
}

result_t ISelfController::UnlockExit() {
    StateManager::GetInstance().UnlockExit();
    return RESULT_SUCCESS;
}

result_t ISelfController::GetLibraryAppletLaunchableEvent(
    OutHandle<HandleAttr::Copy> out_handle) {
    out_handle = library_applet_launchable_event.id;
    return RESULT_SUCCESS;
}

result_t ISelfController::CreateManagedDisplayLayer(u64* out_layer_id) {
    u32 binder_id = OS::GetInstance().GetDisplayDriver().AddBinder();

    // TODO: what display ID should be used?
    *out_layer_id =
        KERNEL_INSTANCE.GetBus().GetDisplay(0)->CreateLayer(binder_id);
    return RESULT_SUCCESS;
}

result_t ISelfController::CreateManagedDisplaySeparableLayer(
    u64* out_display_layer_id, u64* out_recording_layer_id) {
    u32 binder_id = OS::GetInstance().GetDisplayDriver().AddBinder();

    // TODO: what display ID should be used?
    *out_display_layer_id =
        KERNEL_INSTANCE.GetBus().GetDisplay(0)->CreateLayer(binder_id);

    // TODO: what is a recording layer?
    *out_recording_layer_id =
        KERNEL_INSTANCE.GetBus().GetDisplay(0)->CreateLayer(binder_id);

    return RESULT_SUCCESS;
}

result_t
ISelfController::SetIdleTimeDetectionExtension(IdleTimeDetectionExtension ext) {
    LOG_FUNC_STUBBED(Services);

    LOG_DEBUG(Services, "Extension: {}", ext);

    return RESULT_SUCCESS;
}

result_t ISelfController::GetAccumulatedSuspendedTickChangedEvent(
    OutHandle<HandleAttr::Copy> out_handle) {
    out_handle = accumulated_suspended_tick_changed_event.id;
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::am
