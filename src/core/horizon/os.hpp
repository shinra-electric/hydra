#pragma once

#include "core/horizon/display/driver.hpp"
#include "core/horizon/input_manager.hpp"
#include "core/horizon/services/account/user_manager.hpp"
#include "core/horizon/services/pl/internal/shared_font_manager.hpp"
#include "core/horizon/services/server.hpp"
#include "core/horizon/services/timesrv/internal/time_manager.hpp"

#define OS_INSTANCE horizon::OS::GetInstance()
#define INPUT_MANAGER_INSTANCE OS_INSTANCE.GetInputManager()
#define USER_MANAGER_INSTANCE OS_INSTANCE.GetUserManager()
#define SHARED_FONT_MANAGER_INSTANCE OS_INSTANCE.GetSharedFontManager()
#define TIME_MANAGER_INSTANCE OS_INSTANCE.GetTimeManager()

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

    void SetSurfaceResolution(uint2 resolution);
    uint2 GetDisplayResolution() const;

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
    services::pl::internal::SharedFontManager shared_font_manager;
    services::timesrv::internal::TimeManager time_manager;

    services::am::LibraryAppletController* library_applet_self_controller{
        nullptr};

    // Display
    uint2 surface_resolution;

  public:
    REF_GETTER(audio_core, GetAudioCore);
    REF_GETTER(ui_handler, GetUIHandler);
    REF_GETTER(kernel, GetKernel);
    REF_GETTER(service_manager, GetServiceManager);
    REF_GETTER(display_driver, GetDisplayDriver);
    REF_GETTER(input_manager, GetInputManager);
    REF_GETTER(user_manager, GetUserManager);
    REF_GETTER(shared_font_manager, GetSharedFontManager);
    REF_GETTER(time_manager, GetTimeManager);
    GETTER_AND_SETTER(library_applet_self_controller,
                      GetLibraryAppletSelfController,
                      SetLibraryAppletSelfController);
};

} // namespace hydra::horizon
