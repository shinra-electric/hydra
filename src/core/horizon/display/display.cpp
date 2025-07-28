#include "core/horizon/display/display.hpp"

namespace hydra::horizon::display {

void Display::AcquirePresentTextures(
    std::vector<std::chrono::nanoseconds>& out_dt_list) {
    for (u32 i = 0; i < layer_pool.GetCapacity(); i++) {
        auto layer = layer_pool.GetOrDefault(i);
        if (layer)
            layer->AcquirePresentTexture(out_dt_list);
    }
}

bool Display::Present(u32 width, u32 height) {
    std::lock_guard display_lock(mutex);

    // Signal V-Sync
    vsync_event->Signal();

    // Layers
    bool drawn = false;
    for (u32 i = 0; i < layer_pool.GetCapacity(); i++) {
        auto layer = layer_pool.GetOrDefault(i);
        if (layer)
            drawn = drawn || layer->Present(width, height);
    }

    return drawn;
}

} // namespace hydra::horizon::display
