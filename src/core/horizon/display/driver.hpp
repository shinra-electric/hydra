#pragma once

#include "core/horizon/display/binder.hpp"
#include "core/horizon/display/display.hpp"

namespace hydra::horizon::display {

class Driver {
  public:
    // Displays
    u32 CreateDisplay() {
        std::lock_guard lock(display_mutex);
        u32 id = display_pool.AllocateHandle();
        display_pool.Get(id) = new Display();
        return id;
    }

    void DestroyDisplay(u32 id) {
        std::lock_guard lock(display_mutex);
        delete display_pool.Get(id);
        display_pool.Free(id);
    }

    Display& GetDisplay(u32 id) {
        std::lock_guard lock(display_mutex);
        return *display_pool.Get(id);
    }

    // Binders
    u32 CreateBinder() {
        std::lock_guard lock(binder_mutex);
        u32 id = binder_pool.AllocateHandle();
        binder_pool.Get(id) = new Binder();
        return id;
    }

    void DestroyBinder(u32 id) {
        std::lock_guard lock(binder_mutex);
        delete binder_pool.Get(id);
        binder_pool.Free(id);
    }

    Binder& GetBinder(u32 id) {
        std::lock_guard lock(binder_mutex);
        return *binder_pool.Get(id);
    }

    // Main layer
    Layer* GetMainLayer() {
        std::lock_guard lock(display_mutex);
        // TODO: get the main display for the main process
        const auto display_id = 1;
        if (!display_pool.IsValid(display_id))
            return nullptr;

        return display_pool.Get(display_id)->GetMainLayer();
    }

  private:
    std::mutex display_mutex;
    StaticPool<Display*, 8> display_pool;
    std::mutex binder_mutex;
    StaticPool<Binder*, 16, true>
        binder_pool; // Allow zero handle (official games expect binder IDs to
                     // behave as indices, e.g. they expect the first binder ID
                     // to be 0)
};

} // namespace hydra::horizon::display
