#pragma once

#include "core/horizon/display/binder.hpp"
#include "core/horizon/display/display.hpp"

namespace hydra::horizon::display {

class Driver {
  public:
    // Binders
    u32 CreateBinder() {
        u32 id = binder_pool.AllocateForIndex();
        binder_pool.GetRef(id) = new Binder();
        return id;
    }

    void DestroyBinder(u32 id) {
        delete binder_pool.Get(id);
        binder_pool.Free(id);
    }

    Binder& GetBinder(u32 id) { return *binder_pool.Get(id); }

    // Displays
    u32 CreateDisplay() {
        u32 id = display_pool.AllocateForIndex();
        display_pool.GetRef(id) = new Display();
        return id;
    }

    void DestroyDisplay(u32 id) {
        delete display_pool.Get(id);
        display_pool.Free(id);
    }

    Display& GetDisplay(u32 id) { return *display_pool.Get(id); }

  private:
    StaticPool<Display*, 8> display_pool;
    StaticPool<Binder*, 16> binder_pool;
};

} // namespace hydra::horizon::display
