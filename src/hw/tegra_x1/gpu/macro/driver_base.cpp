#include "hw/tegra_x1/gpu/macro/driver_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Macro {

void DriverBase::Execute(u32 index) {
    ExecuteImpl(macro_start_address_ram[index]);
}

void DriverBase::LoadInstructionRamPointer(u32 ptr) {
    macro_instruction_ram_ptr = ptr;
}

void DriverBase::LoadInstructionRam(u32 data) {
    macro_instruction_ram[macro_instruction_ram_ptr++] = data;
}

void DriverBase::LoadStartAddressRamPointer(u32 ptr) {
    macro_start_address_ram_ptr = ptr;
}

void DriverBase::LoadStartAddressRam(u32 data) {
    macro_start_address_ram[macro_start_address_ram_ptr++] = data;
}

} // namespace Hydra::HW::TegraX1::GPU::Macro
