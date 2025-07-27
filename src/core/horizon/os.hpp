#pragma once

#include "core/horizon/display/driver.hpp"
#include "core/horizon/input_manager.hpp"
#include "core/horizon/services/account/user_manager.hpp"
#include "core/horizon/services/server.hpp"

#define OS_INSTANCE horizon::OS::GetInstance()
#define INPUT_MANAGER_INSTANCE OS_INSTANCE.GetInputManager()

namespace hydra::audio {
class ICore;
}

namespace hydra::horizon {

namespace services::am {
class LibraryAppletController;
}

namespace ui {
class HandlerBase;
}

class OS {
  public:
    static OS& GetInstance();

    OS(audio::ICore& audio_core_, ui::HandlerBase& ui_handler_);
    ~OS();

    bool IsInHandheldMode() const {
        // TODO: make this configurable
        return true;
    }

  private:
    audio::ICore& audio_core;
    ui::HandlerBase& ui_handler;

    kernel::Kernel kernel;

    // Services
    kernel::hipc::ServiceManager<u64> service_manager;

    services::Server sm_server;

    // services::Server account_server;
    services::Server hid_server;
    services::Server am_server;
    services::Server ns_server;
    services::Server ppc_server;
    services::Server glue_server;
    services::Server audio_server;
    services::Server bcat_server;
    services::Server pcv_server;
    services::Server socket_server;
    services::Server capsrv_server;
    services::Server vi_server;
    services::Server nvnflinger_server;
    // services::Server error_server;
    services::Server ptm_server;
    services::Server fatal_server;
    services::Server friends_server;
    services::Server fs_server;
    services::Server nfc_server;
    services::Server nifm_server;
    services::Server nvservices_server;
    services::Server pctl_server;
    services::Server settings_server;
    services::Server ssl_server;
    services::Server spl_server;
    services::Server unknown_server;

    // Managers
    display::Driver display_driver;
    InputManager input_manager;
    services::account::UserManager user_manager;

    services::am::LibraryAppletController* library_applet_self_controller{
        nullptr};

  public:
    REF_GETTER(audio_core, GetAudioCore);
    REF_GETTER(ui_handler, GetUIHandler);
    REF_GETTER(kernel, GetKernel);
    REF_GETTER(service_manager, GetServiceManager);
    REF_GETTER(display_driver, GetDisplayDriver);
    REF_GETTER(input_manager, GetInputManager);
    GETTER_AND_SETTER(library_applet_self_controller,
                      GetLibraryAppletSelfController,
                      SetLibraryAppletSelfController);
};

} // namespace hydra::horizon
