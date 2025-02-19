#pragma once

#include "hypervisor/const.hpp"

namespace Hydra::Horizon {
class OS;
}

namespace Hydra::HW::TegraX1::MMU::Hypervisor {
class MMU;
}

namespace Hydra::HW::TegraX1::CPU::Hypervisor {
class CPU;
}

namespace Hydra::Hypervisor {

class Hypervisor {
  public:
    Hypervisor(Horizon::OS& horizon_);
    ~Hypervisor();

    void LoadROM(Rom* rom);

    void Run();

  private:
    Horizon::OS& horizon;

    HW::TegraX1::MMU::Hypervisor::MMU* mmu;
    HW::TegraX1::CPU::Hypervisor::CPU* cpu;

    void DataAbort(u32 instruction, u64 far, u64 elr);

    // Interpreter
    void InterpretLDAXR(u8 size0, u8 out_reg, u64 addr);
    void InterpretSTLXR(u8 size0, u8 out_res_reg, u8 reg, u64 addr);
    void InterpretDC(u64 addr);
    void InterpretLDR(u8 size0, u8 size1, u8 out_reg, u64 addr);
    void InterpretSTR(u8 size0, u8 size1, u8 reg, u64 addr);
    void InterpretLDP(u8 size0, u8 size1, u8 out_reg0, u8 out_reg1, u64 addr);
    void InterpretSTP(u8 size0, u8 size1, u8 reg0, u8 reg1, u64 addr);
};

} // namespace Hydra::Hypervisor
