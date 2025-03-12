#pragma once

#include "common/logging/log.hpp"
#include "hw/tegra_x1/gpu/const.hpp"

#define METHOD_CASE(method, func, arg_type)                                    \
    case method:                                                               \
        func(bit_cast<arg_type>(arg));                                         \
        break;

#define DEFINE_METHOD_TABLE(type, ...)                                         \
    void type::Method(u32 method, u32 arg) {                                   \
        if (method >= MACRO_METHODS_REGION) {                                  \
            Macro(method, arg);                                                \
            return;                                                            \
        }                                                                      \
        switch (method) {                                                      \
            FOR_EACH_0_3(METHOD_CASE, __VA_ARGS__)                             \
        default:                                                               \
            WriteReg(method, arg);                                             \
            break;                                                             \
        }                                                                      \
    }

namespace Hydra::HW::TegraX1::GPU::Engines {

class EngineBase {
  public:
    virtual void Method(u32 method, u32 arg) = 0;

    virtual void FlushMacro() {
        LOG_ERROR(Engines, "This engine does not support macros");
    }

  protected:
    virtual void WriteReg(u32 reg, u32 value) {
        LOG_NOT_IMPLEMENTED(
            Engines, "Writing to registers (0x{:08x}) in this engine", reg);
    }

    virtual void Macro(u32 method, u32 arg) {
        LOG_ERROR(Engines,
                  "This engine does not support macros (method: 0x{:08x})",
                  method);
    }
};

} // namespace Hydra::HW::TegraX1::GPU::Engines
