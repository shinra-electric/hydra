#pragma once

#include <assert.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/mman.h>

#include "common/type_aliases.hpp"

namespace Hydra {

[[noreturn]] inline void unreachable() {
#if defined(_MSC_VER) && !defined(__clang__) // MSVC
    __assume(false);
#else // GCC, Clang
    __builtin_unreachable();
#endif
}

template <typename T> constexpr T invalid() {
    return std::numeric_limits<T>::max();
}

template <typename T, u64 b, u64 count> constexpr T mask() {
    return ((1u << count) - 1u) << b;
}

template <typename T, u64 b, u64 count, typename SrcT>
T extract_bits(SrcT src) {
    return static_cast<T>((src >> b) & mask<T, 0, count>());
}

template <typename T, u32 bit_count> T sign_extend(T v) {
    static_assert(std::is_signed<T>::value);
    static_assert(bit_count < sizeof(T) * 8);
    T const m = T(1) << (bit_count - 1);
    return (v ^ m) - m;
}

template <typename T> T align(T v, T alignment) {
    return (v + alignment - 1) & ~(alignment - 1);
}

template <typename T> T align_down(T v, T alignment) {
    return v & ~(alignment - 1);
}

template <typename PtrT, typename AlignmentT>
PtrT* align_ptr(PtrT* ptr, AlignmentT alignment) {
    return reinterpret_cast<PtrT*>(
        align(reinterpret_cast<u64>(ptr), static_cast<u64>(alignment)));
}

inline uptr make_addr(u32 lo, u32 hi) {
    return (static_cast<uptr>(hi) << 32) | lo;
}

template <typename T> inline T ceil_divide(T dividend, T divisor) {
    return (dividend + divisor - 1) / divisor;
}

inline std::string to_lower(const std::string& str) {
    // TODO: make this more efficient?
    std::string result;
    std::transform(str.begin(), str.end(), std::back_inserter(result),
                   [](unsigned char c) { return std::tolower(c); });

    return result;
}

inline std::string to_upper(const std::string& str) {
    // TODO: make this more efficient?
    std::string result;
    std::transform(str.begin(), str.end(), std::back_inserter(result),
                   [](unsigned char c) { return std::toupper(c); });

    return result;
}

inline constexpr uint64_t str_to_u64(const char* str, size_t idx = 0,
                                     uint64_t result = 0) {
    return (str[idx] == '\0' || idx >= 8)
               ? result
               : str_to_u64(str, idx + 1,
                            result |
                                (static_cast<uint64_t>(str[idx]) << (idx * 8)));
}

inline std::string u64_to_str(u64 value) {
    char* str = reinterpret_cast<char*>(&value);
    return std::string(str, std::min(strlen(str), (usize)8));
}

template <typename T> void push_unique(std::vector<T>& vec, T value) {
    auto it = std::find_if(vec.begin(), vec.end(),
                           [&](const T v) { return v == value; });
    if (it == vec.end())
        vec.push_back(value);
}

inline std::ifstream open_file(const std::string& path, usize& out_size) {
    const auto iflags = std::ios::in | std::ios::binary | std::ios::ate;
    auto ifs = std::ifstream{path, iflags};
    out_size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    return ifs;
}

inline usize get_file_size(const std::string& path) {
    usize size = 0;
    auto ifs = open_file(path, size);
    ifs.close();

    return size;
}

// HACK
template <typename T> T rotl(T v, u64 shift) {
    return (v << shift) | (v >> (32 - shift));
}

} // namespace Hydra
