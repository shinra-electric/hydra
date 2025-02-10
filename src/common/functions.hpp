#pragma once

#include <assert.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/mman.h>

#include "common/types.hpp"

namespace Hydra {

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

inline std::ifstream open_file(const std::string& path, usize& size) {
    const auto iflags = std::ios::in | std::ios::binary | std::ios::ate;
    auto ifs = std::ifstream{path, iflags};
    size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    return ifs;
}

class Writer {
  public:
    Writer(u8* base_) : base{base_}, ptr{base_} {}

    template <typename T> T* Write(const T& value) {
        T* result = reinterpret_cast<T*>(ptr);
        *result = value;
        ptr += sizeof(T);

        return result;
    }

    // Getters
    u8* GetBase() const { return base; }

    usize GetWrittenSize() const { return ptr - base; }

  private:
    u8* base;
    u8* ptr;
};

} // namespace Hydra
