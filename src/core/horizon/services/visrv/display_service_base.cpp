#include "core/horizon/services/visrv/display_service_base.hpp"

#include "core/horizon/os.hpp"

namespace hydra::horizon::services::visrv {

result_t DisplayServiceBase::CreateStrayLayerImpl(
    u32 flags, u64 display_id, u64* out_layer_id, u64* out_native_window_size,
    hosbinder::ParcelWriter& out_parcel_writer) {
    u32 binder_id = OS::GetInstance().GetDisplayDriver().CreateBinder();
    auto& display = OS_INSTANCE.GetDisplayDriver().GetDisplay(display_id);
    std::unique_lock<std::mutex> display_lock(display.GetMutex());

    // Out
    *out_layer_id = display.CreateLayer(binder_id);
    *out_native_window_size =
        sizeof(hosbinder::ParcelHeader) + sizeof(ParcelData);

    // TODO: correct?
    out_parcel_writer.Write<ParcelData>({
        .unknown0 = 0x2,
        .unknown1 = 0x0, // TODO
        .binder_id = binder_id,
        .unknown2 = {0x0},
        .str = "dispdrv"_u64,
        .unknown3 = 0x0,
    });

    return RESULT_SUCCESS;
}

result_t DisplayServiceBase::SetLayerVisibilityImpl(u64 layer_id,
                                                    bool visible) {
    LOG_FUNC_STUBBED(Services);

    LOG_DEBUG(Services, "Layer ID: {}, visible: {}", layer_id, visible);

    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::visrv
