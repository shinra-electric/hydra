#pragma once

#include "hw/tegra_x1/gpu/const.hpp"
#include "hw/tegra_x1/gpu/gpu_mmu.hpp"

namespace Hydra::HW::TegraX1::CPU {
class MMUBase;
}

namespace Hydra::HW::TegraX1::GPU {

class GPUMMU;

class Pfifo {
  public:
    Pfifo(GPUMMU& gpu_mmu_) : gpu_mmu{gpu_mmu_} {}

    // TODO: use std::span instead
    void SubmitEntries(const std::vector<GpfifoEntry>& entries);

  private:
    GPUMMU& gpu_mmu;

    void SubmitEntry(const GpfifoEntry entry);
    uptr SubmitCommand(uptr gpu_addr);

    // Helpers
    template <typename T> T Read(uptr& gpu_addr) {
        T word = gpu_mmu.Load<T>(gpu_addr);
        gpu_addr += sizeof(T);

        return word;
    }

    void ProcessMethodArg(Subchannel subchannel, uptr& gpu_addr, u32& offset,
                          bool increment);
};

} // namespace Hydra::HW::TegraX1::GPU
