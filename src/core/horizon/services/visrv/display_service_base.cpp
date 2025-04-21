#include "core/horizon/services/visrv/display_service_base.hpp"

#include "core/horizon/os.hpp"
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
        .native_window_size = sizeof(ParcelData) + sizeof(Parcel),
    };
    writers.writer.Write(out);

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
        .binder_id = binder_id,
    };
    writers.recv_buffers_writers[0].Write(data);
}

} // namespace Hydra::Horizon::Services::ViSrv
