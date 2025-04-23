#include "core/horizon/services/sm/user_interface.hpp"

#include "core/horizon/services/account/account_service_for_application.hpp"
#include "core/horizon/services/account/account_service_for_system_service.hpp"
#include "core/horizon/services/am/apm_manager.hpp"
#include "core/horizon/services/am/application_proxy_service.hpp"
#include "core/horizon/services/aocsrv/add_on_content_manager.hpp"
#include "core/horizon/services/apm/manager_privileged.hpp"
#include "core/horizon/services/audio/audio_out_manager.hpp"
#include "core/horizon/services/audio/audio_renderer_manager.hpp"
#include "core/horizon/services/fssrv/filesystem_proxy.hpp"
#include "core/horizon/services/hid/hid_debug_server.hpp"
#include "core/horizon/services/hid/hid_server.hpp"
#include "core/horizon/services/hid/hid_system_server.hpp"
#include "core/horizon/services/lm/log_service.hpp"
#include "core/horizon/services/nifm/static_service.hpp"
#include "core/horizon/services/nvdrv/nvdrv_services.hpp"
#include "core/horizon/services/pctl/ipc/parental_control_service_factory.hpp"
#include "core/horizon/services/pcv/pcv_service.hpp"
#include "core/horizon/services/pl/sharedresource/platform_shared_resource_manager.hpp"
#include "core/horizon/services/psm/psm_server.hpp"
#include "core/horizon/services/settings/settings_server.hpp"
#include "core/horizon/services/settings/system_settings_server.hpp"
#include "core/horizon/services/socket/client.hpp"
#include "core/horizon/services/socket/resolver/resolver.hpp"
#include "core/horizon/services/spl/general_interface.hpp"
#include "core/horizon/services/spl/random_interface.hpp"
#include "core/horizon/services/ssl/sf/ssl_service.hpp"
#include "core/horizon/services/timesrv/static_service.hpp"
#include "core/horizon/services/visrv/application_root_service.hpp"
#include "core/horizon/services/visrv/manager_root_service.hpp"
#include "core/horizon/services/visrv/system_root_service.hpp"

namespace Hydra::Horizon::Services::Sm {

DEFINE_SERVICE_COMMAND_TABLE(IUserInterface, 0, RegisterProcess, 1,
                             GetServiceHandle)

void IUserInterface::GetServiceHandle(REQUEST_COMMAND_PARAMS) {
    u64 name = readers.reader.Read<u64>();

    LOG_DEBUG(HorizonServices, "Service name: \"{}\"", u64_to_str(name));

#define SERVICE_CASE_CASE(str) case str_to_u64(str):
// TODO: don't instantiate the services?
#define SERVICE_CASE(name, ...)                                                \
    FOR_EACH_0_1(SERVICE_CASE_CASE, __VA_ARGS__)                               \
    add_service(new name());                                                   \
    break;

    switch (name) {
        SERVICE_CASE(Hid::IHidServer, "hid")
        SERVICE_CASE(Hid::IHidDebugServer, "hid:dbg")
        SERVICE_CASE(Hid::IHidSystemServer, "hid:sys")
        SERVICE_CASE(Fssrv::IFileSystemProxy, "fsp-srv")
        SERVICE_CASE(TimeSrv::IStaticService, "time:u", "time:a", "time:s")
        SERVICE_CASE(NvDrv::INvDrvServices, "nvdrv")
        SERVICE_CASE(Settings::ISystemSettingsServer, "set:sys")
        SERVICE_CASE(Am::IApmManager, "apm", "apm:am")
        SERVICE_CASE(Am::IApplicationProxyService, "appletOE")
        SERVICE_CASE(ViSrv::IApplicationRootService, "vi:u")
        SERVICE_CASE(ViSrv::ISystemRootService, "vi:s")
        SERVICE_CASE(ViSrv::IManagerRootService, "vi:m")
        SERVICE_CASE(Pl::SharedResource::IPlatformSharedResourceManager, "pl:u")
        SERVICE_CASE(Psm::IPsmServer, "psm")
        SERVICE_CASE(Lm::ILogService, "lm")
        SERVICE_CASE(AocSrv::IAddOnContentManager, "aoc:u")
        SERVICE_CASE(Pctl::Ipc::IParentalControlServiceFactory, "pctl:s",
                     "pctl:r", "pctl:a", "pctl")
        SERVICE_CASE(Account::IAccountServiceForApplication, "acc:u0")
        SERVICE_CASE(Apm::IManagerPrivileged, "apm:p")
        SERVICE_CASE(Socket::IClient, "bsd:u", "bsd:s", "bsd:a")
        SERVICE_CASE(Settings::ISettingsServer, "set")
        SERVICE_CASE(Account::IAccountServiceForSystemService, "acc:u1")
        SERVICE_CASE(Audio::IAudioOutManager, "audout:u")
        SERVICE_CASE(Audio::IAudioRendererManager, "audren:u")
        SERVICE_CASE(Ssl::Sf::ISslService, "ssl")
        SERVICE_CASE(Spl::IRandomInterface, "csrng")
        SERVICE_CASE(Nifm::IStaticService, "nifm:a", "nifm:s", "nifm:u")
        SERVICE_CASE(Pcv::IPcvService, "pcv")
        SERVICE_CASE(Socket::Resolver::IResolver, "sfdnsres")
        SERVICE_CASE(Spl::IGeneralInterface, "spl:")
    default:
        LOG_WARN(HorizonServices, "Unknown service name \"{}\"",
                 u64_to_str(name));
        result = MAKE_RESULT(Svc, Kernel::Error::NotFound); // TODO: module
        break;
    }
}

} // namespace Hydra::Horizon::Services::Sm
