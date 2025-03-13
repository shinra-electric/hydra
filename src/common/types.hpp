#pragma once

#include <map>
#include <stdint.h>
#include <string>

#include "common/macros.hpp"

namespace Hydra {

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using i128 = __int128_t;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using u128 = __uint128_t;
using usize = size_t;
using uptr = uintptr_t;
using f32 = float;
using f64 = double;
using HandleId = u32;

template <typename T> struct range {
  public:
    range() : base{0}, size{0} {}
    range(T base_, usize size_) : base{base_}, size{size_} {}

    // Getters
    T GetBase() const { return base; }
    usize GetSize() const { return size; }

  private:
    T base;
    usize size;
};

template <typename T> class readonly {
  public:
    readonly() {}
    readonly(const T& value_) : value{value_} {}
    void operator=(const T&) = delete;

    operator T() const { return value; }

    // Getters
    const T& Get() const { return value; }

  private:
    T value;
} __attribute__((packed));

template <typename T> class writeonly {
  public:
    writeonly() {}
    writeonly(const T& value_) : value{value_} {}
    void operator=(const T& new_value) { value = new_value; }

    // Getters
    const T& Get() const { return value; }

  private:
    T value;
} __attribute__((packed));

template <typename T> class readwrite {
  public:
    readwrite() {}
    readwrite(const T& value_) : value{value_} {}
    void operator=(const T& new_value) { value = new_value; }

    operator T() const { return value; }

    // Getters
    const T& Get() const { return value; }

  private:
    T value;
} __attribute__((packed));

class Reader {
  public:
    Reader(u8* base_) : base{base_}, ptr{base_} {}

    void JumpToOffset(u32 offset) { ptr = base + offset; }

    template <typename T> T* ReadPtr() {
        T* result = reinterpret_cast<T*>(ptr);
        ptr += sizeof(T);

        return result;
    }

    template <typename T> T Read() { return *ReadPtr<T>(); }

    template <typename T> T* Read(T* read_ptr, usize count) {
        T* result = reinterpret_cast<T*>(ptr);
        ptr += sizeof(T) * count;

        return result;
    }

    std::string ReadString() {
        std::string result(reinterpret_cast<char*>(ptr));
        ptr += result.size();

        return result;
    }

    // Getters
    u8* GetBase() const { return base; }

  private:
    u8* base;
    u8* ptr;
};

class Writer {
  public:
    Writer(u8* base_) : base{base_}, ptr{base_} {}

    template <typename T> T* Write(const T& value) {
        T* result = reinterpret_cast<T*>(ptr);
        *result = value;
        ptr += sizeof(T);

        return result;
    }

    template <typename T> T* Write(const T* write_ptr, usize count) {
        T* result = reinterpret_cast<T*>(ptr);
        memcpy(result, write_ptr, sizeof(T) * count);
        ptr += sizeof(T) * count;

        return result;
    }

    // Getters
    u8* GetBase() const { return base; }

    usize GetWrittenSize() const { return ptr - base; }

  private:
    u8* base;
    u8* ptr;
};

template <typename SubclassT, typename T, typename DescriptorT>
class CacheBase {
  public:
    ~CacheBase() {
        for (const auto& [key, value] : cache) {
            THIS->DestroyElement(value);
        }

        THIS->Destroy();
    }

    T Find(const DescriptorT& descriptor) {
        u64 hash = THIS->Hash(descriptor);
        auto& element = cache[hash];
        if (element) { // TODO: make this so that non-pointer types are
                       // supported
                       // as well
            THIS->Update(element);
            return element;
        }

        element = THIS->Create(descriptor);

        return element;
    }

  private:
    std::map<u64, T> cache;
};

} // namespace Hydra
