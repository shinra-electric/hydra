#include "hw/tegra_x1/gpu/gpu_mmu.hpp"

#include "hw/tegra_x1/cpu/mmu_base.hpp"

namespace Hydra::HW::TegraX1::GPU {

uptr GPUMMU::UnmapAddrToCpuAddr(uptr gpu_addr) {
    usize base;
    auto as = FindAddrImpl(gpu_addr, base);
    ASSERT_DEBUG(as.addr != 0x0, GPU, "Address 0x{:08x} is not host mapped",
                 gpu_addr);

    return as.addr + (gpu_addr - base);
}

uptr GPUMMU::UnmapAddr(uptr gpu_addr) {
    return mmu->UnmapAddr(UnmapAddrToCpuAddr(gpu_addr));
}

} // namespace Hydra::HW::TegraX1::GPU
