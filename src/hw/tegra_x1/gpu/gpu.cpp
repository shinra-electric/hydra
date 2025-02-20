#include "hw/tegra_x1/gpu/gpu.hpp"

namespace Hydra::HW::TegraX1::GPU {

static GPU* s_instance = nullptr;

GPU& GPU::GetInstance() { return *s_instance; }

GPU::GPU(CPU::MMUBase* mmu_) : mmu{mmu_} {
    ASSERT(s_instance == nullptr, GPU, "GPU already exists");
    s_instance = this;

    // TODO: create renderer
}

GPU::~GPU() {}

} // namespace Hydra::HW::TegraX1::GPU
