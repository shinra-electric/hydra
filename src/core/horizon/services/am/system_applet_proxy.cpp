#include "core/horizon/services/am/system_applet_proxy.hpp"

#include "core/horizon/services/am/application_creator.hpp"
#include "core/horizon/services/am/audio_controller.hpp"
#include "core/horizon/services/am/common_state_getter.hpp"
#include "core/horizon/services/am/debug_functions.hpp"
#include "core/horizon/services/am/display_controller.hpp"
#include "core/horizon/services/am/global_state_controller.hpp"
#include "core/horizon/services/am/home_menu_functions.hpp"
#include "core/horizon/services/am/library_applet_creator.hpp"
#include "core/horizon/services/am/process_winding_controller.hpp"
#include "core/horizon/services/am/self_controller.hpp"
#include "core/horizon/services/am/window_controller.hpp"

namespace hydra::horizon::services::am {

DEFINE_SERVICE_COMMAND_TABLE(ISystemAppletProxy, 0, GetCommonStateGetter, 1,
                             GetSelfController, 2, GetWindowController, 3,
                             GetAudioController, 4, GetDisplayController, 10,
                             GetProcessWindingController, 11,
                             GetLibraryAppletCreator, 20, GetHomeMenuFunctions,
                             21, GetGlobalStateController, 22,
                             GetApplicationCreator, 1000, GetDebugFunctions)

result_t
ISystemAppletProxy::GetCommonStateGetter(add_service_fn_t add_service) {
    add_service(new ICommonStateGetter());
    return RESULT_SUCCESS;
}

result_t ISystemAppletProxy::GetSelfController(add_service_fn_t add_service) {
    add_service(new ISelfController());
    return RESULT_SUCCESS;
}

result_t ISystemAppletProxy::GetWindowController(add_service_fn_t add_service) {
    add_service(new IWindowController());
    return RESULT_SUCCESS;
}

result_t ISystemAppletProxy::GetAudioController(add_service_fn_t add_service) {
    add_service(new IAudioController());
    return RESULT_SUCCESS;
}

result_t
ISystemAppletProxy::GetDisplayController(add_service_fn_t add_service) {
    add_service(new IDisplayController());
    return RESULT_SUCCESS;
}

result_t
ISystemAppletProxy::GetProcessWindingController(add_service_fn_t add_service) {
    add_service(new IProcessWindingController());
    return RESULT_SUCCESS;
}

result_t
ISystemAppletProxy::GetLibraryAppletCreator(add_service_fn_t add_service) {
    add_service(new ILibraryAppletCreator());
    return RESULT_SUCCESS;
}

result_t
ISystemAppletProxy::GetHomeMenuFunctions(add_service_fn_t add_service) {
    add_service(new IHomeMenuFunctions());
    return RESULT_SUCCESS;
}

result_t
ISystemAppletProxy::GetGlobalStateController(add_service_fn_t add_service) {
    add_service(new IGlobalStateController());
    return RESULT_SUCCESS;
}

result_t
ISystemAppletProxy::GetApplicationCreator(add_service_fn_t add_service) {
    add_service(new IApplicationCreator());
    return RESULT_SUCCESS;
}

result_t ISystemAppletProxy::GetDebugFunctions(add_service_fn_t add_service) {
    add_service(new IDebugFunctions());
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::am
