#include "hw/tegra_x1/cpu/dynarmic/mmu.hpp"

#include "hw/tegra_x1/cpu/dynarmic/memory.hpp"

namespace Hydra::HW::TegraX1::CPU::Dynarmic {

MMU::MMU() {}

MMU::~MMU() {}

MemoryBase* MMU::AllocateMemory(usize size) {
    size = align(size, PAGE_SIZE);
    auto memory = new Memory(size);

    return memory;
}

void MMU::FreeMemory(MemoryBase* memory) { delete memory; }

uptr MMU::GetMemoryPtr(MemoryBase* memory) const {
    return static_cast<Memory*>(memory)->GetPtr();
}

void MMU::Map(vaddr va, usize size, MemoryBase* memory,
              const Horizon::MemoryState state) {
    ASSERT_ALIGNMENT(size, PAGE_SIZE, Dynarmic, "size");
    // TODO
}

void MMU::Map(vaddr dst_va, vaddr src_va, usize size) {
    // TODO
}

void MMU::Unmap(vaddr va, usize size) {
    // TODO
}

void MMU::ResizeHeap(vaddr va, usize size) {
    // TODO
}

uptr MMU::UnmapAddr(vaddr va) const {
    // TODO
    return invalid<uptr>();
}

Horizon::MemoryInfo MMU::QueryMemory(vaddr va) const {
    LOG_NOT_IMPLEMENTED(Dynarmic, "Memory querying");
    return Horizon::MemoryInfo{};
}

} // namespace Hydra::HW::TegraX1::CPU::Dynarmic
