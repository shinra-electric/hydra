#include "core/horizon/services/visrv/display_service_base.hpp"

#include "core/horizon/os.hpp"

namespace hydra::horizon::services::visrv {

// TODO: flags, display ID
result_t DisplayServiceBase::CreateStrayLayerImpl(
    kernel::Process* process, u32 flags, u64 display_id, u64* out_layer_id,
    u64* out_native_window_size, io::MemoryStream* out_parcel_stream) {
    (void)flags;
    (void)display_id;

    u32 binder_id = OS_INSTANCE.GetDisplayDriver().CreateBinder();
    // TODO: what's the display for?
    // auto& display = OS_INSTANCE.GetDisplayDriver().GetDisplay(display_id);

    *out_layer_id =
        OS_INSTANCE.GetDisplayDriver().CreateLayer(process, binder_id);

    // Parcel
    hosbinder::ParcelWriter parcel_writer(out_parcel_stream);
    parcel_writer.WriteObject(binder_id, "dispdrv"_u64);
    parcel_writer.Finish();

    *out_native_window_size = parcel_writer.GetWrittenSize();

    return RESULT_SUCCESS;
}

result_t DisplayServiceBase::SetLayerVisibilityImpl(u64 layer_id,
                                                    bool visible) {
    LOG_FUNC_WITH_ARGS_STUBBED(Services, "layer ID: {}, visible: {}", layer_id,
                               visible);
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::visrv
