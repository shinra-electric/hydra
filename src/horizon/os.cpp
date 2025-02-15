#include "horizon/os.hpp"

#include "horizon/services/am/apm_manager.hpp"
#include "horizon/services/am/application_proxy.hpp"
#include "horizon/services/am/application_proxy_service.hpp"
#include "horizon/services/am/common_state_getter.hpp"
#include "horizon/services/fssrv/filesystem_proxy.hpp"
#include "horizon/services/hid/hid_server.hpp"
#include "horizon/services/hosbinder/hos_binder_driver.hpp"
#include "horizon/services/nvdrv/nvdrv_services.hpp"
#include "horizon/services/settings/system_settings_server.hpp"
#include "horizon/services/sm/user_interface.hpp"
#include "horizon/services/timesrv/static_service.hpp"
#include "horizon/services/timesrv/steady_clock.hpp"
#include "horizon/services/timesrv/system_clock.hpp"
#include "horizon/services/timesrv/time_zone_service.hpp"
#include "horizon/services/visrv/application_display_service.hpp"
#include "horizon/services/visrv/manager_display_service.hpp"
#include "horizon/services/visrv/manager_root_service.hpp"
#include "horizon/services/visrv/system_display_service.hpp"

namespace Hydra::Horizon {

static OS* s_instance = nullptr;

OS& OS::GetInstance() { return *s_instance; }

OS::OS(HW::Bus& bus) : kernel(bus) {
    ASSERT(s_instance == nullptr, Horizon, "Horizon OS already exists");
    s_instance = this;

    // Services

    // am
    am = {
        .apm_manager = new Services::Am::IApmManager(),
        .application_proxy = new Services::Am::IApplicationProxy(),
        .application_proxy_service =
            new Services::Am::IApplicationProxyService(),
        .common_state_getter = new Services::Am::ICommonStateGetter(),
    };

    // fssrv
    fs_srv = {
        .filesystem_proxy = new Services::FsSrv::IFileSystemProxy(),
    };

    // hid
    hid = {
        .hid_server = new Services::Hid::IHidServer(),
    };

    // hosbinder
    hos_binder = {
        .hos_binder_driver = new Services::HosBinder::IHOSBinderDriver(),
    };

    // nvdrv
    nv_drv = {
        .nvdrv_services = new Services::NvDrv::INvDrvServices(),
    };

    // settings
    settings = {
        .system_settings_server =
            new Services::Settings::ISystemSettingsServer(),
    };

    // sm
    sm = {
        .user_interface = new Services::Sm::IUserInterface(),
    };

    // timesrv
    time_srv = {
        .static_service = new Services::TimeSrv::IStaticService(),
        .steady_clock = new Services::TimeSrv::ISteadyClock(),
        .standard_user_system_clock = new Services::TimeSrv::ISystemClock(
            Services::TimeSrv::SystemClockType::StandardUser),
        .standard_network_system_clock = new Services::TimeSrv::ISystemClock(
            Services::TimeSrv::SystemClockType::StandardNetwork),
        .standard_local_system_clock = new Services::TimeSrv::ISystemClock(
            Services::TimeSrv::SystemClockType::StandardLocal),
        .ephemeral_network_system_clock = new Services::TimeSrv::ISystemClock(
            Services::TimeSrv::SystemClockType::EphemeralNetwork),
        .time_zone_service = new Services::TimeSrv::ITimeZoneService(),
    };

    // visrv
    vi_srv = {
        .application_display_service =
            new Services::ViSrv::IApplicationDisplayService(),
        .manager_display_service =
            new Services::ViSrv::IManagerDisplayService(),
        .manager_root_service = new Services::ViSrv::IManagerRootService(),
        .system_display_service = new Services::ViSrv::ISystemDisplayService(),
    };

    kernel.ConnectServiceToPort("sm:", sm.user_interface);
}

OS::~OS() {}

void OS::SetMMU(HW::MMU::MMUBase* mmu) { kernel.SetMMU(mmu); }

} // namespace Hydra::Horizon
