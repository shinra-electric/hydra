#pragma once

#include "common/common.hpp"

namespace Hydra::HW {

template <typename T, typename Impl> class GenericMMU {
  public:
    void Map(uptr base, Impl impl) {
        mapped_ranges[base] = impl;
        ((T*)this)->MapImpl(base, impl);
    }

    void Unmap(uptr base) {
        auto it = mapped_ranges.find(base);
        ASSERT_DEBUG(it != mapped_ranges.end(), MMU,
                     "Failed to unmap with base 0x{:08x}", base);
        ((T*)this)->UnmapImpl(base, it->second);
        mapped_ranges.erase(it);
    }

    void Remap(uptr base) {
        auto impl = mapped_ranges.at(base);
        ((T*)this)->UnmapImpl(base, impl);
        ((T*)this)->MapImpl(base, impl);
    }

    Impl FindAddrImpl(uptr addr, uptr& out_base) const {
        for (auto [base, impl] : mapped_ranges) {
            if (addr >= base && addr < base + ((T*)this)->ImplGetSize(impl)) {
                out_base = base;
                return impl;
            }
        }

        LOG_ERROR(MMU, "Failed to find impl for addr 0x{:08x}", addr);

        return nullptr;
    }

  private:
    std::map<uptr, Impl> mapped_ranges;
};

} // namespace Hydra::HW
