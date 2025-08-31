#include "core/horizon/services/visrv/manager_display_service.hpp"

#include "core/horizon/os.hpp"

namespace hydra::horizon::services::visrv {

DEFINE_SERVICE_COMMAND_TABLE(IManagerDisplayService, 2010, CreateManagedLayer,
                             2011, DestroyManagedLayer, 2012, CreateStrayLayer,
                             6000, AddToLayerStack, 6002, SetLayerVisibility)

result_t IManagerDisplayService::CreateManagedLayer(kernel::Process* process,
                                                    aligned<u32, 8> flags,
                                                    u64 display_id, u64 aruid,
                                                    u64* out_layer_id) {
    // TODO: flags
    u32 binder_id = OS_INSTANCE.GetDisplayDriver().CreateBinder();
    // TODO: what's the display for?
    // auto& display = OS_INSTANCE.GetDisplayDriver().GetDisplay(display_id);

    *out_layer_id =
        OS_INSTANCE.GetDisplayDriver().CreateLayer(process, binder_id);
    return RESULT_SUCCESS;
}

result_t IManagerDisplayService::DestroyManagedLayer(u64 layer_id) {
    OS_INSTANCE.GetDisplayDriver().DestroyLayer(layer_id);
    return RESULT_SUCCESS;
}

result_t IManagerDisplayService::CreateStrayLayer(
    kernel::Process* process, aligned<u32, 8> flags, u64 display_id,
    u64* out_layer_id, u64* out_native_window_size,
    OutBuffer<BufferAttr::MapAlias> out_parcel_buffer) {
    hosbinder::ParcelWriter parcel_writer(*out_parcel_buffer.writer);
    auto result = CreateStrayLayerImpl(process, flags, display_id, out_layer_id,
                                       out_native_window_size, parcel_writer);

    parcel_writer.Finalize();
    return RESULT_SUCCESS;
}

result_t IManagerDisplayService::AddToLayerStack(u32 stack, u64 layer_id) {
    LOG_FUNC_STUBBED(Services);

    LOG_DEBUG(Services, "Stack: {}, Layer ID: {}", stack, layer_id);

    return RESULT_SUCCESS;
}

result_t IManagerDisplayService::SetLayerVisibility(u64 layer_id,
                                                    bool visible) {
    return SetLayerVisibilityImpl(layer_id, visible);
}

} // namespace hydra::horizon::services::visrv
