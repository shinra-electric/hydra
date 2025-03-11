#include "hw/tegra_x1/gpu/engines/3d.hpp"

#include "hw/tegra_x1/gpu/macro/interpreter/driver.hpp"

namespace Hydra::HW::TegraX1::GPU::Engines {

DEFINE_METHOD_TABLE(ThreeD, 0x45, LoadMmeInstructionRamPointer, 0x46,
                    LoadMmeInstructionRam, 0x47, LoadMmeStartAddressRamPointer,
                    0x48, LoadMmeStartAddressRam)

ThreeD::ThreeD() {
    // TODO: choose based on Macro backend
    {
        macro_driver = new Macro::Interpreter::Driver();
    }
}

void ThreeD::Macro(u32 method, u32 arg) {
    u32 index = (method - MACRO_METHODS_REGION) >> 1;
    LOG_DEBUG(GPU, "Macro (index: 0x{:08x})", index);
    if ((method & 0x1) == 0x0) {
        LOG_DEBUG(GPU, "Parameter1: 0x{:08x}", arg);
        // TODO: upload arg to reg1
        macro_driver->Execute(index);
    } else {
        LOG_DEBUG(GPU, "ParameterN: 0x{:08x}", arg);
        // TODO: upload arg to regX (TODO: what is X?)
    }
}

void ThreeD::LoadMmeInstructionRamPointer(u32 ptr) {
    macro_driver->LoadInstructionRamPointer(ptr);
}

void ThreeD::LoadMmeInstructionRam(u32 data) {
    macro_driver->LoadInstructionRam(data);
}

void ThreeD::LoadMmeStartAddressRamPointer(u32 ptr) {
    macro_driver->LoadStartAddressRamPointer(ptr);
}

void ThreeD::LoadMmeStartAddressRam(u32 data) {
    macro_driver->LoadStartAddressRam(data);
}

} // namespace Hydra::HW::TegraX1::GPU::Engines
