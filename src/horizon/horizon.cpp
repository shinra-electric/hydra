#include "horizon/horizon.hpp"

namespace Hydra::Horizon {

OS::OS() {}

OS::~OS() {}

void OS::SetMMU(HW::MMU::MMUBase* mmu) { kernel.SetMMU(mmu); }

} // namespace Hydra::Horizon
