#pragma once

#include <assert.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/mman.h>

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

template <typename T, u32 bit_count> T sign_extend(T v) {
    static_assert(std::is_signed<T>::value);
    static_assert(bit_count < sizeof(T) * 8);
    T const m = T(1) << (bit_count - 1);
    return (v ^ m) - m;
}

inline uptr make_addr(u32 lo, u32 hi) {
    return (static_cast<uptr>(hi) << 32) | lo;
}

inline std::ifstream open_file(const std::string& path, usize& size) {
    const auto iflags = std::ios::in | std::ios::binary | std::ios::ate;
    auto ifs = std::ifstream{path, iflags};
    size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    return ifs;
}

} // namespace Hydra
