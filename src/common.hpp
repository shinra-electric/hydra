#pragma once

#include <assert.h>
#include <errno.h>
#include <fstream>
#include <span>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/mman.h>
#include <thread>

#include "nx_parser.hpp"

// TODO: move this to a separate library
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using usize = size_t;
using uptr = uintptr_t;

#define EXTRACT_BITS(value, high, low)                                         \
    (((value) >> (low)) & ((1 << ((high) - (low) + 1)) - 1))

#define BIT(n) (1 << (n))
#define ENABLE_ENUM_BITMASK_OPERATORS(x)                                       \
    inline x operator|(x a, x b) {                                             \
        return static_cast<x>(static_cast<u32>(a) | static_cast<u32>(b));      \
    }                                                                          \
    inline x& operator|=(x& a, x b) { return a = a | b; }                      \
    inline x operator&(x a, x b) {                                             \
        return static_cast<x>(static_cast<u32>(a) & static_cast<u32>(b));      \
    }                                                                          \
    inline x& operator&=(x& a, x b) { return a = a & b; }                      \
    inline x operator~(x a) { return static_cast<x>(~static_cast<u32>(a)); }   \
    inline bool any(x a) { return a != x::None; }

template <typename T, typename SrcT> T bit_cast(SrcT src) {
    static_assert(sizeof(T) == sizeof(SrcT));
    return *reinterpret_cast<T*>(&src);
}

template <typename T> T align(T v, T alignment) {
    return (v + alignment - 1) & ~(alignment - 1);
}

inline std::ifstream OpenFile(const std::string& path, usize& size) {
    const auto iflags = std::ios::in | std::ios::binary | std::ios::ate;
    auto ifs = std::ifstream{path, iflags};
    size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    return ifs;
}
