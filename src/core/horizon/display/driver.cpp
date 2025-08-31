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
    std::vector<Layer*> sorted_layers;
    for (u32 layer_id = 1; layer_id < layer_pool.GetCapacity() + 1;
         layer_id++) {
        if (!layer_pool.IsValid(layer_id))
            continue;

        auto layer = layer_pool.Get(layer_id);

        // Find the correct position
        bool inserted = false;
        for (u32 i = 0; i < sorted_layers.size(); i++) {
            if (sorted_layers[i]->GetZ() > layer_pool.Get(layer_id)->GetZ()) {
                sorted_layers.insert(sorted_layers.begin() + i,
                                     layer_pool.Get(layer_id));
                inserted = true;
                break;
            }
        }
        if (!inserted)
            sorted_layers.push_back(layer_pool.Get(layer_id));
    }

    for (auto layer : sorted_layers)
        layer->Present(width, height);
}

Layer* Driver::GetFirstLayerForProcess(kernel::Process* process) {
    std::lock_guard lock(layer_mutex);
    for (u32 layer_id = 1; layer_id < layer_pool.GetCapacity() + 1;
         layer_id++) {
        if (!layer_pool.IsValid(layer_id))
            continue;

        auto layer = layer_pool.Get(layer_id);
        if (layer->GetProcess() == process)
            return layer;
    }

    return nullptr;
}

} // namespace hydra::horizon::display
