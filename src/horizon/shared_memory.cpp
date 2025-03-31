#include "horizon/shared_memory.hpp"

#include "horizon/kernel.hpp"
#include "hw/tegra_x1/cpu/mmu_base.hpp"

namespace Hydra::Horizon {

void SharedMemory::MapToRange(
    HW::TegraX1::CPU::MMUBase* mmu, const ::Hydra::range<uptr> range_,
    MemoryPermission perm) { // TODO: why ::Hydra::range?
    range = range_;
    mmu->AllocateAndMap(range.base, range.size,
                        {MemoryType::Shared, MemoryAttribute::None, perm});
}

} // namespace Hydra::Horizon
