#include "core/horizon/services/am/library_applet_proxy.hpp"

#include "core/horizon/services/am/audio_controller.hpp"
#include "core/horizon/services/am/common_state_getter.hpp"
#include "core/horizon/services/am/debug_functions.hpp"
#include "core/horizon/services/am/display_controller.hpp"
#include "core/horizon/services/am/library_applet_creator.hpp"
#include "core/horizon/services/am/library_applet_self_accessor.hpp"
#include "core/horizon/services/am/process_winding_controller.hpp"
#include "core/horizon/services/am/self_controller.hpp"
#include "core/horizon/services/am/window_controller.hpp"

namespace hydra::horizon::services::am {

DEFINE_SERVICE_COMMAND_TABLE(ILibraryAppletProxy, 0, GetCommonStateGetter, 1,
                             GetSelfController, 2, GetWindowController, 3,
                             GetAudioController, 4, GetDisplayController, 10,
                             GetProcessWindingController, 11,
                             GetLibraryAppletCreator, 20,
                             OpenLibraryAppletSelfAccessor, 1000,
                             GetDebugFunctions)

result_t
ILibraryAppletProxy::GetCommonStateGetter(add_service_fn_t add_service) {
    add_service(new ICommonStateGetter());
    return RESULT_SUCCESS;
}

result_t ILibraryAppletProxy::GetSelfController(add_service_fn_t add_service) {
    add_service(new ISelfController());
    return RESULT_SUCCESS;
}

result_t
ILibraryAppletProxy::GetWindowController(add_service_fn_t add_service) {
    add_service(new IWindowController());
    return RESULT_SUCCESS;
}

result_t ILibraryAppletProxy::GetAudioController(add_service_fn_t add_service) {
    add_service(new IAudioController());
    return RESULT_SUCCESS;
}

result_t
ILibraryAppletProxy::GetDisplayController(add_service_fn_t add_service) {
    add_service(new IDisplayController());
    return RESULT_SUCCESS;
}

result_t
ILibraryAppletProxy::GetProcessWindingController(add_service_fn_t add_service) {
    add_service(new IProcessWindingController());
    return RESULT_SUCCESS;
}

result_t
ILibraryAppletProxy::GetLibraryAppletCreator(add_service_fn_t add_service) {
    add_service(new ILibraryAppletCreator());
    return RESULT_SUCCESS;
}

result_t ILibraryAppletProxy::OpenLibraryAppletSelfAccessor(
    add_service_fn_t add_service) {
    add_service(new ILibraryAppletSelfAccessor());
    return RESULT_SUCCESS;
}

result_t ILibraryAppletProxy::GetDebugFunctions(add_service_fn_t add_service) {
    add_service(new IDebugFunctions());
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::am
