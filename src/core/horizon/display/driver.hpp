#pragma once

#include "core/horizon/display/binder.hpp"
#include "core/horizon/display/display.hpp"

namespace hydra::horizon::display {

class Driver {
  public:
    // Binders
    u32 CreateBinder() {
        u32 id = binders.size();
        binders.push_back(new Binder());

        return id;
    }

    void DestroyBinder(u32 id) {
        delete binders[id];
        binders.erase(binders.begin() + id);
    }

    Binder& GetBinder(u32 id) { return *binders[id]; }

    // Displays
    u32 CreateDisplay() {
        u32 id = displays.size();
        displays.push_back(new Display());

        return id;
    }

    void DestroyDisplay(u32 id) {
        delete displays[id];
        displays.erase(displays.begin() + id);
    }

    Display& GetDisplay(u32 id) { return *displays[id]; }

  private:
    std::vector<Display*> displays;
    std::vector<Binder*> binders;
};

} // namespace hydra::horizon::display
