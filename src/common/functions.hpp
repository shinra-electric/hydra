#pragma once

#include <assert.h>
#include <fstream>
#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/mman.h>

#include "common/logging/log.hpp"
#include "common/types.hpp"

namespace Hydra {

template <typename T> constexpr T invalid() {
    return std::numeric_limits<T>::max();
}

template <typename T, typename SrcT> T bit_cast(SrcT src) {
    static_assert(sizeof(T) == sizeof(SrcT));
    return *reinterpret_cast<T*>(&src);
}

template <typename T> T align(T v, T alignment) {
    return (v + alignment - 1) & ~(alignment - 1);
}

template <typename PtrT, typename AlignmentT>
PtrT* align_ptr(PtrT* ptr, AlignmentT alignment) {
    return reinterpret_cast<PtrT*>(
        align(reinterpret_cast<u64>(ptr), static_cast<u64>(alignment)));
}

template <typename T> T floor_divide_u(T v, T d) { return v / d; }

template <typename T> T floor_divide_u_pow2(T v, u32 shift) {
    return v >> shift;
}

template <typename T> T ceil_divide_u(T v, T d) { return (v + d - 1ull) / d; }

template <typename T> T ceil_divide_u_pow2(T v, u32 shift) {
    return (v + (1ull << shift) - 1ull) >> shift;
}

inline std::ifstream open_file(const std::string& path, usize& size) {
    const auto iflags = std::ios::in | std::ios::binary | std::ios::ate;
    auto ifs = std::ifstream{path, iflags};
    size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    return ifs;
}

} // namespace Hydra
