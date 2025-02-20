#include "hw/tegra_x1/gpu/gpu.hpp"

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

GPU::~GPU() { delete renderer; }

} // namespace Hydra::HW::TegraX1::GPU
