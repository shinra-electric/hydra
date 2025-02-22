#pragma once

#include "common/common.hpp"
#include "hw/display/layer.hpp"

namespace Hydra::HW::Display {

class Display {
  public:
    // TODO: are these needed?
    void Open() {}
    void Close() {}

    u32 CreateLayer(u32 binder_id) {
        u32 id = layers.size();
        layers.push_back(new Layer(binder_id));

        return id;
    }

    // TODO: make this more sophisticated
    Layer* GetPresentableLayer() {
        if (layers.empty())
            return nullptr;

        return layers.back();
    }

    // Getters
    Layer* GetLayer(u32 id) { return layers[id]; }

  private:
    std::vector<Layer*> layers;
};

} // namespace Hydra::HW::Display
