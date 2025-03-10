#pragma once

#include "common/common.hpp"

#define METHOD_CASE(method, func)                                              \
    case method:                                                               \
        func(arg);                                                             \
        break;

#define DEFINE_METHOD_TABLE(type, ...)                                         \
    void type::Method(u32 method, u32 arg) {                                   \
        /* TODO: assert valid method */                                        \
        switch (method) {                                                      \
            FOR_EACH_0_2(METHOD_CASE, __VA_ARGS__)                             \
        default:                                                               \
            LOG_NOT_IMPLEMENTED(GPU, "Method 0x{:08x}", method);               \
            break;                                                             \
        }                                                                      \
    }

namespace Hydra::HW::TegraX1::GPU::Engines {

class EngineBase {
  public:
    virtual void Method(u32 method, u32 arg) = 0;

  private:
};

} // namespace Hydra::HW::TegraX1::GPU::Engines
