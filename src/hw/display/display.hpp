#pragma once

#include "common/common.hpp"

namespace Hydra::HW::Display {

class LayerBase {
  public:
    LayerBase(u32 binder_id_) : binder_id{binder_id_} {}

    virtual void Open() {}  // TODO: = 0
    virtual void Close() {} // TODO: = 0

    // Getters
    u32 GetBinderId() { return binder_id; }

  protected:
    u32 binder_id;
};

class DisplayBase {
  public:
    // TODO: are these needed?
    virtual void Open() {}  // TODO: = 0
    virtual void Close() {} // TODO: = 0

    u32 CreateLayer(u32 binder_id) {
        u32 id = layers.size();
        layers.push_back(CreateLayerImpl(binder_id));

        return id;
    }

    // Getters
    LayerBase* GetLayer(u32 id) { return layers[id]; }

  protected:
    virtual LayerBase* CreateLayerImpl(u32 binder_id) {
        return new LayerBase(binder_id);
    } // TODO: = 0

  private:
    std::vector<LayerBase*> layers;
};

} // namespace Hydra::HW::Display
