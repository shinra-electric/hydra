#include "core/horizon/services/visrv/application_display_service.hpp"

#include "core/horizon/kernel.hpp"
#include "core/horizon/os.hpp"
#include "core/horizon/services/hosbinder/hos_binder_driver.hpp"
#include "core/horizon/services/service_base.hpp"
#include "core/horizon/services/visrv/manager_display_service.hpp"
#include "core/horizon/services/visrv/system_display_service.hpp"
#include "core/hw/bus.hpp"
#include "core/hw/display/display.hpp"

namespace Hydra::Horizon::Services::ViSrv {

namespace {

struct OpenLayerIn {
    u64 display_name;
    u64 layer_id;
    u64 applet_resource_user_id;
};

} // namespace

DEFINE_SERVICE_COMMAND_TABLE(IApplicationDisplayService, 100, GetRelayService,
                             101, GetSystemDisplayService, 102,
                             GetManagerDisplayService, 1010, OpenDisplay, 1020,
                             CloseDisplay, 2020, OpenLayer, 2021, CloseLayer,
                             2101, SetLayerScalingMode)

void IApplicationDisplayService::GetRelayService(REQUEST_COMMAND_PARAMS) {
    add_service(new HosBinder::IHOSBinderDriver());
}

void IApplicationDisplayService::GetSystemDisplayService(
    REQUEST_COMMAND_PARAMS) {
    add_service(new ISystemDisplayService());
}

void IApplicationDisplayService::GetManagerDisplayService(
    REQUEST_COMMAND_PARAMS) {
    add_service(new IManagerDisplayService());
}

void IApplicationDisplayService::OpenDisplay(REQUEST_COMMAND_PARAMS) {
    u64 display_id = 0; // TODO: get based on the name
    Kernel::GetInstance().GetBus().GetDisplay(display_id)->Open();
    writers.writer.Write(display_id);
}

void IApplicationDisplayService::CloseDisplay(REQUEST_COMMAND_PARAMS) {
    u64 display_id = readers.reader.Read<u64>();
    Kernel::GetInstance().GetBus().GetDisplay(display_id)->Close();
}

void IApplicationDisplayService::OpenLayer(REQUEST_COMMAND_PARAMS) {
    auto in = readers.reader.Read<OpenLayerIn>();

    u64 display_id = 0; // TODO: get based on the name

    auto layer = Kernel::GetInstance()
                     .GetBus()
                     .GetDisplay(display_id)
                     ->GetLayer(in.layer_id);
    layer->Open();
    LOG_DEBUG(HorizonServices, "OPENED LAYER {}", in.layer_id);

    // Out
    // TODO: correct?
    writers.writer.Write(sizeof(Parcel) + sizeof(ParcelData));

    // Parcel
    Parcel parcel{
        .data_size = sizeof(ParcelData),
        .data_offset = sizeof(Parcel),
        .objects_size = 0,
        .objects_offset = 0,
    };
    writers.recv_buffers_writers[0].Write(parcel);

    // Parcel data
    ParcelData data{
        .unknown0 = 0x2,
        .unknown1 = 0x0, // TODO
        .binder_id = layer->GetBinderId(),
        .unknown2 = {0x0},
        .str = str_to_u64("dispdrv"),
        .unknown3 = 0x0,
    };
    writers.recv_buffers_writers[0].Write(data);
}

void IApplicationDisplayService::CloseLayer(REQUEST_COMMAND_PARAMS) {
    auto layer_id = readers.reader.Read<u64>();

    u64 display_id = 0; // TODO: get from layer ID

    Kernel::GetInstance()
        .GetBus()
        .GetDisplay(display_id)
        ->GetLayer(layer_id)
        ->Close();
}

} // namespace Hydra::Horizon::Services::ViSrv
