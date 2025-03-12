#include "hw/tegra_x1/gpu/engines/3d.hpp"

#include "hw/tegra_x1/gpu/macro/interpreter/driver.hpp"

namespace Hydra::HW::TegraX1::GPU::Engines {

DEFINE_METHOD_TABLE(ThreeD, 0x45, LoadMmeInstructionRamPointer, 0x46,
                    LoadMmeInstructionRam, 0x47, LoadMmeStartAddressRamPointer,
                    0x48, LoadMmeStartAddressRam, 0x8c4, FirmwareCall4)

ThreeD::ThreeD() {
    // TODO: choose based on Macro backend
    {
        macro_driver = new Macro::Interpreter::Driver(this);
    }
}

void ThreeD::Macro(u32 method, u32 arg) {
    u32 index = (method - MACRO_METHODS_REGION) >> 1;
    LOG_DEBUG(Engines, "Macro (index: 0x{:08x})", index);
    if ((method & 0x1) == 0x0) {
        LOG_DEBUG(Engines, "Parameter1: 0x{:08x}", arg);
        macro_driver->SetIndex(index);
        macro_driver->LoadParam1(arg);
    } else {
        LOG_DEBUG(Engines, "ParameterN: 0x{:08x}", arg);
        macro_driver->LoadParam(arg);
    }
}

void ThreeD::FlushMacro() { macro_driver->Execute(); }

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

void ThreeD::FirmwareCall4(u32 data) {
    LOG_NOT_IMPLEMENTED(Engines, "Firmware call 4");

    // TODO: find out what this does
    regs.mme_firmware_args[0] = 0x1;
}

} // namespace Hydra::HW::TegraX1::GPU::Engines
