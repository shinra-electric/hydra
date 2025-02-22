#pragma once

#include "horizon/os.hpp"
#include "hw/bus.hpp"
#include "hw/display/display.hpp"
#include "hw/tegra_x1/cpu/cpu_base.hpp"
#include "hw/tegra_x1/cpu/hypervisor/cpu.hpp"
#include "hw/tegra_x1/cpu/hypervisor/thread.hpp"
#include "hw/tegra_x1/gpu/gpu.hpp"

namespace Hydra {

class EmulationContext {
  public:
    EmulationContext(const std::string& rom_filename);
    ~EmulationContext();

    void Start();

    // Getters
    Hydra::HW::TegraX1::CPU::CPUBase* GetCPU() const { return cpu; }
    Hydra::HW::TegraX1::GPU::GPU* GetGPU() const { return gpu; }
    Hydra::HW::Bus* GetBus() const { return bus; }
    Hydra::Horizon::OS* GetOS() const { return os; }

  private:
    Hydra::HW::TegraX1::CPU::CPUBase* cpu;
    Hydra::HW::TegraX1::GPU::GPU* gpu;
    Hydra::HW::Display::Display* builtin_display;
    Hydra::HW::Bus* bus;
    Hydra::Horizon::OS* os;

    std::vector<std::thread*> threads;
};

} // namespace Hydra
