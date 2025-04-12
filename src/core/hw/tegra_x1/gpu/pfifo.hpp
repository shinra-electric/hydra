#pragma once

#include "core/hw/tegra_x1/gpu/const.hpp"
#include "core/hw/tegra_x1/gpu/gpu_mmu.hpp"

namespace Hydra::HW::TegraX1::CPU {
class MMUBase;
}

namespace Hydra::HW::TegraX1::GPU {

class GPUMMU;

class Pfifo {
  public:
    Pfifo(GPUMMU& gpu_mmu_) : gpu_mmu{gpu_mmu_} {}

    // TODO: use std::span instead
    void SubmitEntries(const std::vector<GpfifoEntry>& entries,
                       GpfifoFlags flags);

  private:
    GPUMMU& gpu_mmu;

    void SubmitEntry(const GpfifoEntry entry);
    bool SubmitCommand(uptr& gpu_addr); // TODO: return void

    // Helpers
    template <typename T> T Read(uptr& gpu_addr) {
        T word = gpu_mmu.Load<T>(gpu_addr);
        gpu_addr += sizeof(T);

        return word;
    }

    void ProcessMethodArg(u32 subchannel, uptr& gpu_addr, u32& method,
                          bool increment);
};

} // namespace Hydra::HW::TegraX1::GPU
