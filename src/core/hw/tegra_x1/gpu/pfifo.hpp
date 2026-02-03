#pragma once

#include "core/hw/tegra_x1/gpu/const.hpp"
#include "core/hw/tegra_x1/gpu/gmmu.hpp"

namespace hydra::hw::tegra_x1::cpu {
class IMmu;
}

namespace hydra::hw::tegra_x1::gpu {

class GMmu;

class Pfifo {
  public:
    // TODO: use std::span instead
    void SubmitEntries(GMmu& gmmu, const std::vector<GpfifoEntry>& entries,
                       GpfifoFlags flags);

  private:
    void SubmitEntry(const GpfifoEntry entry);
    bool SubmitCommand(uptr& gpu_addr); // TODO: return void

    // Helpers
    void ProcessMethodArg(u32 subchannel, uptr& gpu_addr, u32& method,
                          bool increment);
};

} // namespace hydra::hw::tegra_x1::gpu
