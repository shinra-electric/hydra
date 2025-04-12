#include "core/horizon/services/am/application_proxy.hpp"

#include "core/horizon/services/am/application_functions.hpp"
#include "core/horizon/services/am/audio_controller.hpp"
#include "core/horizon/services/am/common_state_getter.hpp"
#include "core/horizon/services/am/debug_functions.hpp"
#include "core/horizon/services/am/display_controller.hpp"
#include "core/horizon/services/am/library_applet_creator.hpp"
#include "core/horizon/services/am/self_controller.hpp"
#include "core/horizon/services/am/window_controller.hpp"

namespace Hydra::Horizon::Services::Am {

DEFINE_SERVICE_COMMAND_TABLE(IApplicationProxy, 0, GetCommonStateGetter, 1,
                             GetSelfController, 2, GetWindowController, 3,
                             GetAudioController, 4, GetDisplayController, 11,
                             GetLibraryAppletCreator, 20,
                             GetApplicationFunctions, 1000, GetDebugFunctions)

void IApplicationProxy::GetCommonStateGetter(REQUEST_COMMAND_PARAMS) {
    add_service(new ICommonStateGetter());
}

void IApplicationProxy::GetSelfController(REQUEST_COMMAND_PARAMS) {
    add_service(new ISelfController());
}

void IApplicationProxy::GetWindowController(REQUEST_COMMAND_PARAMS) {
    add_service(new IWindowController());
}

void IApplicationProxy::GetAudioController(REQUEST_COMMAND_PARAMS) {
    add_service(new IAudioController());
}

void IApplicationProxy::GetDisplayController(REQUEST_COMMAND_PARAMS) {
    add_service(new IDisplayController());
}

void IApplicationProxy::GetLibraryAppletCreator(REQUEST_COMMAND_PARAMS) {
    add_service(new ILibraryAppletCreator());
}

void IApplicationProxy::GetApplicationFunctions(REQUEST_COMMAND_PARAMS) {
    add_service(new IApplicationFunctions());
}

void IApplicationProxy::GetDebugFunctions(REQUEST_COMMAND_PARAMS) {
    add_service(new IDebugFunctions());
}

} // namespace Hydra::Horizon::Services::Am
