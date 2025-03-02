#pragma once

#include "common/allocators/dynamic_pool.hpp"
#include "common/common.hpp"
#include "hw/generic_mmu.hpp"
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

struct AddressSpace {
    usize size;
};

class MMU : public GenericMMU<MMU, AddressSpace> {
  public:
    usize ImplGetSize(AddressSpace as) const { return as.size; }

    void MapImpl(uptr base, AddressSpace as) {}
    void UnmapImpl(uptr base, AddressSpace as) {}
};

constexpr usize PAGE_SIZE = 0x4000; // TODO: what shoud this be?

class GPU {
  public:
    static GPU& GetInstance();

    GPU(CPU::MMUBase* mmu_);
    ~GPU();

    // Memory map
    u32 CreateMap(usize size) {
        HandleId handle_id = memory_maps.AllocateForIndex();
        MemoryMap& memory_map = memory_maps.GetObjectRef(handle_id);
        memory_map = {};
        memory_map.size = size;

        return handle_id;
    }

    void AllocateMap(HandleId handle_id, uptr addr, bool write) {
        MemoryMap& memory_map = memory_maps.GetObjectRef(handle_id);
        memory_map.addr = addr;
        memory_map.write = write;
    }

    void FreeMap(HandleId handle_id) { memory_maps.FreeByIndex(handle_id); }

    u32 GetMapId(HandleId handle_id) { return handle_id + 1; }

    MemoryMap& GetMap(HandleId handle_id) {
        return memory_maps.GetObjectRef(handle_id);
    }

    MemoryMap& GetMapById(u32 id) { return GetMap(id - 1); }

    // Address space
    uptr CreateAddressSpace(usize size, u32 flags) {
        AddressSpace as;
        as.size = size;
        // TODO: flags

        uptr base = address_space_base;
        address_space_base += align(size, PAGE_SIZE);
        mmu_gpu.Map(base, as);

        return base;
    }

    // Descriptors

    // Texture
    TextureDescriptor CreateTextureDescriptor(const NvGraphicsBuffer& buff);

    // Getters
    CPU::MMUBase* GetMMU() const { return mmu; }

    TextureCache& GetTextureCache() { return texture_cache; }

    Renderer::RendererBase* GetRenderer() const { return renderer; }

  private:
    CPU::MMUBase* mmu;

    // Address space
    MMU mmu_gpu;
    uptr address_space_base = PAGE_SIZE;

    // Caches
    TextureCache texture_cache;

    Renderer::RendererBase* renderer;

    Allocators::DynamicPool<MemoryMap> memory_maps;
};

} // namespace Hydra::HW::TegraX1::GPU
