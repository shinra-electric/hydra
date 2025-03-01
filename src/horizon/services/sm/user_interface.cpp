#include "horizon/services/sm/user_interface.hpp"

#include "horizon/const.hpp"
#include "horizon/kernel.hpp"
#include "horizon/services/am/apm_manager.hpp"
#include "horizon/services/am/application_proxy_service.hpp"
#include "horizon/services/fssrv/filesystem_proxy.hpp"
#include "horizon/services/hid/hid_server.hpp"
#include "horizon/services/nvdrv/nvdrv_services.hpp"
#include "horizon/services/pl/sharedresource/platform_shared_resource_manager.hpp"
#include "horizon/services/settings/system_settings_server.hpp"
#include "horizon/services/timesrv/static_service.hpp"
#include "horizon/services/visrv/manager_root_service.hpp"

namespace Hydra::Horizon::Services::Sm {

enum class Service : u64 {
    Hid = 0x0000000000646968,
    FspSrv = 0x007672732d707366,
    TimeU = 0x0000753a656d6974,
    Nvdrv = 0x000000767264766e,
    SetSys = 0x007379733a746573,
    Apm = 0x00000000006d7061,
    AppletOE = 0x454f74656c707061,
    ViM = 0x000000006d3a6976,
};

DEFINE_SERVICE_COMMAND_TABLE(IUserInterface, 1, GetServiceHandle)

void IUserInterface::GetServiceHandle(REQUEST_COMMAND_PARAMS) {
    // auto in = *reinterpret_cast<GetServiceHandleIn*>(in_ptr);
    // std::string name(in.name);
    u64 name_u64 = readers.reader.Read<u64>();
    const char* name_c = (const char*)(&name_u64);
    std::string name(name_c, std::min(strlen(name_c), (usize)8));

    if (name == "hid") {
        add_service(new Hid::IHidServer());
    } else if (name == "fsp-srv") {
        add_service(new Fssrv::IFileSystemProxy());
    } else if (name == "time:u" || name == "time:a" || name == "time:r") {
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
    } else {
        LOG_WARNING(HorizonServices, "Unknown service \"{}\"", name);
        result = MAKE_KERNEL_RESULT(NotFound);
        // TODO: don't throw
        throw;
    }
}

} // namespace Hydra::Horizon::Services::Sm
