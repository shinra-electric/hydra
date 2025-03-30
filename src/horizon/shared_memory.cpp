#include "horizon/shared_memory.hpp"

#include "horizon/kernel.hpp"
#include "hw/tegra_x1/cpu/hypervisor/mmu.hpp"
#include "hw/tegra_x1/cpu/mmu_base.hpp"

namespace Hydra::Horizon {

SharedMemory::SharedMemory(usize size) {
    memory =
        HW::TegraX1::CPU::Hypervisor::MMU::GetInstance().AllocateMemory(size);
}

SharedMemory::~SharedMemory() {
    HW::TegraX1::CPU::Hypervisor::MMU::GetInstance().FreeMemory(memory);
}

void SharedMemory::MapToRange(
    const ::Hydra::range<uptr> range_,
    MemoryPermission perm) { // TODO: why ::Hydra::range?
    range = range_;
    HW::TegraX1::CPU::Hypervisor::MMU::GetInstance().Map(
        range.base, memory, {MemoryType::Shared, MemoryAttribute::None, perm});
}

} // namespace Hydra::Horizon
