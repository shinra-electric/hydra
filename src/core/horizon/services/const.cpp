#include "core/horizon/services/const.hpp"

#include "core/hw/tegra_x1/cpu/mmu_base.hpp"

namespace Hydra::Horizon::Services {

u8* get_buffer_ptr(const HW::TegraX1::CPU::MMUBase* mmu,
                   const Hipc::BufferDescriptor& descriptor, usize size) {
    uptr addr = descriptor.address_low | (u64)descriptor.address_mid << 32 |
                (u64)descriptor.address_high << 36;
    if (addr == 0x0)
        return nullptr;

    size = descriptor.size_low | (usize)descriptor.size_high << 32;
    if (size == 0x0)
        return nullptr;

    return reinterpret_cast<u8*>(mmu->UnmapAddr(addr));
}

u8* get_static_ptr(const HW::TegraX1::CPU::MMUBase* mmu,
                   const Hipc::StaticDescriptor& descriptor, usize size) {
    uptr addr = descriptor.address_low | (u64)descriptor.address_mid << 32 |
                (u64)descriptor.address_high << 36;
    if (addr == 0x0)
        return nullptr;

    size = descriptor.size;
    if (size == 0x0)
        return nullptr;

    return reinterpret_cast<u8*>(mmu->UnmapAddr(addr));
}

u8* get_list_entry_ptr(const HW::TegraX1::CPU::MMUBase* mmu,
                       const Hipc::RecvListEntry& descriptor, usize size) {
    uptr addr = descriptor.address_low | (u64)descriptor.address_high << 32;
    if (addr == 0x0)
        return nullptr;

    size = descriptor.size;
    if (size == 0x0)
        return nullptr;

    return reinterpret_cast<u8*>(mmu->UnmapAddr(addr));
}

} // namespace Hydra::Horizon::Services
