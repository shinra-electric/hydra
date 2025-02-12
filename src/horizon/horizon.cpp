#include "horizon/horizon.hpp"

namespace Hydra::Horizon {

OS::OS() {}

OS::~OS() {}

void OS::SetDisplay(HW::Display::DisplayBase* display, u32 display_id) {
    kernel.SetDisplay(display, display_id);
}

void OS::SetMMU(HW::MMU::MMUBase* mmu) { kernel.SetMMU(mmu); }

} // namespace Hydra::Horizon
