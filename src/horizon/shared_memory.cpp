#include "horizon/shared_memory.hpp"

#include "horizon/kernel.hpp"
#include "hw/tegra_x1/cpu/memory.hpp"
#include "hw/tegra_x1/cpu/mmu_base.hpp"

namespace Hydra::Horizon {

SharedMemory::SharedMemory(usize size) {
    mem = new HW::TegraX1::CPU::Memory(size, Permission::Read);
}

SharedMemory::~SharedMemory() { delete mem; }

void SharedMemory::MapToRange(HW::TegraX1::CPU::MMUBase* mmu,
                              const range<uptr> range) {
    mmu->Map(range.base,
             HW::TegraX1::CPU::MemoryMapping{mem->GetPtr(), range.size});
}

uptr SharedMemory::GetPtr() const { return mem->GetPtr(); }

} // namespace Hydra::Horizon
