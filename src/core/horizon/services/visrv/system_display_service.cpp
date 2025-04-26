#include "core/horizon/services/visrv/system_display_service.hpp"

namespace Hydra::Horizon::Services::ViSrv {

namespace {

// TODO: correct?
struct GetDisplayModeOut {
    u32 width;
    u32 height;
    float refresh_rate;
    i32 unknown;
};

} // namespace

DEFINE_SERVICE_COMMAND_TABLE(ISystemDisplayService, 2207, SetLayerVisibility,
                             2312, CreateStrayLayer, 3200, GetDisplayMode)

void ISystemDisplayService::GetDisplayMode(REQUEST_COMMAND_PARAMS) {
    LOG_FUNC_STUBBED(HorizonServices);

    const auto display_id = readers.reader.Read<u64>();

    // TODO: get this from the display
    writers.writer.Write<GetDisplayModeOut>({
        .width = 1280,
        .height = 720,
        .refresh_rate = 60.0f,
        .unknown = 0,
    });
}

} // namespace Hydra::Horizon::Services::ViSrv
