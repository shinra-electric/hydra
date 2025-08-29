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

    void NotifyOperationModeChanged();

  private:
    audio::ICore& audio_core;
    ui::HandlerBase& ui_handler;

    kernel::Kernel kernel;

    // Services
    kernel::hipc::ServiceManager<u64> service_manager;
    services::Server nvservices_server;
    services::Server others_server;

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
