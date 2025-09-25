#pragma once

#include "type_aliases.hpp"

namespace hydra {

template <typename T>
void atomic_store(T* ptr, T value) {
    std::atomic_ref ref(*ptr);
    ref.store(value);
}

template <typename T>
T atomic_load(T* ptr) {
    std::atomic_ref ref(*ptr);
    return ref.load();
}

template <typename T>
T atomic_exchange(T* ptr, T value) {
    std::atomic_ref ref(*ptr);
    return ref.exchange(value);
}

template <typename T>
bool atomic_compare_exchange_weak(T* ptr, T& expected, T desired) {
    std::atomic_ref ref(*ptr);
    return ref.compare_exchange_weak(expected, desired);
}

template <typename T>
bool atomic_compare_exchange_strong(T* ptr, T& expected, T desired) {
    std::atomic_ref ref(*ptr);
    return ref.compare_exchange_strong(expected, desired);
}

template <typename T>
T atomic_fetch_add(T* ptr, T value) {
    std::atomic_ref ref(*ptr);
    return ref.fetch_add(value);
}

template <typename T>
T atomic_fetch_sub(T* ptr, T value) {
    std::atomic_ref ref(*ptr);
    return ref.fetch_sub(value);
}

} // namespace hydra
