#include "horizon/services/sm.hpp"

#include "horizon/cmif.hpp"
#include "horizon/const.hpp"
#include "horizon/kernel.hpp"
#include "horizon/services/am/application_proxy_service.hpp"
#include "horizon/services/apm/manager.hpp"
#include "horizon/services/fssrv/filesystem_proxy.hpp"
#include "horizon/services/hid/hid_server.hpp"
#include "horizon/services/nvdrv/nvdrv_services.hpp"
#include "horizon/services/settings/system_settings_server.hpp"
#include "horizon/services/time/static_service.hpp"

namespace Hydra::Horizon::Services {

enum class Service : u64 {
    Hid = 0x0000000000646968,
    FspSrv = 0x007672732d707366,
    TimeU = 0x0000753a656d6974,
    Nvdrv = 0x000000767264766e,
    SetSys = 0x007379733a746573,
    Apm = 0x00000000006d7061,
    AppletOE = 0x454f74656c707061,
};

void ServiceManager::Request(Writers& writers, u8* in_ptr,
                             std::function<void(ServiceBase*)> add_service) {
    auto cmif_in = Cmif::read_in_header(in_ptr);

    Result* res = Cmif::write_out_header(writers.writer);
    *res = RESULT_SUCCESS;

    switch (cmif_in.command_id) {
    case 1: {
        Logging::log(Logging::Level::Debug, "GetServiceHandle");

        // auto in = *reinterpret_cast<GetServiceHandleIn*>(in_ptr);
        // std::string name(in.name);
        Service service = *reinterpret_cast<Service*>(in_ptr);
        Handle handle;
        switch (service) {
        case Service::Hid:
            add_service(new Hid::HidServer());
            break;
        case Service::FspSrv:
            add_service(new Fssrv::FileSystemProxy());
            break;
        case Service::TimeU:
            add_service(new Time::StaticService());
            break;
        case Service::Nvdrv:
            add_service(new Nvdrv::NvDrvServices());
            break;
        case Service::SetSys:
            add_service(new Settings::SystemSettingsServer());
            break;
        case Service::Apm:
            add_service(new Apm::Manager());
            break;
        case Service::AppletOE:
            add_service(new Am::ApplicationProxyService());
            break;
        default:
            Logging::log(Logging::Level::Warning,
                         "Unknown service 0x{:08x} -> {}", (u64)service,
                         std::string((char*)in_ptr, 8));
            handle = UINT32_MAX;
            *res = MAKE_KERNEL_RESULT(NotFound);
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
        Logging::log(Logging::Level::Warning, "Unknown sm command {}",
                     cmif_in.command_id);
        break;
    }
}

} // namespace Hydra::Horizon::Services
