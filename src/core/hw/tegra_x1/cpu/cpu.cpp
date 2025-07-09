#include "core/hw/tegra_x1/cpu/cpu.hpp"

namespace hydra::hw::tegra_x1::cpu {

SINGLETON_DEFINE_GET_INSTANCE(ICpu, Mmu)

ICpu::ICpu() { SINGLETON_SET_INSTANCE(Mmu, Mmu); }

ICpu::~ICpu() { SINGLETON_UNSET_INSTANCE(); }

} // namespace hydra::hw::tegra_x1::cpu
