#pragma once

#include "core/horizon/kernel/const.hpp"

namespace hydra::horizon::kernel {

class AutoObject {
  public:
    AutoObject(const std::string_view debug_name_ = "AutoObject")
        : debug_name{debug_name_}, ref_count{0} {}

    void Retain() { ref_count++; }

    // Returns true if the object has been deallocated
    bool Release() {
        // TODO: do only 1 atomic operation
        ref_count--;
        if (ref_count == 0) {
            delete this;
            return true;
        }

        return false;
    }

    std::string GetDebugName() const {
        return fmt::format("{} {}", debug_name, (void*)this);
    }

  protected:
    virtual ~AutoObject() {}

  private:
    std::string debug_name;

    std::atomic<u32> ref_count;
};

} // namespace hydra::horizon::kernel
