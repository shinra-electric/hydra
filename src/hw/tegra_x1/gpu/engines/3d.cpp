#include "hw/tegra_x1/gpu/engines/3d.hpp"

namespace Hydra::HW::TegraX1::GPU::Engines {

DEFINE_METHOD_TABLE(ThreeD, 0x45, LoadMmeInstructionRamPointer, 0x46,
                    LoadMmeStartAddressRamPointer, 0x47,
                    LoadMmeStartAddressRamPointer, 0x48, LoadMmeStartAddressRam)

void ThreeD::Macro(u32 method, u32 arg) {
    u32 index = (method - MACRO_METHODS_REGION) >> 1;
    LOG_NOT_IMPLEMENTED(GPU, "Macro (index: 0x{:08x})", index);
    if ((method & 0x1) == 0x0) {
        u32 macro_start = macro_start_address_ram[index];
        LOG_DEBUG(GPU, "Macro start: 0x{:08x}", macro_start);
        LOG_DEBUG(GPU, "Parameter1: 0x{:08x}", arg);
        // TODO: upload arg to reg1
        // TODO: execute macro
    } else {
        LOG_DEBUG(GPU, "ParameterN: 0x{:08x}", arg);
        // TODO: upload arg to regX (TODO: what is X?)
    }
}

void ThreeD::LoadMmeInstructionRamPointer(u32 ptr) {
    macro_instruction_ram_ptr = ptr;
}

void ThreeD::LoadMmeInstructionRam(u32 data) {
    macro_instruction_ram[macro_instruction_ram_ptr++] = data;
}

void ThreeD::LoadMmeStartAddressRamPointer(u32 ptr) {
    macro_start_address_ram_ptr = ptr;
}

void ThreeD::LoadMmeStartAddressRam(u32 data) {
    macro_start_address_ram[macro_start_address_ram_ptr++] = data;
}

} // namespace Hydra::HW::TegraX1::GPU::Engines
