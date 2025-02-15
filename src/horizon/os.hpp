#pragma once

#include "horizon/kernel.hpp"

#define GET_SERVICE(category, service)                                         \
    OS::GetInstance().Get##category##Services().service
// Explicitly cast to ServiceBase
#define GET_SERVICE_EXPLICIT(category, service)                                \
    reinterpret_cast<ServiceBase*>(GET_SERVICE(category, service))

namespace Hydra::HW::MMU {
class MMUBase;
}

namespace Hydra::HW::Display {
class DisplayBase;
}

namespace Hydra::Horizon {

namespace Services::Am {
class IApmManager;
class IApplicationProxy;
class IApplicationProxyService;
class ICommonStateGetter;
} // namespace Services::Am

namespace Services::FsSrv {
class IFileSystemProxy;
}

namespace Services::Hid {
class IHidServer;
}

namespace Services::HosBinder {
class IHOSBinderDriver;
}

namespace Services::NvDrv {
class INvDrvServices;
}

namespace Services::Settings {
class ISystemSettingsServer;
}

namespace Services::Sm {
class IUserInterface;
}

namespace Services::TimeSrv {
class IStaticService;
class ISteadyClock;
class ISystemClock;
class ITimeZoneService;
} // namespace Services::TimeSrv

namespace Services::ViSrv {
class IApplicationDisplayService;
class IManagerDisplayService;
class IManagerRootService;
class ISystemDisplayService;
} // namespace Services::ViSrv

class OS {
  public:
    static OS& GetInstance();

    OS(HW::Bus& bus);
    ~OS();

    void SetMMU(HW::MMU::MMUBase* mmu);

    // Getters
    Kernel& GetKernel() { return kernel; }

    auto GetAmServices() const { return am; }
    auto GetFsSrvServices() const { return fs_srv; }
    auto GetHidServices() const { return hid; }
    auto GetHosBinderServices() const { return hos_binder; }
    auto GetNvDrvServices() const { return nv_drv; }
    auto GetSettingsServices() const { return settings; }
    auto GetSmServices() const { return sm; }
    auto GetTimeSrvServices() const { return time_srv; }
    auto GetViSrvServices() const { return vi_srv; }

  private:
    Kernel kernel;

    // Services

    // am
    struct {
        Services::Am::IApmManager* apm_manager;
        Services::Am::IApplicationProxy* application_proxy;
        Services::Am::IApplicationProxyService* application_proxy_service;
        Services::Am::ICommonStateGetter* common_state_getter;
    } am;

    // fssrv
    struct {
        Services::FsSrv::IFileSystemProxy* filesystem_proxy;
    } fs_srv;

    // hid
    struct {
        Services::Hid::IHidServer* hid_server;
    } hid;

    // hosbinder
    struct {
        Services::HosBinder::IHOSBinderDriver* hos_binder_driver;
    } hos_binder;

    // nvdrv
    struct {
        Services::NvDrv::INvDrvServices* nvdrv_services;
    } nv_drv;

    // settings
    struct {
        Services::Settings::ISystemSettingsServer* system_settings_server;
    } settings;

    // sm
    struct {
        Services::Sm::IUserInterface* user_interface;
    } sm;

    // timesrv
    struct {
        Services::TimeSrv::IStaticService* static_service;
        Services::TimeSrv::ISteadyClock* steady_clock;
        Services::TimeSrv::ISystemClock* standard_user_system_clock;
        Services::TimeSrv::ISystemClock* standard_network_system_clock;
        Services::TimeSrv::ISystemClock* standard_local_system_clock;
        Services::TimeSrv::ISystemClock* ephemeral_network_system_clock;
        Services::TimeSrv::ITimeZoneService* time_zone_service;
    } time_srv;

    // visrv
    struct {
        Services::ViSrv::IApplicationDisplayService*
            application_display_service;
        Services::ViSrv::IManagerDisplayService* manager_display_service;
        Services::ViSrv::IManagerRootService* manager_root_service;
        Services::ViSrv::ISystemDisplayService* system_display_service;
    } vi_srv;
};

} // namespace Hydra::Horizon
