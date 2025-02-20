#pragma once

#include "common/common.hpp"

namespace Hydra::HW::Display {

class LayerBase;

class DisplayBase {
  public:
    virtual ~DisplayBase() {}

    // TODO: are these needed?
    virtual void Open() = 0;
    virtual void Close() = 0;

    virtual void Run() = 0;

    u32 CreateLayer(u32 binder_id) {
        u32 id = layers.size();
        layers.push_back(CreateLayerImpl(binder_id));

        return id;
    }

    // Getters
    LayerBase* GetLayer(u32 id) { return layers[id]; }

  protected:
    virtual LayerBase* CreateLayerImpl(u32 binder_id) = 0;

  private:
    std::vector<LayerBase*> layers;
};

} // namespace Hydra::HW::Display
