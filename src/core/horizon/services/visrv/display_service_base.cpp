#include "core/horizon/services/visrv/display_service_base.hpp"

#include "core/horizon/os.hpp"
#include "core/horizon/services/hosbinder/parcel.hpp"
#include "core/hw/bus.hpp"
#include "core/hw/display/display.hpp"

namespace Hydra::Horizon::Services::ViSrv {

struct CreateStrayLayerIn {
    u32 layer_flags;
    u32 pad;
    u64 display_id;
};

struct CreateStrayLayerOut {
    u64 layer_id;
    u64 native_window_size;
};

void DisplayServiceBase::CreateStrayLayer(REQUEST_COMMAND_PARAMS) {
    auto in = readers.reader.Read<CreateStrayLayerIn>();

    u32 binder_id = OS::GetInstance().GetDisplayDriver().AddBinder();

    // Out
    CreateStrayLayerOut out{
        .layer_id = Kernel::Kernel::GetInstance()
                        .GetBus()
                        .GetDisplay(in.display_id)
                        ->CreateLayer(binder_id),
        .native_window_size =
            sizeof(HosBinder::ParcelHeader) + sizeof(ParcelData),
    };
    writers.writer.Write(out);

    // Parcel
    HosBinder::ParcelWriter parcel_writer(writers.recv_buffers_writers[0]);

    // TODO: correct?
    parcel_writer.Write<ParcelData>({
        .unknown0 = 0x2,
        .unknown1 = 0x0, // TODO
        .binder_id = binder_id,
        .unknown2 = {0x0},
        .str = str_to_u64("dispdrv"),
        .unknown3 = 0x0,
    });

    parcel_writer.Finalize();
}

void DisplayServiceBase::SetLayerVisibility(REQUEST_COMMAND_PARAMS) {
    LOG_FUNC_NOT_IMPLEMENTED(HorizonServices);
}

} // namespace Hydra::Horizon::Services::ViSrv
