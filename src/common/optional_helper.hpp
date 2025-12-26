#pragma once

#include <optional>

namespace hydra {

template <typename T>
T* unwrap_or_null(std::optional<T>& opt) {
    return opt ? std::addressof(opt.value()) : nullptr;
}

template <typename T>
const T* unwrap_or_null(const std::optional<T>& opt) {
    return opt ? std::addressof(opt.value()) : nullptr;
}

} // namespace hydra
