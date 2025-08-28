#include "core/horizon/display/display.hpp"

namespace hydra::horizon::display {

bool Display::AcquirePresentTextures() {
    std::lock_guard lock(mutex);
    bool acquired = false;
    for (u32 layer_id = 1; layer_id < layer_pool.GetCapacity() + 1;
         layer_id++) {
        if (!layer_pool.IsValid(layer_id))
            continue;
        acquired =
            acquired || layer_pool.Get(layer_id)->AcquirePresentTexture();
    }

    return acquired;
}

void Display::Present(u32 width, u32 height) {
    // Signal V-Sync
    vsync_event->Signal();

    // Layers
    {
        std::lock_guard lock(mutex);
        for (u32 layer_id = 1; layer_id < layer_pool.GetCapacity() + 1;
             layer_id++) {
            if (!layer_pool.IsValid(layer_id))
                continue;
            layer_pool.Get(layer_id)->Present(width, height);
        }
    }
}

Layer* Display::GetMainLayer() {
    std::lock_guard lock(mutex);
    // TODO: get the main layer for the main process
    const auto layer_id = 1;
    if (!layer_pool.IsValid(layer_id))
        return nullptr;

    return layer_pool.Get(layer_id);
}

} // namespace hydra::horizon::display
