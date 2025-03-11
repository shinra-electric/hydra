#include "hw/tegra_x1/gpu/macro/interpreter/driver.hpp"

namespace Hydra::HW::TegraX1::GPU::Macro::Interpreter {

void Driver::ExecuteImpl(u32 pc) {
    LOG_NOT_IMPLEMENTED(GPU, "Macro (PC: 0x{:08x})", pc);
}

} // namespace Hydra::HW::TegraX1::GPU::Macro::Interpreter
