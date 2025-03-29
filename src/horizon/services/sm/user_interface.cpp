#include "horizon/services/sm/user_interface.hpp"

#include "horizon/services/account/account_service_for_application.hpp"
#include "horizon/services/account/account_service_for_system_service.hpp"
#include "horizon/services/am/apm_manager.hpp"
#include "horizon/services/am/application_proxy_service.hpp"
#include "horizon/services/aocsrv/add_on_content_manager.hpp"
#include "horizon/services/apm/manager_privileged.hpp"
#include "horizon/services/audio/audio_out_manager.hpp"
#include "horizon/services/fssrv/filesystem_proxy.hpp"
#include "horizon/services/hid/hid_server.hpp"
#include "horizon/services/lm/log_service.hpp"
#include "horizon/services/nvdrv/nvdrv_services.hpp"
#include "horizon/services/pctl/ipc/parental_control_service_factory.hpp"
#include "horizon/services/pl/sharedresource/platform_shared_resource_manager.hpp"
#include "horizon/services/psm/psm_server.hpp"
#include "horizon/services/settings/settings_server.hpp"
#include "horizon/services/settings/system_settings_server.hpp"
#include "horizon/services/socket/client.hpp"
#include "horizon/services/timesrv/static_service.hpp"
#include "horizon/services/visrv/manager_root_service.hpp"

namespace Hydra::Horizon::Services::Sm {

DEFINE_SERVICE_COMMAND_TABLE(IUserInterface, 1, GetServiceHandle)

void IUserInterface::GetServiceHandle(REQUEST_COMMAND_PARAMS) {
    // auto in = *reinterpret_cast<GetServiceHandleIn*>(in_ptr);
    // std::string name(in.name);
    u64 name_u64 = readers.reader.Read<u64>();
    const char* name_c = (const char*)(&name_u64);
    std::string name(name_c, std::min(strlen(name_c), (usize)8));

    if (name == "") {
        result = MAKE_KERNEL_RESULT(NotFound);
        return;
    }

    if (name == "hid") {
        add_service(new Hid::IHidServer());
    } else if (name == "fsp-srv") {
        add_service(new Fssrv::IFileSystemProxy());
    } else if (name == "time:u" || name == "time:a" || name == "time:s") {
        add_service(new TimeSrv::IStaticService());
    } else if (name == "nvdrv") {
        add_service(new NvDrv::INvDrvServices());
    } else if (name == "set:sys") {
        add_service(new Settings::ISystemSettingsServer());
    } else if (name == "apm" || name == "apm:am") {
        add_service(new Am::IApmManager());
    } else if (name == "appletOE") {
        add_service(new Am::IApplicationProxyService());
    } else if (name == "vi:m") {
        add_service(new ViSrv::IManagerRootService());
    } else if (name == "pl:u") {
        add_service(new Pl::SharedResource::IPlatformSharedResourceManager());
    } else if (name == "psm") {
        add_service(new Psm::IPsmServer());
    } else if (name == "lm") {
        add_service(new Lm::ILogService());
    } else if (name == "aoc:u") {
        add_service(new AocSrv::IAddOnContentManager());
    } else if (name == "pctl:s" || name == "pctl:r" || name == "pctl:a" ||
               name == "pctl") {
        add_service(new Pctl::Ipc::IParentalControlServiceFactory());
    } else if (name == "acc:u0") {
        add_service(new Account::IAccountServiceForApplication());
    } else if (name == "apm:p") {
        add_service(new Apm::IManagerPrivileged());
    } else if (name == "bsd:u" || name == "bsd:s" || name == "bsd:a") {
        add_service(new Socket::IClient());
    } else if (name == "set") {
        add_service(new Settings::ISettingsServer());
    } else if (name == "acc:u1") {
        add_service(new Account::IAccountServiceForSystemService());
    } else if (name == "audout:u") {
        add_service(new Audio::IAudioOutManager());
    } else {
        LOG_WARNING(HorizonServices, "Unknown service \"{}\"", name);
        result = MAKE_KERNEL_RESULT(NotFound);
        // TODO: don't throw
        throw;
    }
}

} // namespace Hydra::Horizon::Services::Sm
