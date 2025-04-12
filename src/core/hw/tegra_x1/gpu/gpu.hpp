#pragma once

#include "common/allocators/dynamic_pool.hpp"
#include "core/hw/tegra_x1/gpu/const.hpp"
#include "core/hw/tegra_x1/gpu/engines/engine_base.hpp"
#include "core/hw/tegra_x1/gpu/gpu_mmu.hpp"
#include "core/hw/tegra_x1/gpu/pfifo.hpp"
#include "core/hw/tegra_x1/gpu/renderer/renderer_base.hpp"

#define RENDERER GPU::GetInstance().GetRenderer()

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

constexpr usize PAGE_SIZE = 0x20000; // Big page size (TODO: correct?)

constexpr usize SUBCHANNEL_COUNT = 5; // TODO: correct?

class GPU {
  public:
    static GPU& GetInstance();

    GPU(CPU::MMUBase* mmu_);
    ~GPU();

    // Memory map
    u32 CreateMap(usize size) {
        handle_id_t handle_id = memory_maps.AllocateForIndex();
        MemoryMap& memory_map = memory_maps.GetObjectRef(handle_id);
        memory_map = {};
        memory_map.size = size;

        return handle_id;
    }

    void AllocateMap(handle_id_t handle_id, uptr addr, bool write) {
        MemoryMap& memory_map = memory_maps.GetObjectRef(handle_id);
        memory_map.addr = addr;
        memory_map.write = write;
    }

    void FreeMap(handle_id_t handle_id) { memory_maps.FreeByIndex(handle_id); }

    u32 GetMapId(handle_id_t handle_id) { return handle_id + 1; }

    handle_id_t GetMapHandleId(u32 id) { return id - 1; }

    MemoryMap& GetMap(handle_id_t handle_id) {
        return memory_maps.GetObjectRef(handle_id);
    }

    MemoryMap& GetMapById(u32 id) { return GetMap(GetMapHandleId(id)); }

    // Address space
    uptr CreateAddressSpace(uptr addr, usize size, uptr gpu_addr) {
        AddressSpace as;
        as.addr = addr;
        as.size = size;

        if (gpu_addr == invalid<uptr>()) {
            gpu_addr =
                address_space_base; // TODO: ask the MMU for a base address
            address_space_base += align(size, PAGE_SIZE);
        }
        gpu_mmu.Map(gpu_addr, as);

        return gpu_addr;
    }

    uptr AllocatePrivateAddressSpace(usize size, uptr gpu_addr) {
        return CreateAddressSpace(0, size, gpu_addr);
    }

    uptr MapBufferToAddressSpace(uptr addr, usize size, uptr gpu_addr) {
        return CreateAddressSpace(addr, size, gpu_addr);
    }

    // Engines
    Engines::EngineBase* GetEngineAtSubchannel(u32 subchannel) {
        ASSERT_DEBUG(subchannel <= SUBCHANNEL_COUNT, GPU,
                     "Invalid subchannel {}", subchannel);

        auto engine = subchannels[subchannel];
        ASSERT_DEBUG(engine, GPU, "Subchannel {} does not have a bound engine",
                     subchannel);

        return engine;
    }

    void SubchannelMethod(u32 subchannel, u32 method, u32 arg);

    void SubchannelFlushMacro(u32 subchannel) {
        GetEngineAtSubchannel(subchannel)->FlushMacro();
    }

    // Texture
    Renderer::TextureBase* GetTexture(const NvGraphicsBuffer& buff);

    // Getters
    CPU::MMUBase* GetMMU() const { return mmu; }

    GPUMMU& GetGPUMMU() { return gpu_mmu; }

    Pfifo& GetPfifo() { return pfifo; }

    Renderer::RendererBase* GetRenderer() const { return renderer; }

  private:
    CPU::MMUBase* mmu;

    // Address space
    GPUMMU gpu_mmu;
    uptr address_space_base{PAGE_SIZE};

    // Pfifo
    Pfifo pfifo;

    // Engines
    Engines::EngineBase* subchannels[SUBCHANNEL_COUNT] = {nullptr};

    // Renderer
    Renderer::RendererBase* renderer;

    // Memory
    Allocators::DynamicPool<MemoryMap> memory_maps;
};

} // namespace Hydra::HW::TegraX1::GPU
