#include "horizon/services/sm.hpp"

#include "horizon/const.hpp"
#include "horizon/kernel.hpp"
#include "horizon/services/am/apm_manager.hpp"
#include "horizon/services/am/application_proxy_service.hpp"
#include "horizon/services/fssrv/filesystem_proxy.hpp"
#include "horizon/services/hid/hid_server.hpp"
#include "horizon/services/nvdrv/nvdrv_services.hpp"
#include "horizon/services/settings/system_settings_server.hpp"
#include "horizon/services/timesrv/static_service.hpp"
#include "horizon/services/visrv/manager_root_service.hpp"

namespace Hydra::Horizon::Services {

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

void ServiceManager::RequestImpl(Readers& readers, Writers& writers,
                                 std::function<void(ServiceBase*)> add_service,
                                 Result& result, u32 id) {
    switch (id) {
    case 1: {
        LOG_DEBUG(HorizonServices, "GetServiceHandle");

        // auto in = *reinterpret_cast<GetServiceHandleIn*>(in_ptr);
        // std::string name(in.name);
        Service service = readers.reader.Read<Service>();
        Handle handle;
        switch (service) {
        case Service::Hid:
            add_service(new Hid::IHidServer());
            break;
        case Service::FspSrv:
            add_service(new Fssrv::IFileSystemProxy());
            break;
        case Service::TimeU:
            add_service(new TimeSrv::IStaticService());
            break;
        case Service::Nvdrv:
            add_service(new NvDrv::INvDrvServices());
            break;
        case Service::SetSys:
            add_service(new Settings::ISystemSettingsServer());
            break;
        case Service::Apm:
            add_service(new Am::IApmManager());
            break;
        case Service::AppletOE:
            add_service(new Am::IApplicationProxyService());
            break;
        case Service::ViM:
            add_service(new ViSrv::IManagerRootService());
            break;
        default:
            LOG_WARNING(HorizonServices, "Unknown service 0x{:016x} -> {}",
                        (u64)service, std::string((char*)(&service), 8));
            handle = UINT32_MAX;
            result = MAKE_KERNEL_RESULT(NotFound);
            // TODO: don't throw
            throw;
            break;
        }
        /*
        if (name == "hid") {
            handle = kernel.AddService<Hid::HidServer>();
        } else if (name == "fsp-srv") {
            handle = kernel.AddService<Fssrv::FileSystemProxy>();
        } else if (name == "time:u" || name == "time:a" || name == "time:r") {
            // TODO: are all these the same?
            handle = kernel.AddService<Time::StaticService>();
        } else if (name == "nvdrv") {
            handle = kernel.AddService<Nvdrv::NvDrvServices>();
        } else if (name == "set:sys") {
            handle = kernel.AddService<Settings::SystemSettingsServer>();
        } else if (name == "apm" || name == "apm:am") {
            handle = kernel.AddService<Apm::Manager>();
        } else {
            Logging::log(Logging::Level::Debug, "Unknown service name \"%s\"",
        name.c_str()); handle = UINT32_MAX; *res = MAKE_KERNEL_RESULT(NotFound);
        }
        */

        break;
    }
    default:
        LOG_WARNING(HorizonServices, "Unknown request {}", id);
        break;
    }
}

} // namespace Hydra::Horizon::Services
