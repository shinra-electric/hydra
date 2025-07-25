#pragma once

#include "common/type_aliases.hpp"

namespace hydra {

inline unsigned long long operator"" _KiB(unsigned long long x) {
    return x * 1024;
}

inline unsigned long long operator"" _MiB(unsigned long long x) {
    return x * 1024_KiB;
}

inline unsigned long long operator"" _GiB(unsigned long long x) {
    return x * 1024_MiB;
}

inline unsigned long long operator"" _TiB(unsigned long long x) {
    return x * 1024_GiB;
}

// TODO: make sure the string's length doesn't exceed 8 characters
inline constexpr u64 operator"" _u64(const char* str, unsigned long len) {
    u64 res = 0;
    for (u32 i = 0; i < len; i++)
        res |= u64(str[i]) << (i * 8);

    return res;
}

inline constexpr const char* operator"" _str(u64 value) {
    return reinterpret_cast<const char*>(&value);
}

} // namespace hydra
