#include "core/horizon/display/display.hpp"

namespace hydra::horizon::display {

void Display::AcquirePresentTextures(
    std::vector<std::chrono::nanoseconds>& out_dt_list) {
    for (u32 id = 1; id < layer_pool.GetCapacity() + 1; id++) {
        if (!layer_pool.IsValid(id))
            continue;
        layer_pool.Get(id)->AcquirePresentTexture(out_dt_list);
    }
}

bool Display::Present(u32 width, u32 height) {
    std::lock_guard display_lock(mutex);

    // Signal V-Sync
    vsync_event->Signal();

    // Layers
    bool drawn = false;
    for (u32 id = 1; id < layer_pool.GetCapacity() + 1; id++) {
        if (!layer_pool.IsValid(id))
            continue;
        drawn = drawn || layer_pool.Get(id)->Present(width, height);
    }

    return drawn;
}

} // namespace hydra::horizon::display
