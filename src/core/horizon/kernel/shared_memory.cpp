#include "core/horizon/kernel/shared_memory.hpp"

#include "core/hw/tegra_x1/cpu/mmu_base.hpp"

namespace Hydra::Horizon::Kernel {

SharedMemory::SharedMemory(usize size) {
    memory = HW::TegraX1::CPU::MMUBase::GetInstance().AllocateMemory(size);
}

SharedMemory::~SharedMemory() {
    HW::TegraX1::CPU::MMUBase::GetInstance().FreeMemory(memory);
}

void SharedMemory::MapToRange(
    const ::Hydra::range<uptr> range,
    MemoryPermission perm) { // TODO: why ::Hydra::range?
    HW::TegraX1::CPU::MMUBase::GetInstance().Map(
        range.base, memory, {MemoryType::Shared, MemoryAttribute::None, perm});
}

uptr SharedMemory::GetPtr() const {
    return HW::TegraX1::CPU::MMUBase::GetInstance().GetMemoryPtr(memory);
}

} // namespace Hydra::Horizon::Kernel
