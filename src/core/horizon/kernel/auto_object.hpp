#pragma once

#include "core/horizon/kernel/const.hpp"

namespace hydra::horizon::kernel {

class AutoObject {
  public:
    AutoObject(const std::string_view debug_name_ = "AutoObject")
        : debug_name{fmt::format("{} {}", debug_name_, (void*)this)}, ref_count{
                                                                          1} {}

    void Retain() { ref_count.fetch_add(1, std::memory_order_relaxed); }

    // Returns true if the object has been deallocated
    bool Release() {
        if (ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            // TODO: this assumes that the object is heap allocated, but that
            // may not always be the case
            delete this;
            return true;
        }

        return false;
    }

    std::string_view GetDebugName() const { return debug_name; }

  protected:
    virtual ~AutoObject() {}

  private:
    std::string debug_name;

    std::atomic<u32> ref_count;
};

} // namespace hydra::horizon::kernel
