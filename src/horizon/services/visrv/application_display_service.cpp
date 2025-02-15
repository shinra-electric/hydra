#include "horizon/services/visrv/application_display_service.hpp"

#include "horizon/kernel.hpp"
#include "horizon/os.hpp"
#include "horizon/services/hosbinder/hos_binder_driver.hpp"
#include "horizon/services/service_base.hpp"
#include "hw/bus.hpp"
#include "hw/display/display.hpp"

namespace Hydra::Horizon::Services::ViSrv {

struct OpenLayerIn {
    u64 display_name;
    u64 layer_id;
    u64 applet_resource_user_id;
};

void IApplicationDisplayService::RequestImpl(REQUEST_IMPL_PARAMS) {
    switch (id) {
    case 100: // GetRelayService
        add_service(GET_SERVICE_EXPLICIT(HosBinder, hos_binder_driver));
        break;
    case 101: // GetSystemDisplayService
        add_service(GET_SERVICE_EXPLICIT(ViSrv, system_display_service));
        break;
    case 102: // GetManagerDisplayService
        add_service(GET_SERVICE_EXPLICIT(ViSrv, manager_display_service));
        break;
    case 1010: // OpenDisplay
        CmdOpenDisplay(PASS_REQUEST_PARAMS_WITH_RESULT);
        break;
    case 1020: // CloseDisplay
        CmdCloseDisplay(PASS_REQUEST_PARAMS_WITH_RESULT);
        break;
    case 2020: // OpenLayer
        CmdOpenLayer(PASS_REQUEST_PARAMS_WITH_RESULT);
        break;
    default:
        LOG_WARNING(HorizonServices, "Unknown request {}", id);
        break;
    }
}

void IApplicationDisplayService::CmdOpenDisplay(REQUEST_PARAMS_WITH_RESULT) {
    u64 display_id = 0; // TODO: get based on the name
    Kernel::GetInstance().GetBus().GetDisplay(display_id)->Open();
    writers.writer.Write(display_id);
}

void IApplicationDisplayService::CmdCloseDisplay(REQUEST_PARAMS_WITH_RESULT) {
    u64 display_id = readers.reader.Read<u64>();
    Kernel::GetInstance().GetBus().GetDisplay(display_id)->Close();
}

void IApplicationDisplayService::CmdOpenLayer(REQUEST_PARAMS_WITH_RESULT) {
    auto in = readers.reader.Read<OpenLayerIn>();

    // Out
    // TODO: output window size

    // Parcel
    Parcel parcel{
        .data_size = sizeof(ParcelData),
        .data_offset = sizeof(Parcel),
        .objects_size = 0,
        .objects_offset = 0,
    };
    writers.revc_buffers_writer.Write(parcel);

    // Parcel data
    ParcelData data{
        .binder_id = GET_SERVICE(HosBinder, hos_binder_driver)->AddBinder(),
    };
    writers.revc_buffers_writer.Write(data);
}

} // namespace Hydra::Horizon::Services::ViSrv
