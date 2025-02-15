#include "horizon/services/visrv/display_service_base.hpp"

#include "horizon/os.hpp"
#include "horizon/services/hosbinder/hos_binder_driver.hpp"

namespace Hydra::Horizon::Services::ViSrv {

struct CreateStrayLayerIn {
    u32 layer_flags;
    u64 display_id;
};

struct CreateStrayLayerOut {
    u64 layer_id;
    u64 native_window_size;
};

void DisplayServiceBase::CmdCreateStrayLayer(REQUEST_PARAMS_WITH_RESULT) {
    auto in = readers.reader.Read<CreateStrayLayerIn>();
    CreateStrayLayerOut out{
        .layer_id = 0,           // TODO
        .native_window_size = 0, // TODO
    };

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
        .binder_id = OS::GetInstance().GetHosBinderDriver()->AddBinder(),
    };
    writers.revc_buffers_writer.Write(data);
}

} // namespace Hydra::Horizon::Services::ViSrv
