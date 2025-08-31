#include "core/horizon/services/visrv/system_display_service.hpp"

#include "core/horizon/os.hpp"

namespace hydra::horizon::services::visrv {

DEFINE_SERVICE_COMMAND_TABLE(ISystemDisplayService, 1202, GetZOrderCountMax,
                             2201, SetLayerPosition, 2203, SetLayerSize, 2205,
                             SetLayerZ, 2207, SetLayerVisibility, 2312,
                             CreateStrayLayer, 3200, GetDisplayMode)

result_t ISystemDisplayService::GetZOrderCountMax(u64 display_id,
                                                  i64* out_count) {
    LOG_FUNC_STUBBED(Services);

    // HACK
    *out_count = 16;
    return RESULT_SUCCESS;
}

result_t ISystemDisplayService::SetLayerPosition(f32 x, f32 y, u64 layer_id) {
    LOG_FUNC_STUBBED(Services);

    LOG_DEBUG(Services, "Layer ID: {}, position: ({}, {})", layer_id, x, y);

    return RESULT_SUCCESS;
}

result_t ISystemDisplayService::SetLayerSize(u64 layer_id, i64 width,
                                             i64 height) {
    LOG_FUNC_STUBBED(Services);

    LOG_DEBUG(Services, "Layer ID: {}, size: {}x{}", layer_id, width, height);

    return RESULT_SUCCESS;
}

result_t ISystemDisplayService::SetLayerZ(u64 layer_id, i64 z) {
    LOG_FUNC_STUBBED(Services);

    LOG_DEBUG(Services, "Layer ID: {}, Z: {}", layer_id, z);

    return RESULT_SUCCESS;
}

result_t ISystemDisplayService::CreateStrayLayer(
    aligned<u32, 8> flags, u64 display_id, u64* out_layer_id,
    u64* out_native_window_size,
    OutBuffer<BufferAttr::MapAlias> out_parcel_buffer) {
    hosbinder::ParcelWriter parcel_writer(*out_parcel_buffer.writer);
    auto result = CreateStrayLayerImpl(flags, display_id, out_layer_id,
                                       out_native_window_size, parcel_writer);

    parcel_writer.Finalize();
    return RESULT_SUCCESS;
}

result_t ISystemDisplayService::SetLayerVisibility(u64 layer_id, bool visible) {
    return SetLayerVisibilityImpl(layer_id, visible);
}

result_t ISystemDisplayService::GetDisplayMode(u64 display_id, u32* out_width,
                                               u32* out_height,
                                               float* out_refresh_rate,
                                               i32* out_unknown) {
    LOG_FUNC_STUBBED(Services);

    const auto res = OS_INSTANCE.GetDisplayResolution();
    *out_width = res.x();
    *out_height = res.y();
    *out_refresh_rate = 60.0f;
    *out_unknown = 0;
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::visrv
