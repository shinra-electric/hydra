#pragma once

#include "horizon/os.hpp"
#include "hw/bus.hpp"
#include "hw/display/display.hpp"
#include "hw/tegra_x1/cpu/cpu_base.hpp"
#include "hw/tegra_x1/gpu/gpu.hpp"
#include <atomic>

namespace Hydra {

class EmulationContext {
  public:
    EmulationContext();
    ~EmulationContext();

    void Start(const std::string& rom_filename);

    // Getters
    Hydra::HW::TegraX1::CPU::CPUBase* GetCPU() const { return cpu; }
    Hydra::HW::TegraX1::GPU::GPU* GetGPU() const { return gpu; }
    Hydra::HW::Bus* GetBus() const { return bus; }
    Hydra::Horizon::OS* GetOS() const { return os; }

    bool IsRunning() const { return is_running; }

  private:
    Hydra::HW::TegraX1::CPU::CPUBase* cpu;
    Hydra::HW::TegraX1::GPU::GPU* gpu;
    Hydra::HW::Display::Display* builtin_display;
    Hydra::HW::Bus* bus;
    Hydra::Horizon::OS* os;

    std::atomic_bool is_running = false;
    std::vector<std::thread*> threads;
};

} // namespace Hydra
