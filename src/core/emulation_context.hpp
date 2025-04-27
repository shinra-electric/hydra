#pragma once

#include "core/horizon/kernel/process.hpp"
#include "core/horizon/os.hpp"
#include "core/hw/bus.hpp"
#include "core/hw/display/display.hpp"
#include "core/hw/tegra_x1/cpu/cpu_base.hpp"
#include "core/hw/tegra_x1/gpu/gpu.hpp"

namespace Hydra {

class EmulationContext {
  public:
    EmulationContext();
    ~EmulationContext();

    void SetSurface(void* surface) { gpu->GetRenderer()->SetSurface(surface); }
    void LoadRom(const std::string& rom_filename);
    void Run();

    void Present();

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

    Hydra::Horizon::Kernel::Process* process;

    std::atomic_bool is_running = false;
};

} // namespace Hydra
