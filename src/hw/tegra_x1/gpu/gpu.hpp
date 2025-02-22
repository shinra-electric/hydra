#pragma once

#include "common/allocators/dynamic_pool.hpp"
#include "common/common.hpp"
#include "hw/tegra_x1/gpu/renderer/renderer_base.hpp"
#include "hw/tegra_x1/gpu/texture_cache.hpp"

namespace Hydra::HW::TegraX1::CPU {
class MMUBase;
}

namespace Hydra::HW::TegraX1::GPU {

struct MemoryMap {
    uptr addr = 0;
    usize size;
    bool write;
    // TODO: alignment
    // TODO: kind
};

class GPU {
  public:
    static GPU& GetInstance();

    GPU(CPU::MMUBase* mmu_);
    ~GPU();

    // Memory map
    u32 CreateMap(usize size) {
        Handle handle = memory_maps.AllocateForIndex();
        MemoryMap& memory_map = memory_maps.GetObjectRef(handle);
        memory_map = {};
        memory_map.size = size;

        return handle;
    }

    void AllocateMap(Handle handle, uptr addr, bool write) {
        MemoryMap& memory_map = memory_maps.GetObjectRef(handle);
        memory_map.addr = addr;
        memory_map.write = write;
        LOG_DEBUG(GPU, "Addr: 0x{:08x}, map ID: {}", addr, GetMapId(handle));
    }

    u32 GetMapId(Handle handle) { return handle + 1; }

    MemoryMap& GetMapById(u32 id) { return memory_maps.GetObjectRef(id - 1); }

    // Descriptors

    // Texture
    TextureDescriptor CreateTextureDescriptor(const NvGraphicsBuffer& buff);

    // Getters
    CPU::MMUBase* GetMMU() const { return mmu; }

    TextureCache& GetTextureCache() { return texture_cache; }

    Renderer::RendererBase* GetRenderer() const { return renderer; }

  private:
    CPU::MMUBase* mmu;

    TextureCache texture_cache;

    Renderer::RendererBase* renderer;

    Allocators::DynamicPool<MemoryMap> memory_maps;
};

} // namespace Hydra::HW::TegraX1::GPU
