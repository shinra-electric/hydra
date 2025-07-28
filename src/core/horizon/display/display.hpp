#pragma once

#include "core/horizon/display/layer.hpp"
#include "core/horizon/kernel/kernel.hpp"

namespace hydra::horizon::display {

class Display {
  public:
    Display() : vsync_event{new kernel::Event(false, "V-Sync event")} {}

    // TODO
    void Open() {}
    void Close() {}

    void
    AcquirePresentTextures(std::vector<std::chrono::nanoseconds>& out_dt_list);
    bool Present(u32 width, u32 height);

    // Layers
    u32 CreateLayer(u32 binder_id) {
        u32 id = layer_pool.AllocateForIndex();
        layer_pool.GetRef(id) = new Layer(binder_id);
        return id;
    }

    void DestroyLayer(u32 id) {
        delete layer_pool.Get(id);
        layer_pool.Free(id);
    }

    Layer& GetLayer(u32 id) { return *layer_pool.Get(id); }

  private:
    std::mutex mutex;
    kernel::Event* vsync_event;

    StaticPool<Layer*, 8> layer_pool;

  public:
    REF_GETTER(mutex, GetMutex);
    REF_GETTER(vsync_event, GetVSyncEvent);
};

} // namespace hydra::horizon::display
