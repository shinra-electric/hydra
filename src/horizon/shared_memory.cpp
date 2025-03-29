#include "horizon/shared_memory.hpp"

#include "horizon/kernel.hpp"
#include "hw/tegra_x1/cpu/memory.hpp"
#include "hw/tegra_x1/cpu/mmu_base.hpp"

namespace Hydra::Horizon {

SharedMemory::SharedMemory(usize size) {
    // Add PAGE_SIZE - ORIGINAL_PAGE_SIZE to the size to ensure that the memory
    // can be aligned to a page when mapping
    mem = new HW::TegraX1::CPU::Memory(size + HW::TegraX1::CPU::PAGE_SIZE -
                                           HW::TegraX1::CPU::ORIGINAL_PAGE_SIZE,
                                       Permission::Read);
}

SharedMemory::~SharedMemory() { delete mem; }

void SharedMemory::MapToRange(HW::TegraX1::CPU::MMUBase* mmu,
                              const range<uptr> range) {
    uptr base = align_down(range.base, HW::TegraX1::CPU::PAGE_SIZE);

    mmu->Map(base, HW::TegraX1::CPU::MemoryMapping{
                       mem->GetPtr(),
                       align(range.size, HW::TegraX1::CPU::PAGE_SIZE)});
}

uptr SharedMemory::GetPtr() const { return mem->GetPtr(); }

} // namespace Hydra::Horizon
