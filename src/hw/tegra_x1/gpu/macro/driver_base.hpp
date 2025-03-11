#pragma once

#include "hw/tegra_x1/gpu/macro/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Macro {

class DriverBase {
  public:
    void Execute(u32 index);

    void LoadInstructionRamPointer(u32 ptr);
    void LoadInstructionRam(u32 data);
    void LoadStartAddressRamPointer(u32 ptr);
    void LoadStartAddressRam(u32 data);

  protected:
    virtual void ExecuteImpl(u32 pc) = 0;

  private:
    u32 macro_instruction_ram[0x1000] = {0}; // TODO: what should be the size?
    u32 macro_instruction_ram_ptr;
    u32 macro_start_address_ram[0x100] = {0}; // TODO: what should be the size?
    u32 macro_start_address_ram_ptr;
};

} // namespace Hydra::HW::TegraX1::GPU::Macro
