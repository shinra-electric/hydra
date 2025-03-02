#include "hw/tegra_x1/gpu/gpu.hpp"

#include "hw/tegra_x1/cpu/mmu_base.hpp"
#include "hw/tegra_x1/gpu/const.hpp"
#include "hw/tegra_x1/gpu/renderer/metal/renderer.hpp"

namespace Hydra::HW::TegraX1::GPU {

static GPU* s_instance = nullptr;

GPU& GPU::GetInstance() { return *s_instance; }

GPU::GPU(CPU::MMUBase* mmu_) : mmu{mmu_} {
    ASSERT(s_instance == nullptr, GPU, "GPU already exists");
    s_instance = this;

    // TODO: choose based on the Renderer backend
    {
        renderer = new Renderer::Metal::Renderer();
    }
}

GPU::~GPU() {
    delete renderer;

    s_instance = nullptr;
}

void GPU::SubmitCommands(const std::vector<GpfifoEntry>& entries) {
    for (const auto& entry : entries) {
        uptr gpu_addr = entry.entry & 0xffffffffff; // 40 bits
        usize size = (entry.entry32[1] >> 10) &
                     0xfffff; // TODO: is the mask correct? 20 bits
        bool allow_flush = entry.entry32[1] & 0x100;
        bool is_push_buf = entry.entry32[1] & 0x200;
        bool sync = entry.entry32[1] & 0x10000000;

        LOG_DEBUG(GPU,
                  "Gpfifo entry (address: 0x{:08x}, size: 0x{:08x}, allow "
                  "flush: {}, is push buf: {}, sync: {})",
                  gpu_addr, size, allow_flush, is_push_buf, sync);
    }
}

TextureDescriptor GPU::CreateTextureDescriptor(const NvGraphicsBuffer& buff) {
    LOG_DEBUG(GPU,
              "Map id: {}, width: {}, "
              "height: {}",
              buff.nvmap_id, buff.planes[0].width, buff.planes[0].height);
    TextureDescriptor descriptor;
    descriptor.ptr =
        mmu->UnmapAddr(GetMapById(buff.nvmap_id).addr + buff.planes[0].offset);
    // TODO: why are there more planes?
    descriptor.color_format = buff.planes[0].color_format;
    descriptor.kind = buff.planes[0].kind;
    descriptor.width = buff.planes[0].width;
    descriptor.height = buff.planes[0].height;
    descriptor.stride = buff.stride;
    descriptor.block_height_log2 = buff.planes[0].block_height_log2;
    descriptor.pitch = buff.planes[0].pitch;

    return descriptor;
}

} // namespace Hydra::HW::TegraX1::GPU
