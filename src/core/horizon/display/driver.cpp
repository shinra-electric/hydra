#include "core/horizon/display/driver.hpp"

namespace hydra::horizon::display {

Driver::Driver() { display_pool.Add(new Display()); }

bool Driver::AcquirePresentTextures() {
    // Signal V-Sync
    {
        // NOTE: we signal all displays at once for simplicity
        std::lock_guard lock(display_mutex);
        for (u32 display_id = 1; display_id < layer_pool.GetCapacity() + 1;
             display_id++) {
            if (!display_pool.IsValid(display_id))
                continue;
            display_pool.Get(display_id)->GetVSyncEvent()->Signal();
        }
    }

    // Acquire
    bool acquired = false;
    {
        std::lock_guard lock(layer_mutex);
        for (u32 layer_id = 1; layer_id < layer_pool.GetCapacity() + 1;
             layer_id++) {
            if (!layer_pool.IsValid(layer_id))
                continue;
            acquired |= layer_pool.Get(layer_id)->AcquirePresentTexture();
        }
    }

    return acquired;
}

void Driver::Present(u32 width, u32 height) {
    std::lock_guard lock(layer_mutex);
    for (u32 layer_id = 1; layer_id < layer_pool.GetCapacity() + 1;
         layer_id++) {
        if (!layer_pool.IsValid(layer_id))
            continue;
        layer_pool.Get(layer_id)->Present(width, height);
    }
}

Layer* Driver::GetMainLayer() {
    std::lock_guard lock(layer_mutex);
    for (u32 layer_id = 1; layer_id < layer_pool.GetCapacity() + 1;
         layer_id++) {
        if (layer_pool.IsValid(layer_id))
            return layer_pool.Get(layer_id);
    }

    return nullptr;
}

} // namespace hydra::horizon::display
