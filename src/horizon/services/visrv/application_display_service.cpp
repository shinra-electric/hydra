#include "horizon/services/visrv/application_display_service.hpp"

#include "horizon/cmif.hpp"
#include "horizon/kernel.hpp"
#include "horizon/services/hosbinder/hos_binder_driver.hpp"
#include "horizon/services/visrv/manager_display_service.hpp"
#include "horizon/services/visrv/system_display_service.hpp"
#include "hw/bus.hpp"
#include "hw/display/display.hpp"

namespace Hydra::Horizon::Services::ViSrv {

void IApplicationDisplayService::Request(
    Writers& writers, u8* in_ptr,
    std::function<void(ServiceBase*)> add_service) {
    auto cmif_in = Cmif::read_in_header(in_ptr);

    Result* res = Cmif::write_out_header(writers.writer);

    switch (cmif_in.command_id) {
    case 100: // GetRelayService
        add_service(new HosBinder::IHOSBinderDriver());
        break;
    case 101: // GetSystemDisplayService
        add_service(new ISystemDisplayService());
        break;
    case 102: // GetManagerDisplayService
        add_service(new IManagerDisplayService());
        break;
    case 1010: {            // OpenDisplay
        u64 display_id = 0; // TODO: get based on the name
        Kernel::GetInstance().GetBus().GetDisplay(display_id)->Open();
        writers.writer.Write(display_id);
        break;
    }
    default:
        LOG_WARNING(HorizonServices, "Unknown request {}", cmif_in.command_id);
        break;
    }

    *res = RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::ViSrv
