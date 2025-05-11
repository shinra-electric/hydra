#include "core/horizon/services/visrv/display_service_base.hpp"

#include "core/horizon/os.hpp"
#include "core/hw/bus.hpp"
#include "core/hw/display/display.hpp"

namespace Hydra::Horizon::Services::ViSrv {

result_t DisplayServiceBase::CreateStrayLayerImpl(
    u32 flags, u64 display_id, u64* out_layer_id, u64* out_native_window_size,
    HosBinder::ParcelWriter& out_parcel_writer) {
    u32 binder_id = OS::GetInstance().GetDisplayDriver().AddBinder();

    // Out
    *out_layer_id = Kernel::Kernel::GetInstance()
                        .GetBus()
                        .GetDisplay(display_id)
                        ->CreateLayer(binder_id);
    *out_native_window_size =
        sizeof(HosBinder::ParcelHeader) + sizeof(ParcelData);

    // TODO: correct?
    out_parcel_writer.Write<ParcelData>({
        .unknown0 = 0x2,
        .unknown1 = 0x0, // TODO
        .binder_id = binder_id,
        .unknown2 = {0x0},
        .str = str_to_u64("dispdrv"),
        .unknown3 = 0x0,
    });

    return RESULT_SUCCESS;
}

result_t DisplayServiceBase::SetLayerVisibilityImpl(u64 layer_id,
                                                    bool visible) {
    LOG_DEBUG(HorizonServices, "Layer ID: {}, visible: {}", layer_id, visible);

    LOG_FUNC_NOT_IMPLEMENTED(HorizonServices);
    return RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::ViSrv
