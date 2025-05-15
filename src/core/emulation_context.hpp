#pragma once

#include "core/horizon/kernel/process.hpp"
#include "core/horizon/os.hpp"
#include "core/hw/bus.hpp"
#include "core/hw/display/display.hpp"
#include "core/hw/tegra_x1/cpu/cpu_base.hpp"
#include "core/hw/tegra_x1/gpu/gpu.hpp"

namespace hydra {

class EmulationContext {
  public:
    EmulationContext();
    ~EmulationContext();

    void SetSurface(void* surface) { gpu->GetRenderer()->SetSurface(surface); }
    void LoadRom(const std::string& rom_filename);
    void Run();

    void Present(u32 width, u32 height);

    // Getters
    hw::tegra_x1::cpu::CPUBase* GetCPU() const { return cpu; }
    hw::tegra_x1::gpu::GPU* GetGPU() const { return gpu; }
    hw::Bus* GetBus() const { return bus; }
    horizon::OS* GetOS() const { return os; }

    bool IsRunning() const { return is_running; }

  private:
    Config config;

    hw::tegra_x1::cpu::CPUBase* cpu;
    hw::tegra_x1::gpu::GPU* gpu;
    hw::display::Display* builtin_display;
    hw::Bus* bus;
    horizon::OS* os;

    horizon::kernel::Process* process;

    std::atomic_bool is_running = false;
};

} // namespace hydra
