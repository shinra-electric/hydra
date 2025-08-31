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
    void SubmitEntry(GMmu& gmmu, const GpfifoEntry entry);
    bool SubmitCommand(GMmu& gmmu, uptr& gpu_addr); // TODO: return void

    // Helpers
    template <typename T>
    T Read(GMmu& gmmu, uptr& gpu_addr) {
        T word = gmmu.Load<T>(gpu_addr);
        gpu_addr += sizeof(T);

        return word;
    }

    void ProcessMethodArg(GMmu& gmmu, u32 subchannel, uptr& gpu_addr,
                          u32& method, bool increment);
};

} // namespace hydra::hw::tegra_x1::gpu
